#include "9cc.h"

int main(int argc, char **argv){
	if(argc != 2){
		error("引数の個数は正しくありません");
		return 1;
	}

	// ジャンプのラベル用
	label_num = 0;

	// トークナイズしてパース
	//結果はcodeに保存
	user_input = argv[1];
	tokenize(user_input);
	program();


	// アセンブリの前半部分
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// プロローグ
	// 変数26個分の領域確保
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, 208\n");

	// 先頭の式から順にコード生成
	for(int i=0; code[i]; i++){
		gen(code[i]);

		// 式を評価した後一つ値が残っているはずなのでポップ
		printf("	pop rax\n");
	}

	// エピローグ
	// 最後の式の値がRAXに残っているのでロードして返り値にセット
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return 0;
}
