;;;; Sierpinski Disassembler ;;;;

main:

	push   320                     ; 00000000: 81 40 01 00 00 
	push   240                     ; 00000005: 81 F0 00 00 00 
	pop    scry                    ; 0000000A: 42 0C 
	pop    scrx                    ; 0000000C: 42 0B 
	push   10                      ; 0000000E: 81 0A 00 00 00 
	pop    [0]                     ; 00000013: A2 00 00 00 00 
	push   10                      ; 00000018: 81 0A 00 00 00 
	pop    [4]                     ; 0000001D: A2 04 00 00 00 
	push   300                     ; 00000022: 81 2C 01 00 00 
	pop    [8]                     ; 00000027: A2 08 00 00 00 
	push   120                     ; 0000002C: 81 78 00 00 00 
	pop    [12]                    ; 00000031: A2 0C 00 00 00 
	push   150                     ; 00000036: 81 96 00 00 00 
	pop    [16]                    ; 0000003B: A2 10 00 00 00 
	push   220                     ; 00000040: 81 DC 00 00 00 
	pop    [20]                    ; 00000045: A2 14 00 00 00 
	push   50000                   ; 0000004A: 81 50 C3 00 00 
	pop    [24]                    ; 0000004F: A2 18 00 00 00 
	push   1                       ; 00000054: 81 01 00 00 00 
	pop    [28]                    ; 00000059: A2 1C 00 00 00 
	push   scrx                    ; 0000005E: 41 0B 
	push   scry                    ; 00000060: 41 0C 
	mul                            ; 00000062: 07 
	push   3                       ; 00000063: 81 03 00 00 00 
	mul                            ; 00000068: 07 
	push   28                      ; 00000069: 81 1C 00 00 00 
	add                            ; 0000006E: 05 
	pop    [32]                    ; 0000006F: A2 20 00 00 00 
	push   36                      ; 00000074: 81 24 00 00 00 
	pop    rbp                     ; 00000079: 42 09 
	jmp    L0                      ; 0000007B: 19 87 00 00 00 

L1:

	push   0                       ; 00000080: 81 00 00 00 00 
	pop    [rbp]                   ; 00000085: 62 09 

L0:

	push   4                       ; 00000087: 81 04 00 00 00 
	push   rbp                     ; 0000008C: 41 09 
	add                            ; 0000008E: 05 
	pop    rbp                     ; 0000008F: 42 09 
	pushq  [32]                    ; 00000091: AD 20 00 00 00 
	pushq  rbp                     ; 00000096: 4D 09 
	jb     L1                      ; 00000098: 1E 80 00 00 00 
	push   [0]                     ; 0000009D: A1 00 00 00 00 
	pop    rax                     ; 000000A2: 42 05 
	push   [4]                     ; 000000A4: A1 04 00 00 00 
	pop    rbx                     ; 000000A9: 42 06 
	push   0                       ; 000000AB: 81 00 00 00 00 
	pop    rcx                     ; 000000B0: 42 07 
	jmp    L2                      ; 000000B2: 19 7B 01 00 00 

L14:

	push   3                       ; 000000B7: 81 03 00 00 00 
	call   L3                      ; 000000BC: 20 97 01 00 00 
	pop    eax                     ; 000000C1: 42 01 
	pushq  eax                     ; 000000C3: 4D 01 
	pushq  0.000000                ; 000000C5: 8D 00 00 00 00 00 00 00 00 
	je     L4                      ; 000000CE: 1A F8 00 00 00 
	pushq  eax                     ; 000000D3: 4D 01 
	pushq  1.000000                ; 000000D5: 8D 00 00 00 00 00 00 F0 3F 
	je     L5                      ; 000000DE: 1A 0C 01 00 00 
	pushq  eax                     ; 000000E3: 4D 01 
	pushq  2.000000                ; 000000E5: 8D 00 00 00 00 00 00 00 40 
	je     L6                      ; 000000EE: 1A 20 01 00 00 
	jmp    L7                      ; 000000F3: 19 7B 01 00 00 

L4:

	push   [0]                     ; 000000F8: A1 00 00 00 00 
	push   [4]                     ; 000000FD: A1 04 00 00 00 
	call   L8                      ; 00000102: 20 34 01 00 00 
	jmp    L9                      ; 00000107: 19 7B 01 00 00 

L5:

	push   [8]                     ; 0000010C: A1 08 00 00 00 
	push   [12]                    ; 00000111: A1 0C 00 00 00 
	call   L10                     ; 00000116: 20 34 01 00 00 
	jmp    L11                     ; 0000011B: 19 7B 01 00 00 

L6:

	push   [16]                    ; 00000120: A1 10 00 00 00 
	push   [20]                    ; 00000125: A1 14 00 00 00 
	call   L12                     ; 0000012A: 20 34 01 00 00 
	jmp    L13                     ; 0000012F: 19 7B 01 00 00 

L12:
L10:
L8:

	pop    ebx                     ; 00000134: 42 02 
	pop    eax                     ; 00000136: 42 01 
	push   eax                     ; 00000138: 41 01 
	push   rax                     ; 0000013A: 41 05 
	sub                            ; 0000013C: 06 
	push   2                       ; 0000013D: 81 02 00 00 00 
	div                            ; 00000142: 08 
	push   rax                     ; 00000143: 41 05 
	add                            ; 00000145: 05 
	pop    rax                     ; 00000146: 42 05 
	push   ebx                     ; 00000148: 41 02 
	push   rbx                     ; 0000014A: 41 06 
	sub                            ; 0000014C: 06 
	push   2                       ; 0000014D: 81 02 00 00 00 
	div                            ; 00000152: 08 
	push   rbx                     ; 00000153: 41 06 
	add                            ; 00000155: 05 
	pop    rbx                     ; 00000156: 42 06 
	push   rbx                     ; 00000158: 41 06 
	push   scrx                    ; 0000015A: 41 0B 
	mul                            ; 0000015C: 07 
	push   rax                     ; 0000015D: 41 05 
	add                            ; 0000015F: 05 
	push   3                       ; 00000160: 81 03 00 00 00 
	mul                            ; 00000165: 07 
	pop    rbp                     ; 00000166: 42 09 
	push   [rbp+36]                ; 00000168: E1 09 24 00 00 00 
	push   16777215                ; 0000016E: 81 FF FF FF 00 
	or                             ; 00000173: 0B 
	pop    [rbp+36]                ; 00000174: E2 09 24 00 00 00 
	ret                            ; 0000017A: 21 

L7:
L11:
L2:
L13:
L9:

	push   rcx                     ; 0000017B: 41 07 
	push   1                       ; 0000017D: 81 01 00 00 00 
	add                            ; 00000182: 05 
	pop    rcx                     ; 00000183: 42 07 
	push   [24]                    ; 00000185: A1 18 00 00 00 
	int2flt                        ; 0000018A: 23 
	pushq  rcx                     ; 0000018B: 4D 07 
	jb     L14                     ; 0000018D: 1E B7 00 00 00 
	jmp    L15                     ; 00000192: 19 C8 01 00 00 

L3:

	pop    eax                     ; 00000197: 42 01 
	push   214013                  ; 00000199: 81 FD 43 03 00 
	push   [28]                    ; 0000019E: A1 1C 00 00 00 
	mul                            ; 000001A3: 07 
	push   2531011                 ; 000001A4: 81 C3 9E 26 00 
	add                            ; 000001A9: 05 
	pop    [28]                    ; 000001AA: A2 1C 00 00 00 
	push   [28]                    ; 000001AF: A1 1C 00 00 00 
	push   65536                   ; 000001B4: 81 00 00 01 00 
	div                            ; 000001B9: 08 
	pop    ebx                     ; 000001BA: 42 02 
	push   ebx                     ; 000001BC: 41 02 
	push   ebx                     ; 000001BE: 41 02 
	push   eax                     ; 000001C0: 41 01 
	div                            ; 000001C2: 08 
	push   eax                     ; 000001C3: 41 01 
	mul                            ; 000001C5: 07 
	sub                            ; 000001C6: 06 
	ret                            ; 000001C7: 21 

L15:

	push   36                      ; 000001C8: 81 24 00 00 00 
	pop    rbp                     ; 000001CD: 42 09 
	screen rbp                     ; 000001CF: 24 09 
	hlt                            ; 000001D1: 00 
