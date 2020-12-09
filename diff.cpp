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
  printf ("Syntax Error on a poistion = [%d]\n", error_place);
  return -1;
}

Node* GetN(String* string) {
  Node* node = (Node*)calloc(1, sizeof(Node));

  if (isdigit(string -> str[string -> pos])) {
    node -> type = TYPE_CONST;
    int temp = 0;
    sscanf(string -> str + string -> pos, "%lg%n", &node -> key.dbl, &temp);
    string -> pos += temp;
  } else if (isalpha(string -> str[string -> pos])) {
    node -> type = TYPE_VAR;
    node -> key.symb = string -> str[string -> pos];
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
    } else {
      string -> pos++;
      return node;
    }
  } else {
    return GetN(string);
  }
}

Node* GetD(String* string) {
  Node* node = GetP(string);
  while(string -> str[string -> pos] == '^') {
    Node* node_1 = node;
    string -> pos++;
    Node* node_2 = GetP(string);
    Node::node_type key = {.symb = '^'};
    node = CreateNode(TYPE_OP, key, node_1, node_2);
    node_1 -> parent = node;
    node_2 -> parent = node;
  }
  return node;
}

Node* GetS(String* string) {
  if (string -> str[string -> pos] == 's') {
    char* arr = (char*)calloc(5, sizeof(char));
    arr[0] = 's';
    string -> pos++;
    for (size_t i = 1; isalpha(string -> str[string -> pos]) && i < 5; i++) {
      arr[i] = string -> str[string -> pos];
      string -> pos++;
    }

    if (strcmp(arr, "sin") == 0) {
      Node* node_1 = GetD(string);
      Node::node_type key = {.str = arr};
      Node* node = CreateNode(TYPE_STR, key, node_1, nullptr);
      node_1 -> parent = node;
      return node;
    } else {
      SyntaxError(string -> pos);
      return nullptr;
    }
  } else {
    return GetD(string);
  }
}

Node* GetC(String* string) {
  if (string -> str[string -> pos] == 'c') {
    char* arr = (char*)calloc(5, sizeof(char));
    arr[0] = 'c';
    string -> pos++;
    for (size_t i = 1; isalpha(string -> str[string -> pos]) && i < 5; i++) {
      arr[i] = string -> str[string -> pos];
      string -> pos++;
    }

    if (strcmp(arr, "cos") == 0) {
      Node* node_1 = GetS(string);
      Node::node_type key = {.str = arr};
      Node* node = CreateNode(TYPE_STR, key, node_1, nullptr);
      node_1 -> parent = node;
      return node;
    } else {
      SyntaxError(string -> pos);
      return nullptr;
    }
  } else {
    return GetS(string);
  }
}

Node* GetT(String* string) {
  Node* node = GetC(string);
  while(string -> str[string -> pos] == '*' || string -> str[string -> pos] == '/') {
    char oper = string -> str[string -> pos];
    string -> pos++;
    Node* node_1 = node;
    Node* node_2 = GetC(string);
    Node::node_type key = {.symb = oper};
    node = CreateNode(TYPE_OP, key, node_1, node_2);
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
    Node::node_type key = {.symb = oper};
    node = CreateNode(TYPE_OP, key, node_1, node_2);
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
    } else if (node -> type == TYPE_OP || node -> type == TYPE_STR) {
      if (node -> type == TYPE_OP) {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%c}\"]\n", node, node -> key.symb);
      } else {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%s}\"]\n", node, node -> key.str);
      }
    } else {
      fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"green\", label =\"{%c}\"]\n", node, node -> key.symb);
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

void DumpSubtree(Node* node, FILE* graph_file) {
  assert(graph_file);

  if (node != nullptr) {
    DumpSubtree(node -> left, graph_file);
    DumpSubtree(node -> right, graph_file);

    if (node -> type == TYPE_CONST) {
      fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"pink\", label =\"{%lg}\"]\n", node, node -> key.dbl);
    } else if (node -> type == TYPE_OP || node -> type == TYPE_STR) {
      if (node -> type == TYPE_OP) {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%c}\"]\n", node, node -> key.symb);
      } else {
        fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"blue\", label =\"{%s}\"]\n", node, node -> key.str);
      }
    } else {
      fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"green\", label =\"{%c}\"]\n", node, node -> key.symb);
    }
    if (node -> left != nullptr) {
      fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> left);
      if (node -> right != nullptr) {
        fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> right);
      }
    }
  }
}

void Dump(Node* node) {
  assert(node);

  FILE* graph_file = fopen("graph.gv", "w");

  fprintf(graph_file, "digraph {\n");
  DumpSubtree(node, graph_file);
  fprintf(graph_file, "}\n");
  fclose(graph_file);

  system("dot -Tpng graph.gv -o graph.png");
}

void PrintSubTex(Node* node, FILE* tex_file) {
  assert(tex_file);

  if (node != nullptr) {
    if (node -> left != nullptr && node -> left -> left != nullptr) {
      if (node -> key.symb == '/') {
        fprintf(tex_file, "\\frac{");
      }

      if (node -> type == TYPE_STR) {
        fprintf(tex_file, "\\%s(", node -> key.str);
      }

      fprintf(tex_file, "(");
      PrintSubTex(node -> left, tex_file);
      fprintf(tex_file, ")");
      if (node -> key.symb == '/') {
        fprintf(tex_file, "}");
      }

      if (node -> type == TYPE_STR) {
        fprintf(tex_file, ")");
      }

    } else {
      if (node -> key.symb == '/') {
        fprintf(tex_file, "\\frac{");
      }
      if (node -> type == TYPE_STR) {
        fprintf(tex_file, "\\%s(", node -> key.str);
      }
      PrintSubTex(node -> left, tex_file);
      if (node -> key.symb == '/') {
        fprintf(tex_file, "}");
      }
      if (node -> type == TYPE_STR) {
        fprintf(tex_file, ")");
      }
    }

    if (node -> type == TYPE_CONST) {
      fprintf(tex_file, "%lg", node -> key.dbl);
    }  else if (node -> type == TYPE_VAR) {
      fprintf(tex_file, "%c", node -> key.symb);
    } else if (node -> type == TYPE_OP) {
      if (node  -> key.symb == '*') {
        fprintf(tex_file, "\\cdot(");
      } else if (node -> key.symb == '/') {
        ;
      } else if (node -> key.symb == '^') {
        fprintf(tex_file, "^{");
      } else {
        fprintf(tex_file, "%c", node -> key.symb);
      }
    } else if (node -> type == TYPE_STR) {
      ;
    }

    if (node -> right != nullptr && node -> right -> left != nullptr) {
      if (node -> key.symb == '/') {
        fprintf(tex_file, "{");
      }
      fprintf(tex_file, "(");
      PrintSubTex(node -> right, tex_file);
      fprintf(tex_file, ")");
      if (node -> key.symb == '/') {
        fprintf(tex_file, "}");
      }
      if (node -> key.symb == '*') {
        fprintf(tex_file, ")");
      }
      if (node -> key.symb == '^') {
        fprintf(tex_file, "}");
      }
    } else {
      if (node -> key.symb == '/') {
        fprintf(tex_file, "{");
      }
      PrintSubTex(node -> right, tex_file);
      if (node -> key.symb == '/') {
        fprintf(tex_file, "}");
      }
      if (node -> key.symb == '^') {
        fprintf(tex_file, "}");
      }
    }
  }
}

void PrintTex(Tree* tree) {
  assert(tree);

  FILE* tex_file = fopen("equation.tex", "w");

  fprintf(tex_file, "\\documentclass{report}\n");
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

Node::node_type UnionStr(const char* key) {
  Node::node_type data = {.str = key};
  return data;
}

Node::node_type UnionChar(char key) {
  Node::node_type data = {.symb = key};
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
#define ADD(left, right) CreateNode(TYPE_OP, UnionChar('+'), left, right)
#define SUB(left, right) CreateNode(TYPE_OP, UnionChar('-'), left, right)
#define MUL(left, right) CreateNode(TYPE_OP, UnionChar('*'), left, right)
#define DIV(left, right) CreateNode(TYPE_OP, UnionChar('/'), left, right)
#define DEG(left, right) CreateNode(TYPE_OP, UnionChar('^'), left, right)
#define COS(left) CreateNode(TYPE_STR, UnionStr("cos"), left, NULL)
#define SIN(left) CreateNode(TYPE_STR, UnionStr("sin"), left, NULL)

Node* Diff(Node* my_node) {
  Node* node = nullptr;
  if (my_node != nullptr) {
    switch(my_node -> type) {
      case TYPE_CONST: node = CONSTANT;
                       break;
      case   TYPE_VAR: node = VAR;
                       break;
      case    TYPE_OP: switch(my_node -> key.symb) {
        case '+': node = ADD(dL, dR);
                  break;
        case '-': node = SUB(dL, dR);
                  break;
        case '*': node = ADD(MUL(dL, cR), MUL(cL, dR));
                  break;
        case '/': node = DIV(SUB(MUL(dL, cR), MUL(cL, dR)), DEG(cR, SQUARE));
                  break;
        case '^': node = MUL(cR, DEG(cL, SUB(cR, VAR)));
                  break;
      }
                  break;
      case TYPE_STR: switch(my_node -> key.str[0]) {
        case 's': node = COS(cL);
                  break;
        case 'c': node = MUL(MINUS, SIN(cL));
                  break;
      }
                  break;
    }

    if (my_node -> left != nullptr && my_node -> left -> type != TYPE_CONST && my_node -> type != TYPE_OP) {
      node = MUL(Diff(my_node -> left), node);
    }
    if (my_node -> right != nullptr && my_node -> right -> type != TYPE_CONST && my_node -> type != TYPE_OP) {
      node = MUL(Diff(my_node -> right), node);
    }
  }
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

void UpdateNode(Node* left, Node* right, Node* node) {
  if (right -> key.dbl == 0 || right -> key.dbl == 1) {
    node -> type = node -> left -> type;
    node -> key = node -> left -> key;
    node -> right = node -> left -> right;
    node -> left = node -> left -> left;
    left -> parent = nullptr;
    right -> parent = nullptr;
    left -> left = nullptr;
    left -> right = nullptr;

    free(left);
    free(right);
  } else if (node -> key.symb == '-') {
    node -> left = nullptr;
    free(left);
  } else {
    node -> type = node -> right -> type;
    node -> key = node -> right -> key;
    node -> left = node -> right -> left;
    node -> right = node -> right -> right;
    left -> parent = nullptr;
    right -> parent = nullptr;
    right -> left = nullptr;
    right -> right = nullptr;

    free(left);
    free(right);
  }
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
    node -> type = TYPE_CONST;
    node -> key.dbl = 0;
    L -> parent = nullptr;
    R -> parent = nullptr;
    L = nullptr;
    R = nullptr;

    free(L);
    free(R);

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
    node -> type = TYPE_CONST;
    node -> key.dbl = 1;
    L -> parent = nullptr;
    R -> parent = nullptr;
    R = nullptr;
    L = nullptr;

    free(L);
    free(R);

    return true;
  }

  if(IsOne(R)) {
    node -> type = L -> type;
    node -> key = L -> key;
    L -> parent = nullptr;
    R -> parent = nullptr;
    R = nullptr;
    L = nullptr;

    free(L);
    free(R);

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

  if (node -> type != TYPE_OP && node -> type != TYPE_STR) {
    return result;
  }

  switch(node -> key.symb) {
    case '+': return (SimplifyAdd(node) || result);
    case '-': return (SimplifySub(node) || result);
    case '*': return (SimplifyMul(node) || result);
    case '/': return (SimplifyDiv(node) || result);
    case '^': return (SimplifyDeg(node) || result);
    default:  return result;
  }

}

bool SimplifyConsts(Node* node) {
  if (node == nullptr) {
    return false;
  }

  bool arg_1 = SimplifyConsts(L);
  bool arg_2 = SimplifyConsts(R);

  bool result = arg_1 || arg_2;

  if(node -> type == TYPE_OP && R -> type == TYPE_CONST && L -> type == TYPE_CONST) {
    node -> type = TYPE_CONST;
    switch(node -> key.symb) {
      case '+': node -> key.dbl = L -> key.dbl + R -> key.dbl;
                break;
      case '-': node -> key.dbl = L -> key.dbl - R -> key.dbl;
                break;
      case '*': node -> key.dbl = L -> key.dbl * R -> key.dbl;
                break;
      case '/': node -> key.dbl = L -> key.dbl / R -> key.dbl;
                break;
      case '^': node -> key.dbl = pow(L -> key.dbl, R -> key.dbl);
                break;
    }
    L -> parent = nullptr;
    R -> parent = nullptr;
    L = nullptr;
    R = nullptr;

    free(L);
    free(R);

    return true;
  }

  return result;
}

#undef L
#undef R

void Simplify(Tree* tree) {
  assert(tree);

  bool is_changed = true;

  while(is_changed) {
    bool arg_1 = SimplifyNeutral(tree -> root);
    bool arg_2 = SimplifyConsts(tree -> root);
    is_changed = arg_1 || arg_2;
  }
}

int main() {
  String string = {};
  string.str = (char*)calloc(100, sizeof(char));
  string.pos = 0;
  scanf("%s", string.str);
  Tree tree = {};
  TreeConstruct(&tree, &string);
  DumpPrintTree(&tree);
  Node* new_root = Diff(tree.root);
  tree.root = new_root;
  Simplify(&tree);
  DumpPrintTree(&tree);
  PrintTex(&tree);
  return 0;
}
