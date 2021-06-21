;;;; Circle Disassembler ;;;;

main:

	push   300                     ; 0x00000000: 81 2C 01 00 00 
	push   300                     ; 0x00000005: 81 2C 01 00 00 
	pop    scry                    ; 0x0000000A: 42 0C 
	pop    scrx                    ; 0x0000000C: 42 0B 
	push   140                     ; 0x0000000E: 81 8C 00 00 00 
	pop    [0]                     ; 0x00000013: A2 00 00 00 00 
	push   scrx                    ; 0x00000018: 41 0B 
	push   2                       ; 0x0000001A: 81 02 00 00 00 
	div                            ; 0x0000001F: 08 
	pop    [4]                     ; 0x00000020: A2 04 00 00 00 
	push   scry                    ; 0x00000025: 41 0C 
	push   2                       ; 0x00000027: 81 02 00 00 00 
	div                            ; 0x0000002C: 08 
	pop    [8]                     ; 0x0000002D: A2 08 00 00 00 
	push   12                      ; 0x00000032: 81 0C 00 00 00 
	pop    rbp                     ; 0x00000037: 42 09 
	push   0                       ; 0x00000039: 81 00 00 00 00 
	pop    rbx                     ; 0x0000003E: 42 06 
	jmp    L0                      ; 0x00000040: 19 F5 00 00 00 

L5:

	push   0                       ; 0x00000045: 81 00 00 00 00 
	pop    rax                     ; 0x0000004A: 42 05 
	jmp    L1                      ; 0x0000004C: 19 E2 00 00 00 

L4:

	push   rax                     ; 0x00000051: 41 05 
	push   [4]                     ; 0x00000053: A1 04 00 00 00 
	sub                            ; 0x00000058: 06 
	pop    eax                     ; 0x00000059: 42 01 
	push   eax                     ; 0x0000005B: 41 01 
	push   eax                     ; 0x0000005D: 41 01 
	mul                            ; 0x0000005F: 07 
	push   rbx                     ; 0x00000060: 41 06 
	push   [8]                     ; 0x00000062: A1 08 00 00 00 
	sub                            ; 0x00000067: 06 
	pop    eax                     ; 0x00000068: 42 01 
	push   eax                     ; 0x0000006A: 41 01 
	push   eax                     ; 0x0000006C: 41 01 
	mul                            ; 0x0000006E: 07 
	add                            ; 0x0000006F: 05 
	int2flt                        ; 0x00000070: 23 
	sqrt                           ; 0x00000071: 18 
	popq   eax                     ; 0x00000072: 4E 01 
	pushq  eax                     ; 0x00000074: 4D 01 
	push   [0]                     ; 0x00000076: A1 00 00 00 00 
	int2flt                        ; 0x0000007B: 23 
	jb     L2                      ; 0x0000007C: 1E C7 00 00 00 
	pushq  scry                    ; 0x00000081: 4D 0C 
	pushq  rbx                     ; 0x00000083: 4D 06 
	subq                           ; 0x00000085: 12 
	pushq  scrx                    ; 0x00000086: 4D 0B 
	divq                           ; 0x00000088: 14 
	pushq  256.000000              ; 0x00000089: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 0x00000092: 13 
	flt2int                        ; 0x00000093: 22 
	pop    [rbp]                   ; 0x00000094: 62 09 
	pushq  rax                     ; 0x00000096: 4D 05 
	pushq  scrx                    ; 0x00000098: 4D 0B 
	divq                           ; 0x0000009A: 14 
	pushq  256.000000              ; 0x0000009B: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 0x000000A4: 13 
	flt2int                        ; 0x000000A5: 22 
	pop    [rbp+1]                 ; 0x000000A6: E2 09 01 00 00 00 
	pushq  eax                     ; 0x000000AC: 4D 01 
	pushq  scry                    ; 0x000000AE: 4D 0C 
	divq                           ; 0x000000B0: 14 
	pushq  256.000000              ; 0x000000B1: 8D 00 00 00 00 00 00 70 40 
	mulq                           ; 0x000000BA: 13 
	flt2int                        ; 0x000000BB: 22 
	pop    [rbp+2]                 ; 0x000000BC: E2 09 02 00 00 00 
	jmp    L3                      ; 0x000000C2: 19 CE 00 00 00 

L2:

	push   16777215                ; 0x000000C7: 81 FF FF FF 00 
	pop    [rbp]                   ; 0x000000CC: 62 09 

L3:

	push   3                       ; 0x000000CE: 81 03 00 00 00 
	push   rbp                     ; 0x000000D3: 41 09 
	add                            ; 0x000000D5: 05 
	pop    rbp                     ; 0x000000D6: 42 09 
	push   1                       ; 0x000000D8: 81 01 00 00 00 
	push   rax                     ; 0x000000DD: 41 05 
	add                            ; 0x000000DF: 05 
	pop    rax                     ; 0x000000E0: 42 05 

L1:

	pushq  scrx                    ; 0x000000E2: 4D 0B 
	pushq  rax                     ; 0x000000E4: 4D 05 
	jb     L4                      ; 0x000000E6: 1E 51 00 00 00 
	push   1                       ; 0x000000EB: 81 01 00 00 00 
	push   rbx                     ; 0x000000F0: 41 06 
	add                            ; 0x000000F2: 05 
	pop    rbx                     ; 0x000000F3: 42 06 

L0:

	pushq  scry                    ; 0x000000F5: 4D 0C 
	pushq  rbx                     ; 0x000000F7: 4D 06 
	jb     L5                      ; 0x000000F9: 1E 45 00 00 00 
	push   12                      ; 0x000000FE: 81 0C 00 00 00 
	pop    rbp                     ; 0x00000103: 42 09 
	screen rbp                     ; 0x00000105: 24 09 
	hlt                            ; 0x00000107: 00 
