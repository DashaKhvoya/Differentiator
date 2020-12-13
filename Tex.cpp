#include "diff.h"
#include "tex.h"

void BeginReport(Tree* tree) {
  assert(tree);

  fp_tex = fopen("report.tex", "w");
  assert(fp_tex != nullptr);

  fprintf(fp_tex, "\\documentclass{article}\n\\usepackage{amsmath}\n\\usepackage{xcolor}\n\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n\\usepackage{breqn}\n");
  fprintf(fp_tex, "\\usepackage{hyperref}\n\\usepackage[russian]{babel}\n\\definecolor{urlcolor}{HTML}{330B03}\n\\hypersetup{pdfstartview=FitH,  linkcolor=linkcolor,urlcolor=urlcolor, colorlinks=true}\n\\begin{document}\n");

  fprintf(fp_tex, "\\begin{center}\n{\\LARGE %s}\n\n\\end{center}\n", intro_message_begin);
  fprintf(fp_tex, "\\begin{center}\n{\\LARGE %s}\n\n\\end{center}\n", initials_message);

  fprintf(fp_tex, "%s\n\n", intro_message_description);
  fprintf(fp_tex, "\\begin{center}\n{\\Large %s}\n\n", intro_message_end);

  fprintf(fp_tex, "\\begin{dmath}\n y = ");
  MakeTex(tree -> root, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n");
}

void TexSimplifyBefore(Node* node) {
  fprintf(fp_tex, "%s:\n", simplify_messages_begin[rand() % simplify_messages_begin_count]);

  fprintf(fp_tex, "\\begin{dmath}\n y = ");
  MakeTex(node, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n");
}

void TexSimplifyAfter(Node* node) {
  fprintf(fp_tex, "%s:\n", simplify_messages_end[rand() % simplify_messages_end_count]);

  fprintf(fp_tex, "\\begin{dmath}\n y = ");
  MakeTex(node, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n");
}

void TexSimplifyBegin(Tree* tree) {
  assert(tree);

  fprintf(fp_tex, "{\\LARGE}%s:\n", before_simplify_message);

  fprintf(fp_tex, "\\begin{dmath}\n y' = ");
  MakeTex(tree -> root, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n");
}

Node* TexDiffBefore(Node* node) {
  assert(fp_tex);

  fprintf(fp_tex, "%s\n", before_differentiation_messages[rand() % before_differentiation_messages_count]);
  fprintf(fp_tex, "\\begin{dmath}\n y = ");

  MakeTex(node, fp_tex);

  fprintf(fp_tex, "\n\\end{dmath}\n");

  return node;
}

Node* TexDiffAfter(Node* node) {
  assert(fp_tex);

  fprintf(fp_tex, "%s\n", after_differentiation_messages[rand() % after_differentiation_messages_count]);

  fprintf(fp_tex, "\\begin{dmath}\n \\frac{dy}{dx} = ");
  MakeTex(node, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n");

  return node;
}

void EndReport(Tree* tree) {
  assert(fp_tex);

  fprintf(fp_tex, "%s\n\n", outro_message);

  fprintf(fp_tex, "\\begin{dmath}\n y' = ");
  MakeTex(tree -> root, fp_tex);
  fprintf(fp_tex, "\n\\end{dmath}\n\n{\\Large %s}\n\\end{center}\n", literature_message);

  fprintf(fp_tex, "My \\href{https://github.com/DashaKhvoya/Differentiator}{\\underline{github}} repository https://github.com/DashaKhvoya/Differentiator\\\\\\\\");

  fprintf(fp_tex, "\\begin{enumerate}\n");
  fprintf(fp_tex, "\\item %s\n", references_message1);
  fprintf(fp_tex, "\\item %s\n", references_message2);
  fprintf(fp_tex, "\\end{enumerate}\n");

  fprintf(fp_tex, "\n\\end{document}");
  fclose(fp_tex);
  system("pdflatex report.tex report.pdf");
}

void PrintInRound(Node* node) {
  fprintf(fp_tex, "(");
  MakeTex(node, fp_tex);
  fprintf(fp_tex, ")");
}

void PrintInBraces(Node* node) {
  fprintf(fp_tex, "{");
  MakeTex(node, fp_tex);
  fprintf(fp_tex, "}");
}

void MakeTex(Node* node, FILE* fp_tex) {
  assert(fp_tex);

  if (node == nullptr) {
    return;
  }

  switch (node -> type) {
    case TYPE_VAR:
      fprintf(fp_tex, "%c", node -> key.var);
      break;

    case TYPE_CONST: {
      if (node -> key.dbl < 0) {
        fprintf(fp_tex, "(%lg)", node -> key.dbl);
      } else {
        fprintf(fp_tex, "%lg", node -> key.dbl);
      }
      break;
    }

    case TYPE_BIN_OP: {
      switch (node -> key.op) {
        case OP_ADD:
          MakeTex(node -> left, fp_tex);
          fprintf(fp_tex, "+");
          MakeTex(node -> right, fp_tex);
          break;

        case OP_SUB:
          MakeTex(node -> left, fp_tex);
          fprintf(fp_tex, "-");
          MakeTex(node -> right, fp_tex);
          break;

        case OP_MUL:
          if (node -> left -> type == TYPE_BIN_OP && (node -> left -> key.op == OP_ADD || node -> left -> key.op == OP_SUB)) {
            PrintInRound(node -> left);
          } else {
            MakeTex(node -> left, fp_tex);
          }
          fprintf(fp_tex, "\\cdot ");
          if (node -> right -> type == TYPE_BIN_OP && (node -> right -> key.op == OP_ADD || node -> right -> key.op == OP_SUB)) {
            PrintInRound(node -> right);
          } else {
            MakeTex(node -> right, fp_tex);
          }
          break;

        case OP_DIV:
            fprintf(fp_tex, "\\dfrac ");
            PrintInBraces(node -> left);
            PrintInBraces(node -> right);
            break;

        case OP_POW:
            if (node -> left -> type == TYPE_CONST || node -> left -> type == TYPE_VAR) {
              MakeTex(node -> left, fp_tex);
            } else {
              PrintInRound(node -> left);
            }

            fprintf(fp_tex, "^");
            PrintInBraces(node -> right);
            break;
      }
      break;
    }

    case TYPE_UN_OP: {
      switch (node -> key.op) {
        case OP_LN:
          fprintf(fp_tex, "\\ln ");
          PrintInRound(node -> left);
          break;

        case OP_EXP:
          fprintf(fp_tex, "e^");
          PrintInBraces(node -> left);
          break;

        case OP_SIN:
          fprintf(fp_tex, "\\sin ");
          PrintInRound(node -> left);
          break;

        case OP_COS:
          fprintf(fp_tex, "\\cos ");
          PrintInRound(node -> left);
          break;

        case OP_TAN:
          fprintf(fp_tex, "\\tan ");
          PrintInRound(node -> left);
          break;

        case OP_CTG:
          fprintf(fp_tex, "\\cot ");
          PrintInRound(node -> left);
          break;

        case OP_SQRT:
          fprintf(fp_tex, "\\sqrt ");
          PrintInRound(node -> left);
          break;

        case OP_ARCSIN:
          fprintf(fp_tex, "\\arcsin ");
          PrintInRound(node -> left);
          break;

        case OP_ARCCOS:
          fprintf(fp_tex, "\\arccos ");
          PrintInRound(node -> left);
          break;

        case OP_ARCTAN:
          fprintf(fp_tex, "\\arctan ");
          PrintInRound(node -> left);
          break;

        case OP_SH:
          fprintf(fp_tex, "\\sinh ");
          PrintInRound(node -> left);
          break;

        case OP_CH:
          fprintf(fp_tex, "\\cosh ");
          PrintInRound(node -> left);
          break;

        case OP_TH:
          fprintf(fp_tex, "\\tanh ");
          PrintInRound(node -> left);
          break;
      }
      break;
    }

    default:
      fprintf(stderr, "Unknown node type %lu", node -> type);
      break;
  }
}
