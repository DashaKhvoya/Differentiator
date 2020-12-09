#pragma once
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>

enum Types {
  TYPE_CONST,
  TYPE_STR,
  TYPE_OP,
  TYPE_VAR
};

struct Node {
  size_t type;
  union node_type {
    double dbl;
    const char*  str;
    char   symb;
  } key;
  Node* parent;
  Node* left;
  Node* right;
  //bool is_here;
};

struct Tree {
  Node* root;
//  size_t size;
};

struct String {
  char* str;
  int pos;
};

Node* CreateNode(size_t type, Node::node_type, Node* left, Node* right);
Node* CopyTree(const Node* node);

int SyntaxError(int error_place);

Node* GetN(String* string);
Node* GetP(String* string);
Node* GetD(String* string);
Node* GetS(String* string);
Node* GetC(String* string);
Node* GetT(String* string);
Node* GetE(String* string);
Node* GetG(String* string);

void TreeConstruct(Tree* tree, String* string);

void DumpPrintSubtree(Node* node, FILE* graph_file);
void DumpPrintTree(Tree* tree);

void PrintSubTex(Node* node, FILE* tex_file);
void PrintTex(Tree* tree);

Node::node_type UnionDouble(double key);
Node::node_type UnionStr(const char* key);
Node::node_type UnionChar(char key);

Node* Diff(Node* my_node);

void UpdateNode(Node* left, Node* right, Node* node);

bool IsConst(Node* node);
bool IsZero(Node* node);
bool IsOne(Node* node);

bool SimplifyAdd(Node* node);
bool SimplifySub(Node* node);
bool SimplifyMul(Node* node);
bool SimplifyDiv(Node* node);
bool SimplifyNeutral(Node* node);
void Simplify(Tree* tree);
