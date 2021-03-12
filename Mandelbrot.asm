;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Mandelbrot Assembler ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

push   320
push   240

pop scry       ; window height size
pop scrx       ; window width  size

push   2
pop    [0]     ; limit

push   100
pop    [4]     ; max iterations

pushq  1.3
popq   [8]     ; imaginary up border

pushq  -1.3
popq   [16]    ; imaginary down border

push   80
pop    rbp     ; video memory pointer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; counting for real left and right borders ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pushq  [8]
pushq  [16]
subq

pushq  scrx
mulq
pushq  scry
divq
pushq  5
divq
popq   rax

pushq  rax
pushq  3
mulq
negq
popq   [24]    ; real left border

pushq  rax
pushq  2
mulq
popq   [32]    ; real right border

;;;;;;;;;;;;;;;;;;;;;
;;;; double loop ;;;;
;;;;;;;;;;;;;;;;;;;;;

push   0
pop    rbx     ; y coord

jmp    y_after_loop

y_loop:

	push  0
	pop   rax     ; x coord

	jmp   x_after_loop

;;;;;;;;;;;;;;;;;;;
;;;; main loop ;;;;
;;;;;;;;;;;;;;;;;;;

x_loop:

	pushq [32]
	pushq [24]
	subq
	pushq rax
	mulq
	pushq scrx
	divq
	pushq [24]
	addq
	popq  [40]    ; point real part

	pushq [8]
	pushq [16]
	subq
	pushq rbx
	mulq
	pushq scry
	divq
	pushq [16]
	addq
	popq  [48]    ; point imaginary part

	pushq [40]
	pushq [40]
	mulq
	popq  [56]    ; re^2

	pushq [48]
	pushq [48]
	mulq
	popq  [64]    ; im^2

	pushq [40]
	pushq [48]
	addq
	popq  eax
	pushq eax
	pushq eax
	mulq
	popq  [72]    ; w

	push  0
	pop   rcx     ; i

	jmp   i_after_loop

;;;;;;;;;;;;;;;;;;;;;;;
;;;; point tracing ;;;;
;;;;;;;;;;;;;;;;;;;;;;;

i_loop:

	pushq [56]
	pushq [64]
	subq
	pushq [40]
	addq
	popq  eax     ; re new

	pushq [72]
	pushq [56]
	subq
	pushq [64]
	subq
	pushq [48]
	addq
	popq  ebx     ; im new

	pushq eax
	pushq eax
	mulq
	popq  [56]    ; re^2 new

	pushq ebx
	pushq ebx
	mulq
	popq  [64]    ; im^2 new

	pushq eax
	pushq ebx
	addq
	popq  eax
	pushq eax
	pushq eax
	mulq
	popq  [72]    ; w new

	push  1
	push  rcx
	add
	pop   rcx

;;;;;;;;;;;;;;;;;;;
i_after_loop:

	push  [4]
	int2flt
	push  rcx
	int2flt
	jae   mem_out

	pushq [56]
	pushq [64]
	addq
	push  [0]
	int2flt
	jae   i_loop

mem_out:
	
	push  [4]
	int2flt
	push  rcx
	int2flt
	jb    set_white
	
	push  0
	pop   [rbp]

	jmp   inc_rbp

set_white:
	
	push  16777215   ;; white rgb
	pop   [rbp]

inc_rbp:

	push  3
	push  rbp
	add
	pop   rbp

	push  1
	push  rax
	add
	pop   rax

;;;;;;;;;;;;;;;;;;;
x_after_loop:

	pushq scrx
	pushq rax
	jb    x_loop

	push  1
	push  rbx
	add
	pop   rbx

;;;;;;;;;;;;;;;;;;;
y_after_loop:

	pushq scry
	pushq rbx
	jb    y_loop

;;;;;;;;;;;;;;;;
;;;; SCREEN ;;;;
;;;;;;;;;;;;;;;;

push   80
pop    rbp     ; video memory pointer

screen rbp

hlt
