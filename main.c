#include "9cc.h"

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
