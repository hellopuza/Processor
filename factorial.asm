in rax

push 1
pop rbx

pushq 0
pushq rax
jb less_zero

call factorial
pushq rbx
out
hlt

;;;;;;;;;;;;;;;;;;;;

factorial:

pushq 2
pushq rax
jb ret

push rax
push rbx
mul
pop rbx

push rax
push 1
sub
pop rax

call factorial
ret

ret:
ret

;;;;;;;;;;;;;;;;;;;;

less_zero:
push -666
pop rbx
pushq rbx
out
hlt
