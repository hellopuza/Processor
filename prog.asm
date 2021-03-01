in rax

push 1
pop rbx

push rax
push 0
jb less_zero

call factorial
push rbx
out
hlt


factorial: 

push rax
push 2
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

less_zero:
push -666
pop rbx
push rbx
out
hlt
