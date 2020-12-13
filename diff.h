#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>

const size_t MAX_COMMAND_LENGTH = 5;
const size_t FIRST_UN_OP = 5;
const size_t NUMB_OF_OP = 18;

enum Types {
  TYPE_CONST,
  TYPE_UN_OP,
  TYPE_BIN_OP,
  TYPE_VAR
};

enum OP {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_POW,
  OP_LN,
  OP_SIN,
  OP_COS,
  OP_TAN,
  OP_CTG,
  OP_SQRT,
  OP_EXP,
  OP_ARCSIN,
  OP_ARCCOS,
  OP_ARCTAN,
  OP_SH,
  OP_CH,
  OP_TH
};

static const char* operations_names[] = {
  "+",
  "-",
  "*",
  "/",
  "^",
  "ln",
  "sin",
  "cos",
  "tan",
  "ctg",
  "sqrt",
  "exp",
  "arcsin",
  "arccos",
  "arctan",
  "sh",
  "ch",
  "th"
};

struct Node {
  size_t type;
  union node_type {
    double dbl;
    int op;
    char var;
  } key;
  Node* parent;
  Node* left;
  Node* right;
};

struct Tree {
  Node* root;
};

struct String {
  char* str;
  int pos;
};

Node* CreateNode(size_t type, Node::node_type key, Node* left, Node* right);
Node* CopyTree(const Node* node);
int   SyntaxError(int error_place);

Node* GetN(String* string);
Node* GetP(String* string);
Node* GetPow(String* string);
Node* GetUnOp(String* string);
Node* GetT(String* string);
Node* GetE(String* string);
Node* GetG(String* string);

void PrintInRound(Node* node, FILE* tex_file);
void PrintInBraces(Node* node, FILE* tex_file);
void PrintSubTex(Node* node, FILE* tex_file);
void PrintTex(Tree* tree);

Node::node_type UnionDouble(double key);
Node::node_type UnionOp(int key);
Node::node_type UnionChar(char key);

Node* SwitchBinOp(Node* my_node);
Node* SwitchUnOp(Node* my_node);
Node* Diff(Node* my_node);

void Update(Node* node, Node* node_1, Node* node_2);
void UpdateNode(Node* left, Node* right, Node* node);

bool IsConst(Node* node);
bool IsZero(Node* node);
bool IsOne(Node* node);
bool IsVar(Node* node);

bool SimplifyAdd(Node* node);
bool SimplifySub(Node* node);
bool SimplifyMul(Node* node);
bool SimplifyDiv(Node* node);
bool SimplifyDeg(Node* node);
bool SimplifyNeutral(Node* node);
bool SimplifyConsts(Node* node);
void Simplify(Tree* tree);

void DestructNode(Node* node);
void UpdateTree(Tree* tree);
void Diverative(Tree* tree);

void  BeginReport(Tree* tree);
void  TexSimplifyBefore(Node* node);
void  TexSimplifyAfter(Node* node);
void  TexSimplifyBegin(Tree* tree);
Node* TexDiffBefore(Node* node);
Node* TexDiffAfter(Node* node);
void  EndReport(Tree* tree);
void PrintInRound(Node* node);
void PrintInBraces(Node* node);
void  MakeTex(Node* node, FILE* fp_tex);
