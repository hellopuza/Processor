;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Circle Assembler ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;

main:

	push   300
	push   300

	pop    scry      ; window height size
	pop    scrx      ; window width  size

	push   140
	pop    [0]       ; rad

	push   scrx
	push   2
	div
	pop    [4]       ; center x coord

	push   scry
	push   2
	div
	pop    [8]       ; center y coord

	push   12
	pop    rbp       ; video memory pointer

;;;;;;;;;;;;;;;;;;;;;
;;;; double loop ;;;;
;;;;;;;;;;;;;;;;;;;;;

	push   0
	pop    rbx       ; y coord

	jmp    y_after_loop

y_loop:

	push   0
	pop    rax       ; x coord

	jmp    x_after_loop

;;;;;;;;;;;;;;;;;;;
;;;; main loop ;;;;
;;;;;;;;;;;;;;;;;;;

x_loop:

	push   rax
	push   [4]
	sub
	pop    eax
	push   eax
	push   eax
	mul

	push   rbx
	push   [8]
	sub
	pop    eax
	push   eax
	push   eax
	mul

	add
	int2flt
	sqrt
	popq   eax
	pushq  eax
	push   [0]
	int2flt
	jb     set_white
	

	pushq  scry
	pushq  rbx
	subq
	pushq  scrx
	divq
	pushq  256
	mulq
	flt2int
	pop    [rbp]     ; red
	
	pushq  rax
	pushq  scrx
	divq
	pushq  256
	mulq
	flt2int
	pop    [rbp+1]   ; green

	pushq  eax
	pushq  scry
	divq
	pushq  256
	mulq
	flt2int
	pop    [rbp+2]   ; blue

	jmp    inc_rbp

set_white:
	
	push   16777215  ; white rgb
	pop    [rbp]

inc_rbp:

	push   3
	push   rbp
	add
	pop    rbp

	push   1
	push   rax
	add
	pop    rax

;;;;;;;;;;;;;;;;;;;
x_after_loop:

	pushq  scrx
	pushq  rax
	jb     x_loop

	push   1
	push   rbx
	add
	pop    rbx

;;;;;;;;;;;;;;;;;;;
y_after_loop:

	pushq  scry
	pushq  rbx
	jb     y_loop

;;;;;;;;;;;;;;;;
;;;; SCREEN ;;;;
;;;;;;;;;;;;;;;;

	push   12
	pop    rbp       ; video memory pointer

	screen rbp

	hlt
