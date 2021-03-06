#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// トークンの種類
typedef enum {
	TK_RESERVED,	// 記号
	TK_IDENT,		// 識別子
	TK_NUM,			// 整数トークン
	TK_RETURN,		// return 文
	TK_IF,			// if 文
	TK_ELSE,		// else
	TK_WHILE,		// while 文
	TK_FOR,			// for 文
	TK_EOF,			// 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークンの型
struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

// ノードの種類
typedef enum {
	ND_ADD,		// +
	ND_SUB,		// -
	ND_MUL,		// *
	ND_DIV,		// /
	ND_EQ,		// ==
	ND_NE,		// !=
	ND_LT,		// <
	ND_LE,		// <=
	ND_NUM,		// 整数
	ND_ASSIGN,	// =
	ND_LVAR,	// ローカル変数
	ND_RETURN,	// return
	ND_IF,		// if
	ND_ELSE,	// else
	ND_WHILE,	// while
	ND_FOR,		// for
	ND_BLOCK,	// {}を表す
	ND_FUNC,	// 関数
	ND_FUNC_DEF,// 関数定義
	ND_ADDR,	// アドレス
	ND_DEREF,	// デリファレンス
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;		// ノードの型
	Node *lhs;			// 左辺
	Node *rhs;			// 右辺
	Node *third;		// 三つ目
	Node *forth;		// 四つめ
	Node *next;			// 複文{}サポート
	int val;			// ND_NUMのときの中身
	int offset;			// kindがND_LVARのときに使う
	char *str;			// 関数名
	int len;			// 関数のときの関数名の長さ
};

// 今見ているトークン
Token *token;

// 入力文字列
char *user_input;

// tokenize宣言
void tokenize(char *p);

// gen宣言
void gen(Node *node);

// トークナイズ時のerror
void error_at(char *loc, char *fmt, ...);

// エラー報告関数
void error(char *fmt, ...);

// parseのスタート関数
void program();

// 複数の式をもてるのでNodeを複数持たせる。
Node *code[100];

typedef struct LVar LVar;

// ローカル変数の型
struct LVar{
	LVar *next;
	char *name;
	int len;
	int offset;
};

// ローカル変数
LVar *locals;

// 変数を名前で検索する。見つからなければNULLを返す。実装はparser.cに
LVar *find_lvar(Token *tok);

// ラベルのための変数
int label_num;
