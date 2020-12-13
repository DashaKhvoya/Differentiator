#include "diff.h"

Node* CreateNode(size_t type, Node::node_type key, Node* left, Node* right) {
  Node* node = (Node*)calloc(1, sizeof(Node));
  node -> type = type;
  node -> key = key;
  node -> left = left;
  node -> right = right;

  return node;
}

Node* CopyTree(const Node* node) {
  if (node == nullptr) {
    return nullptr;
  }
  return CreateNode(node -> type, node -> key, CopyTree(node -> left), CopyTree(node -> right));
}

int SyntaxError(int error_place) {
  for (int i = 0; i < error_place; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^\n");
  printf ("Syntax Error on a poistion = [%d]\n", error_place);
  return -1;
}

Node* GetN(String* string) {
  Node* node = (Node*)calloc(1, sizeof(Node));

  if (isdigit(string -> str[string -> pos]) || string -> str[string -> pos] == '-') {
    node -> type = TYPE_CONST;
    int temp = 0;
    sscanf(string -> str + string -> pos, "%lg%n", &node -> key.dbl, &temp);
    string -> pos += temp;
  } else if (isalpha(string -> str[string -> pos])) {
    node -> type = TYPE_VAR;
    node -> key.var = string -> str[string -> pos];
    string -> pos++;
  } else {
    SyntaxError(string -> pos);
    return nullptr;
  }

  return node;
}

Node* GetP(String* string) {
  if (string -> str[string -> pos] == '(') {
    string -> pos++;
    Node* node = GetE(string);

    if (string -> str[string -> pos] != ')') {
      SyntaxError(string -> pos);
      return nullptr;
    }
    string -> pos++;
    return node;
  }

  return GetN(string);
}

Node* GetPow(String* string) {
  Node* node = GetP(string);

  while(string -> str[string -> pos] == '^') {
    Node* node_1 = node;
    string -> pos++;
    Node* node_2 = GetP(string);
    node = CreateNode(TYPE_BIN_OP, UnionOp(OP_POW), node_1, node_2);
    node_1 -> parent = node;
    node_2 -> parent = node;
  }

  return node;
}

Node* GetUnOp(String* string) {
  if (isalpha(string -> str[string -> pos]) && isalpha(string -> str[string -> pos + 1])) {
    char* arr = (char*)calloc(MAX_COMMAND_LENGTH, sizeof(char));
    sscanf(string -> str + string -> pos, "%[^()]s", arr);
    string -> pos += strlen(arr);

    for (size_t i = FIRST_UN_OP; i < NUMB_OF_OP; i++) {
      if (strcmp(arr, operations_names[i]) == 0) {
        Node* node_1 = GetPow(string);
        Node* node = CreateNode(TYPE_UN_OP, UnionOp(i), node_1, nullptr);
        node_1 -> parent = node;
        return node;
      }
    }
    SyntaxError(string -> pos);
    return nullptr;
  }

  return GetPow(string);
}

Node* GetT(String* string) {
  Node* node = GetUnOp(string);

  while(string -> str[string -> pos] == '*' || string -> str[string -> pos] == '/') {
    char oper = string -> str[string -> pos];
    string -> pos++;
    Node* node_1 = node;
    Node* node_2 = GetUnOp(string);

    if (oper == '*') {
      node = CreateNode(TYPE_BIN_OP, UnionOp(OP_MUL), node_1, node_2);
    } else {
      node = CreateNode(TYPE_BIN_OP, UnionOp(OP_DIV), node_1, node_2);
    }
    node_1 -> parent = node;
    node_2 -> parent = node;
  }
  return node;
}

Node* GetE(String* string) {
  Node* node = GetT(string);

  while(string -> str[string -> pos] == '+' || string -> str[string -> pos] == '-') {
    char oper = string -> str[string -> pos];
    string -> pos++;
    Node* node_1 = node;
    Node* node_2 = GetT(string);

    if (oper == '+') {
      node = CreateNode(TYPE_BIN_OP, UnionOp(OP_ADD), node_1, node_2);
    } else {
      node = CreateNode(TYPE_BIN_OP, UnionOp(OP_SUB), node_1, node_2);
    }
    node_1 -> parent = node;
    node_2 -> parent = node;
  }
  return node;
}

Node* GetG(String* string) {
  Node* node = GetE(string);

  if (string -> str[string -> pos] != '\0') {
    SyntaxError(string -> pos);
    return nullptr;
  }

  return node;
}

void TreeConstruct(Tree* tree, String* string) {
  tree -> root = GetG(string);
}

void DumpPrintSubtree(Node* node, FILE* graph_file) {
  assert(graph_file);

  if (node != nullptr) {
    DumpPrintSubtree(node -> left, graph_file);
    DumpPrintSubtree(node -> right, graph_file);

    if (node -> type == TYPE_CONST) {
      fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"pink\", label =\"{%lg}\"]\n", node, node -> key.dbl);
    } else if (node -> type == TYPE_BIN_OP || node -> type == TYPE_UN_OP) {
      if (node -> type == TYPE_BIN_OP) {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%s}\"]\n", node, operations_names[node -> key.op]);
      } else {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%s}\"]\n", node, operations_names[node -> key.op]);
      }
    } else {
      fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"green\", label =\"{%c}\"]\n", node, node -> key.var);
    }
    if (node -> left != nullptr) {
      fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> left);
      if (node -> right != nullptr) {
        fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> right);
      }
    }
  }
}

void DumpPrintTree(Tree* tree) {
  assert(tree);

  FILE* graph_file = fopen("graph.gv", "w");

  fprintf(graph_file, "digraph {\n");
  DumpPrintSubtree(tree -> root, graph_file);
  fprintf(graph_file, "}\n");
  fclose(graph_file);

  system("dot -Tpng graph.gv -o graph.png");
}

void PrintInRound(Node* node, FILE* tex_file) {
  fprintf(tex_file, "(");
  PrintSubTex(node, tex_file);
  fprintf(tex_file, ")");
}

void PrintInBraces(Node* node, FILE* tex_file) {
  fprintf(tex_file, "{");
  PrintSubTex(node, tex_file);
  fprintf(tex_file, "}");
}

void PrintSubTex(Node* node, FILE* tex_file) {
  assert(tex_file);

  if (node == nullptr) {
    return;
  }

  switch (node -> type) {
    case TYPE_VAR:
      fprintf(tex_file, "%c", node -> key.var);
      break;

    case TYPE_CONST: {
      if (node -> key.dbl < 0) {
        fprintf(tex_file, "(%lg)", node -> key.dbl);
      } else {
        fprintf(tex_file, "%lg", node -> key.dbl);
      }
      break;
    }

    case TYPE_BIN_OP: {
      switch (node -> key.op) {
        case OP_ADD:
          PrintSubTex(node -> left, tex_file);
          fprintf(tex_file, "+");
          PrintSubTex(node -> right, tex_file);
          break;

        case OP_SUB:
          PrintSubTex(node -> left, tex_file);
          fprintf(tex_file, "-");
          PrintSubTex(node -> right, tex_file);
          break;

        case OP_MUL:
          if (node -> left -> type == TYPE_BIN_OP && (node -> left -> key.op == OP_ADD || node -> left -> key.op == OP_SUB)) {
            PrintInRound(node -> left, tex_file);
          } else {
            PrintSubTex(node -> left, tex_file);
          }
          fprintf(tex_file, "\\cdot ");
          if (node -> right -> type == TYPE_BIN_OP && (node -> right -> key.op == OP_ADD || node -> right -> key.op == OP_SUB)) {
            PrintInRound(node -> right, tex_file);
          } else {
            PrintSubTex(node -> right, tex_file);
          }
          break;

        case OP_DIV:
            fprintf(tex_file, "\\dfrac ");
            PrintInBraces(node -> left, tex_file);
            PrintInBraces(node -> right, tex_file);
            break;

        case OP_POW:
            if (node -> left -> type == TYPE_CONST || node -> left -> type == TYPE_VAR) {
              PrintSubTex(node -> left, tex_file);
            } else {
              PrintInRound(node -> left, tex_file);
            }

            fprintf(tex_file, "^");
            PrintInBraces(node -> right, tex_file);
            break;
      }
      break;
    }

    case TYPE_UN_OP: {
      switch (node -> key.op) {
        case OP_LN:
          fprintf(tex_file, "\\ln ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_EXP:
          fprintf(tex_file, "e^");
          PrintInBraces(node -> left, tex_file);
          break;

        case OP_SIN:
          fprintf(tex_file, "\\sin ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_COS:
          fprintf(tex_file, "\\cos ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_TAN:
          fprintf(tex_file, "\\tan ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_CTG:
          fprintf(tex_file, "\\cot ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_SQRT:
          fprintf(tex_file, "\\sqrt ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_ARCSIN:
          fprintf(tex_file, "\\arcsin ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_ARCCOS:
          fprintf(tex_file, "\\arccos ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_ARCTAN:
          fprintf(tex_file, "\\arctan ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_SH:
          fprintf(tex_file, "\\sinh ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_CH:
          fprintf(tex_file, "\\cosh ");
          PrintInRound(node -> left, tex_file);
          break;

        case OP_TH:
          fprintf(tex_file, "\\tanh ");
          PrintInRound(node -> left, tex_file);
          break;
      }
      break;
    }

    default:
      fprintf(stderr, "Unknown node type %lu", node -> type);
      break;
  }
}

void PrintTex(Tree* tree) {
  assert(tree);

  FILE* tex_file = fopen("equation.tex", "w");

  fprintf(tex_file, "\\documentclass{report}\n");
  fprintf(tex_file, "\\usepackage{amsmath}\n");
  fprintf(tex_file, "\\begin{document}\n");
  fprintf(tex_file, "\\newpage\n");
  fprintf(tex_file, "$$");
  PrintSubTex(tree -> root, tex_file);
  fprintf(tex_file, "$$\n");
  fprintf(tex_file, "\\end{document}\n");
  fclose(tex_file);

  system("pdflatex equation.tex");
}

Node::node_type UnionDouble(double key) {
  Node::node_type data = {.dbl = key};
  return data;
}

Node::node_type UnionOp(int key) {
  Node::node_type data = {.op = key};
  return data;
}

Node::node_type UnionChar(char key) {
  Node::node_type data = {.var = key};
  return data;
}

#define L my_node -> left
#define R my_node -> right
#define dR Diff(R)
#define dL Diff(L)
#define cR CopyTree(R)
#define cL CopyTree(L)
#define SQUARE CreateNode(TYPE_CONST, UnionDouble(2), NULL, NULL)
#define CONSTANT CreateNode(TYPE_CONST, UnionDouble(0), NULL, NULL)
#define VAR CreateNode(TYPE_CONST, UnionDouble(1), NULL, NULL)
#define MINUS CreateNode(TYPE_CONST, UnionDouble(-1), NULL, NULL)
#define ADD(left, right) CreateNode(TYPE_BIN_OP, UnionOp(OP_ADD), left, right)
#define SUB(left, right) CreateNode(TYPE_BIN_OP, UnionOp(OP_SUB), left, right)
#define MUL(left, right) CreateNode(TYPE_BIN_OP, UnionOp(OP_MUL), left, right)
#define DIV(left, right) CreateNode(TYPE_BIN_OP, UnionOp(OP_DIV), left, right)
#define DEG(left, right) CreateNode(TYPE_BIN_OP, UnionOp(OP_POW), left, right)
#define COS(left) CreateNode(TYPE_UN_OP, UnionOp(OP_COS), left, NULL)
#define SIN(left) CreateNode(TYPE_UN_OP, UnionOp(OP_SIN), left, NULL)
#define CH(left) CreateNode(TYPE_UN_OP, UnionOp(OP_CH), left, NULL)
#define SH(left) CreateNode(TYPE_UN_OP, UnionOp(OP_SH), left, NULL)
#define SQRT(left) CreateNode(TYPE_UN_OP, UnionOp(OP_SQRT), left, NULL)
#define LN(left) CreateNode(TYPE_UN_OP, UnionOp(OP_LN), left, NULL)
#define EXP(left) CreateNode(TYPE_UN_OP, UnionOp(OP_EXP), left, NULL)

Node* SwitchBinOp(Node* my_node) {
  assert(my_node);

  switch (my_node -> key.op) {
    case OP_ADD:
      return ADD(dL, dR);

    case OP_SUB:
      return SUB(dL, dR);

    case OP_MUL:
      return ADD(MUL(dL, cR), MUL(cL, dR));

    case OP_DIV:
      return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), DEG(cR, SQUARE));

    case OP_POW:
    {  bool IsVarLeft = IsVar(L);
      bool IsVarRight = IsVar(R);

      if (IsVarLeft && IsVarRight) {
        return Diff(EXP(MUL(cR, LN(cL))));
      }
      if (IsVarLeft) {
        return MUL(MUL(cR, DEG(cL, SUB(cR, VAR))), dL);
      }

      return MUL(MUL(DEG(cL, cR), LN(cL)), dR);
    }

    default:
      printf("Error: unknown binary operation, line %d\n", __LINE__);
      return nullptr;
  }
}

Node* SwitchUnOp(Node* my_node) {
  assert(my_node);

  switch(my_node -> key.op) {
    case OP_SIN:
      return MUL(COS(cL), dL);

    case OP_COS:
      return MUL(MUL(SIN(cL), dL), MINUS);

    case OP_TAN:
      return MUL(DIV(VAR, DEG(COS(cL), SQUARE)), dL);

    case OP_CTG:
      return MUL(DIV(MINUS, DEG(SIN(cL), SQUARE)), dL);

    case OP_SQRT:
      return MUL(DIV(VAR, MUL(SQUARE, SQRT(cL))), dL);

    case OP_LN:
      return MUL(DIV(VAR, cL), dL);

    case OP_EXP:
      return MUL(CopyTree(my_node), dL);

    case OP_ARCSIN:
      return MUL(DIV(VAR, SQRT(SUB(VAR, DEG(cL, SQUARE)))), dL);

    case OP_ARCCOS:
      return MUL(DIV(MINUS, SQRT(SUB(VAR, DEG(cL, SQUARE)))), dL);

    case OP_ARCTAN:
      return MUL(DIV(VAR, ADD(VAR, DEG(cL, SQUARE))), dL);

    case OP_SH:
      return MUL(CH(cL), dL);

    case OP_CH:
      return MUL(SH(cL), dL);

    case OP_TH:
      return MUL(DIV(VAR, DEG(CH(cL), SQUARE)), dL);
    default:
      printf("Error: unknown unary operation, line %d\n", __LINE__);
      return nullptr;
  }
}

Node* Diff(Node* my_node) {
  Node* node = nullptr;
  if (my_node != nullptr) {
    switch (my_node -> type) {
      case TYPE_CONST:
        node = CONSTANT;
        break;

      case TYPE_VAR:
        node = VAR;
        break;

      case TYPE_BIN_OP:
        node = SwitchBinOp(my_node);
        break;

      case TYPE_UN_OP:
        node = SwitchUnOp(my_node);
        break;
    }
  }

  TexDiffBefore(my_node);
  TexDiffAfter(node);

  return node;
}

#undef R
#undef L
#undef dR
#undef dL
#undef cR
#undef cL
#undef SQUARE
#undef CONSTANT
#undef VAR
#undef MINUS
#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef DEG
#undef COS
#undef SIN
#undef SQRT
#undef LN
#undef EXP

void Update(Node* node, Node* node_1, Node* node_2) {
  node -> type = node_1 -> type;
  node -> key = node_1 -> key;
  node -> right = node_1 -> right;
  node -> left = node_1 -> left;
  node_1 -> parent = nullptr;
  node_2 -> parent = nullptr;
  node_1 -> left = nullptr;
  node_1 -> right = nullptr;

  free(node_1);
  free(node_2);
}

void UpdateNode(Node* left, Node* right, Node* node) {
  assert(node);
  assert(left);
  assert(right);

  TexSimplifyBefore(node);
  if (right -> key.dbl == 0 || right -> key.dbl == 1) {
    Update(node, left, right);
  } else if (node -> key.op == OP_SUB) {
    node -> left = nullptr;
    free(left);
  } else {
    Update(node, right, left);
  }
  TexSimplifyAfter(node);
}

bool IsConst(Node* node) {
  if (node -> type == TYPE_CONST) {
    return true;
  }

  return false;
}

bool IsZero(Node* node) {
  if (node -> type == TYPE_CONST && node -> key.dbl == 0) {
    return true;
  }

  return false;
}

bool IsOne(Node* node) {
  if (node -> type == TYPE_CONST && node -> key.dbl == 1) {
    return true;
  }

  return false;
}

bool IsVar(Node* node) {
  if (node == nullptr) {
    return false;
  }

  if(node -> type == TYPE_VAR) {
    return true;
  }

  if (IsVar(node -> right)) {
    return true;
  }

  if (IsVar(node -> left)) {
    return true;
  }

  return false;
}

#define L node -> left
#define R node -> right

bool SimplifyAdd(Node* node) {
  assert(node);

  if (IsZero(L) || IsZero(R)) {
    UpdateNode(L, R, node);
    return true;
  }

  return false;
}

bool SimplifySub(Node* node) {
  assert(node);

  if (IsZero(L) || IsZero(R)) {
    UpdateNode(L, R, node);
    return true;
  }

  return false;
}

bool SimplifyMul(Node* node) {
  assert(node);

  if (IsOne(L) || IsOne(R)) {
    UpdateNode(L, R, node);
    return true;
  }

  if (IsZero(L) || IsZero(R)) {
    TexSimplifyBefore(node);
    node -> type = TYPE_CONST;
    node -> key.dbl = 0;
    L -> parent = nullptr;
    R -> parent = nullptr;
    L = nullptr;
    R = nullptr;

    free(L);
    free(R);

    TexSimplifyAfter(node);
    return true;
  }

  return false;
}

bool SimplifyDiv(Node* node) {
  assert(node);

  if (IsOne(R)) {
    UpdateNode(L, R, node);
    return true;
  }

  return false;
}

bool SimplifyDeg(Node* node) {
  assert(node);

  if(IsZero(R)) {
    TexSimplifyBefore(node);
    node -> type = TYPE_CONST;
    node -> key.dbl = 1;
    L -> parent = nullptr;
    R -> parent = nullptr;
    R = nullptr;
    L = nullptr;

    free(L);
    free(R);
    TexSimplifyAfter(node);

    return true;
  }

  if(IsOne(R)) {
    TexSimplifyBefore(node);
    node -> type = L -> type;
    node -> key = L -> key;
    L -> parent = nullptr;
    R -> parent = nullptr;
    R = nullptr;
    L = nullptr;

    free(L);
    free(R);
    TexSimplifyAfter(node);

    return true;
  }

  return false;
}

bool SimplifyNeutral(Node* node) {
  if (node == nullptr) {
    return false;
  }

  bool arg_1 = SimplifyNeutral(L);
  bool arg_2 = SimplifyNeutral(R);

  bool result = arg_1 || arg_2;

  if (node -> type != TYPE_BIN_OP && node -> type != TYPE_UN_OP) {
    return result;
  }

  switch(node -> key.op) {
    case OP_ADD:
      return (SimplifyAdd(node) || result);

    case OP_SUB:
      return (SimplifySub(node) || result);

    case OP_MUL:
      return (SimplifyMul(node) || result);

    case OP_DIV:
      return (SimplifyDiv(node) || result);

    case OP_POW:
      return (SimplifyDeg(node) || result);

    default:
      return result;
  }

}

bool SimplifyConsts(Node* node) {
  if (node == nullptr) {
    return false;
  }

  bool arg_1 = SimplifyConsts(L);
  bool arg_2 = SimplifyConsts(R);

  bool result = arg_1 || arg_2;

  if(node -> type == TYPE_BIN_OP && R -> type == TYPE_CONST && L -> type == TYPE_CONST) {
    TexSimplifyBefore(node);
    node -> type = TYPE_CONST;
    switch(node -> key.op) {
      case OP_ADD:
        node -> key.dbl = L -> key.dbl + R -> key.dbl;
        break;

      case OP_SUB:
        node -> key.dbl = L -> key.dbl - R -> key.dbl;
        break;

      case OP_MUL:
        node -> key.dbl = L -> key.dbl * R -> key.dbl;
        break;

      case OP_DIV:
        node -> key.dbl = L -> key.dbl / R -> key.dbl;
        break;

      case OP_POW:
        node -> key.dbl = pow(L -> key.dbl, R -> key.dbl);
        break;
    }
    L -> parent = nullptr;
    R -> parent = nullptr;
    L = nullptr;
    R = nullptr;

    free(L);
    free(R);

    TexSimplifyAfter(node);

    return true;
  }

  return result;
}

#undef L
#undef R

void Simplify(Tree* tree) {
  assert(tree);

  TexSimplifyBegin(tree);
  bool is_changed = true;

  while(is_changed) {
    bool arg_1 = SimplifyNeutral(tree -> root);
    bool arg_2 = SimplifyConsts(tree -> root);
    is_changed = arg_1 || arg_2;
  }
}

void DestructNode(Node* node) {
  assert(node);

  if (node -> left != nullptr) {
    DestructNode(node -> left);
  } else if (node -> right != nullptr) {
    DestructNode(node -> right);
  } else {
    free(node);
  }
}

void UpdateTree(Tree* tree) {
  assert(tree);

  if (tree -> root != nullptr) {
    DestructNode(tree -> root);
  }

  tree -> root = nullptr;
}

void Diverative(Tree* tree) {
  assert(tree);

  BeginReport(tree);

  Node* new_root = Diff(tree -> root);

  UpdateTree(tree);
  tree -> root = new_root;

  Simplify(tree);

  EndReport(tree);
}

int main() {
  String string = {};
  string.str = (char*)calloc(100, sizeof(char));
  string.pos = 0;
  scanf("%s", string.str);
  Tree tree = {};
  TreeConstruct(&tree, &string);
  Diverative(&tree);
  DumpPrintTree(&tree);
  PrintTex(&tree);
  return 0;
}
