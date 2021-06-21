;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Circle Assembler ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;

main:

	in     rax

	push   1
	pop    rbx

	pushq  0
	pushq  rax
	jb     print_err

	call   factorial
	push   rbx
	out
	hlt

factorial:

	pushq  2
	pushq  rax
	jb     ret

	push   rax
	push   rbx
	mul
	pop    rbx

	push   rax
	push   1
	sub
	pop    rax

	call   factorial
	ret

	ret:
	ret

print_err:

	push   -666
	out

	hlt
