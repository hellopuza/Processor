;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Sierpinski triangle Assembler ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

push   320
push   240

pop    scry    ; window height size
pop    scrx    ; window width  size

push   10
pop    [0]     ; first point x coord

push   10
pop    [4]     ; first point y coord

push   300
pop    [8]     ; second point x coord

push   120
pop    [12]    ; second point y coord

push   150
pop    [16]    ; third point x coord

push   220
pop    [20]    ; third point y coord

push   50000
pop    [24]    ; max iterations

push   1
pop    [28]    ; random param next

;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; set window black ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;

push   scrx
push   scry
mul
push   3
mul
push   28
add
pop    [32]    ; video memory size

push   36
pop    rbp     ; video memory pointer

jmp    rbp_after_loop

rbp_loop:

	push  0
	pop   [rbp]

rbp_after_loop:

	push  4
	push  rbp
	add
	pop   rbp

	pushq [32]
	pushq rbp
	jb    rbp_loop

;;;;;;;;;;;;;;;;;;;
;;;; main loop ;;;;
;;;;;;;;;;;;;;;;;;;

	push  [0]
	pop   rax     ; x

	push  [4]
	pop   rbx     ; y

	push  0
	pop   rcx     ; i
	
	jmp   i_after_loop

i_loop:

	push  3
	call  random
	pop   eax

	pushq eax
	pushq 0
	je    first_point

	pushq eax
	pushq 1
	je    second_point

	pushq eax
	pushq 2
	je    third_point

	jmp   i_after_loop

first_point:

	push  [0]
	push  [4]
	call  set_point
	jmp   i_after_loop

second_point:

	push  [8]
	push  [12]
	call  set_point
	jmp   i_after_loop

third_point:

	push  [16]
	push  [20]
	call  set_point
	jmp   i_after_loop

set_point:

	pop   ebx
	pop   eax

	push  eax
	push  rax
	sub
	push  2
	div
	push  rax
	add
	pop   rax

	push  ebx
	push  rbx
	sub
	push  2
	div
	push  rbx
	add
	pop   rbx

	;;;;; rbp = (y * width + x) * 3;

	push  rbx
	push  scrx
	mul
	push  rax
	add
	push  3
	mul
	pop   rbp

	push  [rbp+36]
	push  16777215   ;; white rgb
	or
	pop   [rbp+36]

	ret

i_after_loop:

	push  rcx
	push  1
	add
	pop   rcx

	push [24]
	int2flt
	pushq rcx
	jb    i_loop
	jmp   screen_out

random:
	pop   eax

	;;;;; next = next * 214013 + 2531011;
        ;;;;; ebx  = next / 65536;
	
	push  214013
	push  [28]
	mul
	push  2531011
	add
	pop   [28]
	push  [28]
	push  65536
	div
	pop   ebx

	;;;;; ebx % eax
	;;;;; push = ebx - (ebx / eax) * eax;
	
	push  ebx
	push  ebx
	push  eax
	div
	push  eax
	mul
	sub
	
	ret

;;;;;;;;;;;;;;;;
;;;; SCREEN ;;;;
;;;;;;;;;;;;;;;;

screen_out:

	push   36
	pop    rbp     ; video memory pointer

	screen rbp
hlt