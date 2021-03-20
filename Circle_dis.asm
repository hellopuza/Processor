;;;; Circle Disassembler ;;;;

main:

	push   300                     ; 00000000: 81 2C 01 00 00 
	push   300                     ; 00000005: 81 2C 01 00 00 
	pop    scry                    ; 0000000A: 42 0C 
	pop    scrx                    ; 0000000C: 42 0B 
	push   140                     ; 0000000E: 81 8C 00 00 00 
	pop    [0]                     ; 00000013: A2 00 00 00 00 
	push   scrx                    ; 00000018: 41 0B 
	push   2                       ; 0000001A: 81 02 00 00 00 
	div                            ; 0000001F: 08 
	pop    [4]                     ; 00000020: A2 04 00 00 00 
	push   scry                    ; 00000025: 41 0C 
	push   2                       ; 00000027: 81 02 00 00 00 
	div                            ; 0000002C: 08 
	pop    [8]                     ; 0000002D: A2 08 00 00 00 
	push   12                      ; 00000032: 81 0C 00 00 00 
	pop    rbp                     ; 00000037: 42 09 
	push   0                       ; 00000039: 81 00 00 00 00 
	pop    rbx                     ; 0000003E: 42 06 
	jmp    L0                      ; 00000040: 19 F5 00 00 00 

L5:

	push   0                       ; 00000045: 81 00 00 00 00 
	pop    rax                     ; 0000004A: 42 05 
	jmp    L1                      ; 0000004C: 19 E2 00 00 00 

L4:

	push   rax                     ; 00000051: 41 05 
	push   [4]                     ; 00000053: A1 04 00 00 00 
	sub                            ; 00000058: 06 
	pop    eax                     ; 00000059: 42 01 
	push   eax                     ; 0000005B: 41 01 
	push   eax                     ; 0000005D: 41 01 
	mul                            ; 0000005F: 07 
	push   rbx                     ; 00000060: 41 06 
	push   [8]                     ; 00000062: A1 08 00 00 00 
	sub                            ; 00000067: 06 
	pop    eax                     ; 00000068: 42 01 
	push   eax                     ; 0000006A: 41 01 
	push   eax                     ; 0000006C: 41 01 
	mul                            ; 0000006E: 07 
	add                            ; 0000006F: 05 
	int2flt                        ; 00000070: 23 
	sqrt                           ; 00000071: 18 
	popq   eax                     ; 00000072: 4E 01 
	pushq  eax                     ; 00000074: 4D 01 
	push   [0]                     ; 00000076: A1 00 00 00 00 
	int2flt                        ; 0000007B: 23 
	jb     L2                      ; 0000007C: 1E C7 00 00 00 
	pushq  scry                    ; 00000081: 4D 0C 
	pushq  rbx                     ; 00000083: 4D 06 
	subq                           ; 00000085: 12 
	pushq  scrx                    ; 00000086: 4D 0B 
	divq                           ; 00000088: 14 
	pushq  256.000000              ; 00000089: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 00000092: 13 
	flt2int                        ; 00000093: 22 
	pop    [rbp]                   ; 00000094: 62 09 
	pushq  rax                     ; 00000096: 4D 05 
	pushq  scrx                    ; 00000098: 4D 0B 
	divq                           ; 0000009A: 14 
	pushq  256.000000              ; 0000009B: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 000000A4: 13 
	flt2int                        ; 000000A5: 22 
	pop    [rbp+1]                 ; 000000A6: E2 09 01 00 00 00 
	pushq  eax                     ; 000000AC: 4D 01 
	pushq  scry                    ; 000000AE: 4D 0C 
	divq                           ; 000000B0: 14 
	pushq  256.000000              ; 000000B1: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 000000BA: 13 
	flt2int                        ; 000000BB: 22 
	pop    [rbp+2]                 ; 000000BC: E2 09 02 00 00 00 
	jmp    L3                      ; 000000C2: 19 CE 00 00 00 

L2:

	push   16777215                ; 000000C7: 81 FF FF FF 00 
	pop    [rbp]                   ; 000000CC: 62 09 

L3:

	push   3                       ; 000000CE: 81 03 00 00 00 
	push   rbp                     ; 000000D3: 41 09 
	add                            ; 000000D5: 05 
	pop    rbp                     ; 000000D6: 42 09 
	push   1                       ; 000000D8: 81 01 00 00 00 
	push   rax                     ; 000000DD: 41 05 
	add                            ; 000000DF: 05 
	pop    rax                     ; 000000E0: 42 05 

L1:

	pushq  scrx                    ; 000000E2: 4D 0B 
	pushq  rax                     ; 000000E4: 4D 05 
	jb     L4                      ; 000000E6: 1E 51 00 00 00 
	push   1                       ; 000000EB: 81 01 00 00 00 
	push   rbx                     ; 000000F0: 41 06 
	add                            ; 000000F2: 05 
	pop    rbx                     ; 000000F3: 42 06 

L0:

	pushq  scry                    ; 000000F5: 4D 0C 
	pushq  rbx                     ; 000000F7: 4D 06 
	jb     L5                      ; 000000F9: 1E 45 00 00 00 
	push   12                      ; 000000FE: 81 0C 00 00 00 
	pop    rbp                     ; 00000103: 42 09 
	screen rbp                     ; 00000105: 24 09 
	hlt                            ; 00000107: 00 
