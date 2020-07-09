#include "9cc.h"


// Node作成関数
Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Node *third, Node *forth){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	node->third = third;
	node->forth = forth;
	return node;
}

// 数字Node作成関数
Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}


// 次のトークンが期待している記号(op)の時読み進めてtrueを返す
bool consume(char *op) {
	// printf("%s", op);
	if (!(token->kind == TK_RESERVED||token->kind == TK_RETURN || token->kind == TK_IF || token->kind == TK_ELSE || token->kind == TK_WHILE|| token->kind == TK_FOR) 
		|| strlen(op) != token->len ||
		memcmp(token->str,op, token->len)){
		return false;
	}
	
	token = token->next;
	return true;
}

// 次のトークンが期待している記号の時トークンを進め、なければエラー
void expect(char *op) {
	if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		error_at(token->str, "'%c'ではありません", *op);
	token = token->next;
}

// 次の次のトークンが期待している記号ならtrueを返す
bool is_nextnext(char *op){
	if(memcmp(token->next->str, op, token->next->len)==0){
		return true;
	}
	return false;
}

// 次のトークンが数字の時はトークンを１つ読み進め、それ以外はエラー報告
int expect_number() {
	if(token->kind != TK_NUM)
		error_at(token->str, "kazuではありません");
	int val = token->val;
	token = token->next;
	return val;
}

// 次のトークンが変数の時Tokenをかえす
Token *consume_ident(){
	if(token->kind != TK_IDENT)
		return NULL;
	Token *rtoken = token;
	token = token->next;
	return rtoken;
}

// 変数を名前で検索する。なければNULL
LVar *find_lvar(Token *tok){
	for(LVar *var = locals; var; var= var->next)
		if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;
	return NULL;
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
	LVar *lvar = calloc(1, sizeof(LVar));
	lvar->next =locals;
	lvar->offset = 0;
	locals = lvar;

	int i = 0;
	while(!at_eof())
		code[i++] = stmt();

	code[i] = NULL;
}

Node *stmt() {
	Node *node;
	Node *node2;
	Node *node3;

	if(consume("{")){
		node = new_node(ND_BLOCK, NULL, NULL, NULL, NULL);
		node2 = node;
		while(!consume("}")){
			node2->next = stmt();
			node2 = node2->next;
		}		

	}else if(consume("if")){
		expect("(");
		node = expr();
		expect(")");
		node2 = stmt();
		if(consume("else")){
			node = new_node(ND_ELSE, node, node2, stmt(), NULL);
		}else{
			node = new_node(ND_IF, node, node2, NULL, NULL);
		}

	}else if(consume("while")){
		expect("(");
		node = expr();
		expect(")");
		node = new_node(ND_WHILE, node, stmt(), NULL, NULL);

	}else if(consume("for")){
		expect("(");
		if(consume(";")){
			node = NULL;
		}else{
			node = expr();
			expect(";");
		}
		if(consume(";")){
			node2 = NULL;
		}else{
			node2 = expr();
			expect(";");
		}
		if(consume(")")){
			node3 = NULL;
		}else{
			node3 = expr();
			expect(")");
		}
		node = new_node(ND_FOR, node, node2, node3, stmt());

	}else if(consume("return")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();

	}else{
		node = expr();

	}

	consume(";");

	return node;
}

Node *expr() {
	return assign();
}

Node *assign() {
	Node *node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign(), NULL, NULL);
	return node;
}

Node *equality() {
	Node *node = relational();

	for(;;){
		if(consume("=="))
			node = new_node(ND_EQ, node, relational(), NULL, NULL);
		else if(consume("!="))
			node = new_node(ND_NE, node, relational(), NULL, NULL);
		else
			return node;
	}
}

Node *relational() {
	Node *node = add();

	for(;;){
		if(consume("<"))
			node = new_node(ND_LT, node, add(), NULL, NULL);
		else if(consume("<="))
			node = new_node(ND_LE, node, add(), NULL, NULL);
		else if(consume(">"))
			node = new_node(ND_LT, add(), node, NULL, NULL);
		else if(consume(">="))
			node = new_node(ND_LE, add(), node, NULL, NULL);
		else
			return node;
	}
}

Node *add() {
	Node *node = mul();

	for(;;) {
		if(consume("+"))
			node = new_node(ND_ADD, node, mul(), NULL, NULL);
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul(), NULL, NULL);
		else
			return node;
	}
}

Node *mul(){
	Node *node = unary_on_primary();

	for(;;) {
		if(consume("*"))
			node = new_node(ND_MUL, node, unary_on_primary(), NULL, NULL);
		else if(consume("/"))
			node = new_node(ND_DIV, node, unary_on_primary(), NULL, NULL);
		else
			return node;
	}
}

Node *unary_on_primary(){
	if(consume("+"))
		return primary();
	if(consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary(), NULL, NULL);
	return primary();
}

Node *primary(){
	Node *node2;
	// 次のトークンが"("なら、"(" expr ")"
	if(consume( "(" ) ) {
		Node *node = expr();
		expect( ")" );
		return node;
	}

	// 変数or関数時の処理
	Token *tok = consume_ident();
	if(tok){
		// 関数時
		if(consume("(")){
			// 関数Node
			Node *node = calloc(1, sizeof(Node));
			node->kind = ND_FUNC;
			node->str =	tok->str;
			node->len = tok->len;

			node2 = node;
			while(!consume(")")){
				node2->next = expr();
				node2 = node2->next;
				if(consume(")"))
					break;
				expect(",");
			}		

			return node;

		}else{
			Node *node = calloc(1, sizeof(Node));
			node->kind = ND_LVAR;
	
			LVar *lvar = find_lvar(tok);
			if(lvar){
				node->offset = lvar->offset;
			}else{
				lvar = calloc(1, sizeof(LVar));
				lvar->next = locals;
				lvar->name = tok->str;
				lvar->len = tok->len;
				lvar->offset = locals->offset +8;
				node->offset = lvar->offset;
				locals = lvar;
			}
			return node;
		}
	}
	
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

