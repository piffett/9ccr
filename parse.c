#include "9cc.h"


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


// 次のトークンが期待している記号(op)の時読み進めてtokenを返す
bool consume(char *op) {
	// printf("%s", op);
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str,op, token->len)){
		return false;
	}
	
	token = token->next;
	return true;
}

// 次のトークンが期待している記号の時トークンを進める
void expect(char *op) {
	if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		error_at(token->str, "'%c'ではありません", op);
	token = token->next;
}

// 次のトークンが数字の時はトークンを１つ読み進め、それ以外はエラー報告
int expect_number() {
	if(token->kind != TK_NUM)
		error_at(token->str, "kazuではありません");
	int val = token->val;
	token = token->next;
	return val;
}

// 次のトークンが変数の時トークンを一つ読み進め,trueをかえす
Token *consume_ident(){
	if(token->kind != TK_IDENT)
		return false;
	token = token->next;
	return token;
}

// パーサー用の宣言
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary_on_primary();

bool at_eof(){
	return token->kind == TK_EOF;
}

void program(){
	int i = 0;
	while(!at_eof())
		code[i++] = stmt();

	code[i] = NULL;
}

Node *stmt() {
	Node *node = expr();
	expect(";");
	return node;
}

Node *expr() {
	return assign();
}

Node *assign() {
	Node *node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return node;
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
	if(consume( "(" ) ) {
		Node *node = expr();
		expect( ")" );
		return node;
	}

	Token *tok = consume_ident();
	if(tok){
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8;
		printf("%c", tok->str[0]);
		return node;
	}
	
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

