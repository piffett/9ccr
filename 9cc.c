#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
	TK_RESERVED, // 記号
	TK_NUM,      // 整数トークン
	TK_EOF,      // 入力の終わりを表すトークン
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

// 今見ているトークン
Token *token;

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
} NodeKind;

typedef struct Node Node;

struct Node {
	NodeKind kind;		// ノードの型
	Node *lhs;			// 左辺
	Node *rhs;			// 右辺
	int val;			// ND_NUMのときの中身
};

// Node作成関数
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

// 数字Node作成関数
Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

// 入力文字列
char *user_input;
// エラー報告関数。printfと同じ引数
void error(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, ""); // pos個の空白
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 次のトークンが期待している記号(op)の時読み進めてtrueを返す
bool consume(char *op) {
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str,op, token->len))
		return false;
	token = token->next;
	return true;
}

// 次のトークンが期待している記号の時トークンを進める
void expect(char *op) {
	if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		error(token->str, "'%c'ではありません", op);
	token = token->next;
}

// 次のトークンが数字の時はトークンを１つ読み進め、それ以外はエラー報告
int expect_number() {
	if(token->kind != TK_NUM)
		error(token->str, "数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// 新しくトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->len = len;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// 文字を比較
bool front_cmp(char *p, char*q){
	return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列をトークナイズして返す
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		// 空白文字はスキップ
		if (isspace(*p)){
			p++;
			continue;
		}

		// ２文字トークンを生成
		if(front_cmp(p, "==") || front_cmp(p, "!=") || front_cmp(p, "<=") || front_cmp(p, ">=")){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		// 1文字記号のトークン作成
		if(front_cmp(p, "+") || front_cmp(p, "-") || front_cmp(p, "*") || front_cmp(p,"/")||
				front_cmp(p, "(") ||front_cmp(p, ")")||front_cmp(p, ">")||front_cmp(p, "<")){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}
		
		

		if(isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			char *start_p = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - start_p;
			continue;
		}

		error(token->str, "トークナイズできません");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary_on_primary();

Node *expr() {
	return equality();
}

Node *equality() {
	Node *node = relational();

	for(;;){
		if(consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if(consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

Node *relational() {
	Node *node = add();

	for(;;){
		if(consume("<"))
			node = new_node(ND_LT, node, add());
		else if(consume("<="))
			node = new_node(ND_LE, node, add());
		else if(consume(">"))
			node = new_node(ND_LT, add(), node);
		else if(consume(">="))
			node = new_node(ND_LE, add(), node);
		else
			return node;
	}
}

Node *add() {
	Node *node = mul();

	for(;;) {
		if(consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

Node *mul(){
	Node *node = unary_on_primary();

	for(;;) {
		if(consume("*"))
			node = new_node(ND_MUL, node, unary_on_primary());
		else if(consume("/"))
			node = new_node(ND_DIV, node, unary_on_primary());
		else
			return node;
	}
}

Node *unary_on_primary(){
	if(consume("+"))
		return primary();
	if(consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

Node *primary(){
	// 次のトークンが"("なら、"(" expr ")"
	if(consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}
	
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

void gen(Node *node){
	if(node->kind == ND_NUM) {
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch(node->kind) {
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		case ND_EQ:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LT:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
			break;
	}

	printf("	push rax\n");
}

int main(int argc, char **argv){
	if(argc != 2){
		error(token->str, "引数の個数は正しくありません");
		return 1;
	}

	// トークナイズしてパース
	user_input = argv[1];
	token = tokenize(user_input);
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 抽象構文木を下りながらコード生成
	gen(node);

	// スタックトップに式全体の値が残っているので、RAXにロードして返り値にセット
	printf(" pop rax\n");
	printf("	ret\n");
	return 0;
}
