;;;; Mandelbrot Disassembler ;;;;

main:

	push   320                     ; 00000000: 81 40 01 00 00 
	push   240                     ; 00000005: 81 F0 00 00 00 
	pop    scry                    ; 0000000A: 42 0C 
	pop    scrx                    ; 0000000C: 42 0B 
	push   2                       ; 0000000E: 81 02 00 00 00 
	pop    [0]                     ; 00000013: A2 00 00 00 00 
	push   100                     ; 00000018: 81 64 00 00 00 
	pop    [4]                     ; 0000001D: A2 04 00 00 00 
	pushq  1.300000                ; 00000022: 8D CD CC CC CC CC CC F4 3F 
	popq   [8]                     ; 0000002B: AE 08 00 00 00 
	pushq  -1.300000               ; 00000030: 8D CD CC CC CC CC CC F4 BF 
	popq   [16]                    ; 00000039: AE 10 00 00 00 
	push   80                      ; 0000003E: 81 50 00 00 00 
	pop    rbp                     ; 00000043: 42 09 
	pushq  [8]                     ; 00000045: AD 08 00 00 00 
	pushq  [16]                    ; 0000004A: AD 10 00 00 00 
	subq                           ; 0000004F: 12 
	pushq  scrx                    ; 00000050: 4D 0B 
	mulq                           ; 00000052: 13 
	pushq  scry                    ; 00000053: 4D 0C 
	divq                           ; 00000055: 14 
	pushq  5.000000                ; 00000056: 8D 00 00 00 00 00 00 14 40 
	divq                           ; 0000005F: 14 
	popq   rax                     ; 00000060: 4E 05 
	pushq  rax                     ; 00000062: 4D 05 
	pushq  3.000000                ; 00000064: 8D 00 00 00 00 00 00 08 40 
	mulq                           ; 0000006D: 13 
	negq                           ; 0000006E: 15 
	popq   [24]                    ; 0000006F: AE 18 00 00 00 
	pushq  rax                     ; 00000074: 4D 05 
	pushq  2.000000                ; 00000076: 8D 00 00 00 00 00 00 00 40 
	mulq                           ; 0000007F: 13 
	popq   [32]                    ; 00000080: AE 20 00 00 00 
	push   0                       ; 00000085: 81 00 00 00 00 
	pop    rbx                     ; 0000008A: 42 06 
	jmp    L0                      ; 0000008C: 19 DF 01 00 00 

L8:

	push   0                       ; 00000091: 81 00 00 00 00 
	pop    rax                     ; 00000096: 42 05 
	jmp    L1                      ; 00000098: 19 CC 01 00 00 

L7:

	pushq  [32]                    ; 0000009D: AD 20 00 00 00 
	pushq  [24]                    ; 000000A2: AD 18 00 00 00 
	subq                           ; 000000A7: 12 
	pushq  rax                     ; 000000A8: 4D 05 
	mulq                           ; 000000AA: 13 
	pushq  scrx                    ; 000000AB: 4D 0B 
	divq                           ; 000000AD: 14 
	pushq  [24]                    ; 000000AE: AD 18 00 00 00 
	addq                           ; 000000B3: 11 
	popq   [40]                    ; 000000B4: AE 28 00 00 00 
	pushq  [8]                     ; 000000B9: AD 08 00 00 00 
	pushq  [16]                    ; 000000BE: AD 10 00 00 00 
	subq                           ; 000000C3: 12 
	pushq  rbx                     ; 000000C4: 4D 06 
	mulq                           ; 000000C6: 13 
	pushq  scry                    ; 000000C7: 4D 0C 
	divq                           ; 000000C9: 14 
	pushq  [16]                    ; 000000CA: AD 10 00 00 00 
	addq                           ; 000000CF: 11 
	popq   [48]                    ; 000000D0: AE 30 00 00 00 
	pushq  [40]                    ; 000000D5: AD 28 00 00 00 
	pushq  [40]                    ; 000000DA: AD 28 00 00 00 
	mulq                           ; 000000DF: 13 
	popq   [56]                    ; 000000E0: AE 38 00 00 00 
	pushq  [48]                    ; 000000E5: AD 30 00 00 00 
	pushq  [48]                    ; 000000EA: AD 30 00 00 00 
	mulq                           ; 000000EF: 13 
	popq   [64]                    ; 000000F0: AE 40 00 00 00 
	pushq  [40]                    ; 000000F5: AD 28 00 00 00 
	pushq  [48]                    ; 000000FA: AD 30 00 00 00 
	addq                           ; 000000FF: 11 
	popq   eax                     ; 00000100: 4E 01 
	pushq  eax                     ; 00000102: 4D 01 
	pushq  eax                     ; 00000104: 4D 01 
	mulq                           ; 00000106: 13 
	popq   [72]                    ; 00000107: AE 48 00 00 00 
	push   0                       ; 0000010C: 81 00 00 00 00 
	pop    rcx                     ; 00000111: 42 07 
	jmp    L2                      ; 00000113: 19 73 01 00 00 

L4:

	pushq  [56]                    ; 00000118: AD 38 00 00 00 
	pushq  [64]                    ; 0000011D: AD 40 00 00 00 
	subq                           ; 00000122: 12 
	pushq  [40]                    ; 00000123: AD 28 00 00 00 
	addq                           ; 00000128: 11 
	popq   eax                     ; 00000129: 4E 01 
	pushq  [72]                    ; 0000012B: AD 48 00 00 00 
	pushq  [56]                    ; 00000130: AD 38 00 00 00 
	subq                           ; 00000135: 12 
	pushq  [64]                    ; 00000136: AD 40 00 00 00 
	subq                           ; 0000013B: 12 
	pushq  [48]                    ; 0000013C: AD 30 00 00 00 
	addq                           ; 00000141: 11 
	popq   ebx                     ; 00000142: 4E 02 
	pushq  eax                     ; 00000144: 4D 01 
	pushq  eax                     ; 00000146: 4D 01 
	mulq                           ; 00000148: 13 
	popq   [56]                    ; 00000149: AE 38 00 00 00 
	pushq  ebx                     ; 0000014E: 4D 02 
	pushq  ebx                     ; 00000150: 4D 02 
	mulq                           ; 00000152: 13 
	popq   [64]                    ; 00000153: AE 40 00 00 00 
	pushq  eax                     ; 00000158: 4D 01 
	pushq  ebx                     ; 0000015A: 4D 02 
	addq                           ; 0000015C: 11 
	popq   eax                     ; 0000015D: 4E 01 
	pushq  eax                     ; 0000015F: 4D 01 
	pushq  eax                     ; 00000161: 4D 01 
	mulq                           ; 00000163: 13 
	popq   [72]                    ; 00000164: AE 48 00 00 00 
	push   1                       ; 00000169: 81 01 00 00 00 
	push   rcx                     ; 0000016E: 41 07 
	add                            ; 00000170: 05 
	pop    rcx                     ; 00000171: 42 07 

L2:

	push   [4]                     ; 00000173: A1 04 00 00 00 
	int2flt                        ; 00000178: 23 
	push   rcx                     ; 00000179: 41 07 
	int2flt                        ; 0000017B: 23 
	jae    L3                      ; 0000017C: 1D 97 01 00 00 
	pushq  [56]                    ; 00000181: AD 38 00 00 00 
	pushq  [64]                    ; 00000186: AD 40 00 00 00 
	addq                           ; 0000018B: 11 
	push   [0]                     ; 0000018C: A1 00 00 00 00 
	int2flt                        ; 00000191: 23 
	jae    L4                      ; 00000192: 1D 18 01 00 00 

L3:

	push   [4]                     ; 00000197: A1 04 00 00 00 
	int2flt                        ; 0000019C: 23 
	push   rcx                     ; 0000019D: 41 07 
	int2flt                        ; 0000019F: 23 
	jb     L5                      ; 000001A0: 1E B1 01 00 00 
	push   0                       ; 000001A5: 81 00 00 00 00 
	pop    [rbp]                   ; 000001AA: 62 09 
	jmp    L6                      ; 000001AC: 19 B8 01 00 00 

L5:

	push   16777215                ; 000001B1: 81 FF FF FF 00 
	pop    [rbp]                   ; 000001B6: 62 09 

L6:

	push   3                       ; 000001B8: 81 03 00 00 00 
	push   rbp                     ; 000001BD: 41 09 
	add                            ; 000001BF: 05 
	pop    rbp                     ; 000001C0: 42 09 
	push   1                       ; 000001C2: 81 01 00 00 00 
	push   rax                     ; 000001C7: 41 05 
	add                            ; 000001C9: 05 
	pop    rax                     ; 000001CA: 42 05 

L1:

	pushq  scrx                    ; 000001CC: 4D 0B 
	pushq  rax                     ; 000001CE: 4D 05 
	jb     L7                      ; 000001D0: 1E 9D 00 00 00 
	push   1                       ; 000001D5: 81 01 00 00 00 
	push   rbx                     ; 000001DA: 41 06 
	add                            ; 000001DC: 05 
	pop    rbx                     ; 000001DD: 42 06 

L0:

	pushq  scry                    ; 000001DF: 4D 0C 
	pushq  rbx                     ; 000001E1: 4D 06 
	jb     L8                      ; 000001E3: 1E 91 00 00 00 
	push   80                      ; 000001E8: 81 50 00 00 00 
	pop    rbp                     ; 000001ED: 42 09 
	screen rbp                     ; 000001EF: 24 09 
	hlt                            ; 000001F1: 00 
