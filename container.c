#include "9cc.h"

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


