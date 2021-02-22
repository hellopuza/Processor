push 1
pop rax

sas:
push rax
push rax
mul
out

push rax
push 1
add
pop rax
jmp :sas