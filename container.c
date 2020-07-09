#include "9cc.h"

// トークナイズ時のエラー報告関数。
void error_at(char *loc, char *fmt, ...){
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
bool front_cmp(char *p, char *q){
	return memcmp(p, q, strlen(q)) == 0;
}

// 英数字かアンダースコアか判定する
int is_alnum(char c){
	return ('a' <= c && c <= 'z')||
			('A'<= c && c <= 'A')||
			('0' <= c && c <= '9')||
			(c == '_');
}

// 変数名となるところまでポインタを進めて、変数の文字列を返す
char *strtovar(char *p, char **endptr){
	char *start_p = p;
	int word_len = 0; // 文字数
	for(;;){
		if(is_alnum(*p)){
			p++;
			word_len++;
		}else{
			break;
		}
	}

	char *var = calloc(word_len+1, sizeof(char));
	for(int i =0; i < word_len; i++){
		var[i] = *(start_p+i);
	}
	var[word_len] = '\0';
	*endptr = p;
	return var;
}

// 入力文字列をトークナイズして返す
void tokenize(char *p){
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
				front_cmp(p, "(") ||front_cmp(p, ")")||front_cmp(p, ">")||front_cmp(p, "<")||
				front_cmp(p, "=") || front_cmp(p, ";") || front_cmp(p, "{") || front_cmp(p, "}")||
			front_cmp(p, ",") ){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		// return 文のときトークン作成
		if(front_cmp(p, "return") && !is_alnum(p[6])){
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		// if文の時トークン作成
		if(front_cmp(p, "if") && !is_alnum(p[2])){
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		// else の時トークン作成
		if(front_cmp(p, "else") && !is_alnum(p[4])){
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		// while 文の時トークン作成
		if(front_cmp(p, "while") && !is_alnum(p[5])){
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		// for 文の時トークン作成
		if(front_cmp(p, "for") && !is_alnum(p[3])){
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}
		
		// 整数の時トークンを作成
		if(isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			char *start_p = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - start_p;
			continue;
		}
		// 変数の時識別子のトークンを作成
		if (is_alnum(*p)){
			cur = new_token(TK_IDENT, cur, p, 1);
			char *start_p = p;
			cur->str = strtovar(p, &p);
			cur->len = p - start_p;
			continue;
		}
		
		
		

		error(token->str, "トークナイズできません");
	}

	new_token(TK_EOF, cur, p, 0);
	
	token = head.next;
}


