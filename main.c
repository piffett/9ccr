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

	// 先頭の式から順にコード生成
	for(int i=0; code[i]; i++){
		gen(code[i]);
	}

	return 0;
}
