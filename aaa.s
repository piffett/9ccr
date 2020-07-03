.intel_syntax noprefix
.global main
main:
	push rbp
	mov rbp, rsp
	sub rsp, 208
	mov rax, rbp
	sub rax, 8
	push rax
	push 0
	pop rdi
	pop rax
	mov [rax], rdi
	push rdi
.L:1
	mov rax, rbp
	sub rax, 8
	push rax
	pop rax
	mov rax, [rax]
	push rax
	push 5
	pop rdi
	pop rax
	cmp rax, rdi
	setl al
	movzb rax, al
	push rax
	pop rax
	cmp rax, 0
	je .L0
	mov rax, rbp
	sub rax, 16
	push rax
	push 1
	pop rdi
	pop rax
	mov [rax], rdi
	push rdi
	mov rax, rbp
	sub rax, 8
	push rax
	mov rax, rbp
	sub rax, 8
	push rax
	pop rax
	mov rax, [rax]
	push rax
	push 1
	pop rdi
	pop rax
	add rax, rdi
	push rax
	pop rdi
	pop rax
	mov [rax], rdi
	push rdi
	jmp .L1
.L:0
	pop rax
	mov rax, rbp
	sub rax, 8
	push rax
	pop rax
	mov rax, [rax]
	push rax
	pop rax
	mov rsp, rbp
	pop rbp
	ret
	pop rax
	mov rsp, rbp
	pop rbp
	ret
