#include "9cc.h"


void gen_lval(Node *node){
	if(node->kind != ND_LVAR)
		error("代入の左辺値が変数ではありません");

	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}

char *argRegs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node){
	int tmp, tmp2, argCount;
	switch(node->kind){
		case ND_NUM:
			printf("	push %d\n", node->val);
			return;
		case ND_LVAR:
			gen_lval(node);
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);
			gen(node->rhs);

			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			return;
		case ND_RETURN:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	mov rsp, rbp\n");
			printf("	pop rbp\n");
			printf("	ret\n");
			return;
		case ND_IF:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .L%d\n", label_num);
			tmp = label_num;
			label_num++;
			gen(node->rhs);
			printf(".L%d:\n", tmp);
			return;
		case ND_ELSE:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			tmp = label_num;
			printf("	je .L%d\n", tmp);
			label_num++;
			tmp2 = label_num;
			gen(node->rhs);
			printf("	jmp .L%d\n", tmp2);
			label_num++;
			printf(".L%d:\n", tmp);
			gen(node->third);
			printf(".L%d:\n", tmp2);
			return;
		case ND_WHILE:
			label_num += 2;
			tmp = label_num;
			printf(".L%d:\n", label_num-1);
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .L%d\n", label_num-2);
			gen(node->rhs);
			printf("	jmp .L%d\n", label_num-1);
			printf(".L%d:\n", label_num-2);
			return;
		case ND_FOR:
			tmp = label_num;
			tmp2 = label_num+1;
			label_num += 2;

			gen(node->lhs);
			printf(".L%d:\n", tmp2);
			gen(node->rhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .L%d\n", tmp);
			gen(node->forth);
			gen(node->third);
			printf("	jmp .L%d\n", tmp2);
			printf(".L%d:\n", tmp);
			return;
		case ND_BLOCK:
			for(Node *nvar = node->next; nvar; nvar = nvar->next){
				gen(nvar);
			}
			return;
		case ND_FUNC:
			{
				argCount = 0;
				for(Node *nex = node->next; nex; nex = nex->next){
					gen(nex);
					argCount++;
				}

				for(int i = argCount - 1; i >=0; i--){
					printf("	pop %s\n", argRegs[i]);
				}
				tmp = label_num;
				label_num++;
				printf("	mov rax, rsp\n");
				printf("	and rax, 15\n");
				printf("	jnz .L.call.%d\n", tmp);
				printf("	mov rax, 0\n");
				printf("	call %s\n", node->str);
				printf("	jmp .L.end.%d\n", tmp);
				printf(".L.call.%d:\n", tmp);
				printf("	sub rsp, 8\n");
				printf("	mov rax, 0\n");
				printf("	call %s\n", node->str);
				printf("	add rsp, 8\n");
				printf(".L.end.%d:\n", tmp);
				printf("	push rax\n");
				return;
			}
		case ND_FUNC_DEF:
			{
				printf("%s:\n", node->str);
				// プロローグ
				// 変数26個分の領域確保
				printf("	push rbp\n");
				printf("	mov rbp, rsp\n");
				printf("	sub rsp, 208\n");
				
				argCount = 0;

				for(Node *nex = node->rhs; nex; nex=nex->next){
					gen_lval(nex);
					printf("	mov [rax], %s\n", argRegs[argCount]);
					printf("	push rdi\n");
					argCount++;
				}
				gen(node->lhs);

				return;

			}
		case ND_ADDR:
			gen_lval(node->lhs);
			return;
		case ND_DEREF:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
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


