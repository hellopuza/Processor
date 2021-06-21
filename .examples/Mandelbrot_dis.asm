;;;; Mandelbrot Disassembler ;;;;

main:

	push   320                     ; 0x00000000: 81 40 01 00 00 
	push   240                     ; 0x00000005: 81 F0 00 00 00 
	pop    scry                    ; 0x0000000A: 42 0C 
	pop    scrx                    ; 0x0000000C: 42 0B 
	push   2                       ; 0x0000000E: 81 02 00 00 00 
	pop    [0]                     ; 0x00000013: A2 00 00 00 00 
	push   100                     ; 0x00000018: 81 64 00 00 00 
	pop    [4]                     ; 0x0000001D: A2 04 00 00 00 
	pushq  1.300000                ; 0x00000022: 8D CD CC CC CC CC CC F4 3F 
	popq   [8]                     ; 0x0000002B: AE 08 00 00 00 
	pushq  -1.300000               ; 0x00000030: 8D CD CC CC CC CC CC F4 BF 
	popq   [16]                    ; 0x00000039: AE 10 00 00 00 
	push   80                      ; 0x0000003E: 81 50 00 00 00 
	pop    rbp                     ; 0x00000043: 42 09 
	pushq  [8]                     ; 0x00000045: AD 08 00 00 00 
	pushq  [16]                    ; 0x0000004A: AD 10 00 00 00 
	subq                           ; 0x0000004F: 12 
	pushq  scrx                    ; 0x00000050: 4D 0B 
	mulq                           ; 0x00000052: 13 
	pushq  scry                    ; 0x00000053: 4D 0C 
	divq                           ; 0x00000055: 14 
	pushq  5.000000                ; 0x00000056: 8D 00 00 00 00 00 00 14 40 
	divq                           ; 0x0000005F: 14 
	popq   rax                     ; 0x00000060: 4E 05 
	pushq  rax                     ; 0x00000062: 4D 05 
	pushq  3.000000                ; 0x00000064: 8D 00 00 00 00 00 00 08 40 
	mulq                           ; 0x0000006D: 13 
	negq                           ; 0x0000006E: 15 
	popq   [24]                    ; 0x0000006F: AE 18 00 00 00 
	pushq  rax                     ; 0x00000074: 4D 05 
	pushq  2.000000                ; 0x00000076: 8D 00 00 00 00 00 00 00 40 
	mulq                           ; 0x0000007F: 13 
	popq   [32]                    ; 0x00000080: AE 20 00 00 00 
	push   0                       ; 0x00000085: 81 00 00 00 00 
	pop    rbx                     ; 0x0000008A: 42 06 
	jmp    L0                      ; 0x0000008C: 19 DF 01 00 00 

L8:

	push   0                       ; 0x00000091: 81 00 00 00 00 
	pop    rax                     ; 0x00000096: 42 05 
	jmp    L1                      ; 0x00000098: 19 CC 01 00 00 

L7:

	pushq  [32]                    ; 0x0000009D: AD 20 00 00 00 
	pushq  [24]                    ; 0x000000A2: AD 18 00 00 00 
	subq                           ; 0x000000A7: 12 
	pushq  rax                     ; 0x000000A8: 4D 05 
	mulq                           ; 0x000000AA: 13 
	pushq  scrx                    ; 0x000000AB: 4D 0B 
	divq                           ; 0x000000AD: 14 
	pushq  [24]                    ; 0x000000AE: AD 18 00 00 00 
	addq                           ; 0x000000B3: 11 
	popq   [40]                    ; 0x000000B4: AE 28 00 00 00 
	pushq  [8]                     ; 0x000000B9: AD 08 00 00 00 
	pushq  [16]                    ; 0x000000BE: AD 10 00 00 00 
	subq                           ; 0x000000C3: 12 
	pushq  rbx                     ; 0x000000C4: 4D 06 
	mulq                           ; 0x000000C6: 13 
	pushq  scry                    ; 0x000000C7: 4D 0C 
	divq                           ; 0x000000C9: 14 
	pushq  [16]                    ; 0x000000CA: AD 10 00 00 00 
	addq                           ; 0x000000CF: 11 
	popq   [48]                    ; 0x000000D0: AE 30 00 00 00 
	pushq  [40]                    ; 0x000000D5: AD 28 00 00 00 
	pushq  [40]                    ; 0x000000DA: AD 28 00 00 00 
	mulq                           ; 0x000000DF: 13 
	popq   [56]                    ; 0x000000E0: AE 38 00 00 00 
	pushq  [48]                    ; 0x000000E5: AD 30 00 00 00 
	pushq  [48]                    ; 0x000000EA: AD 30 00 00 00 
	mulq                           ; 0x000000EF: 13 
	popq   [64]                    ; 0x000000F0: AE 40 00 00 00 
	pushq  [40]                    ; 0x000000F5: AD 28 00 00 00 
	pushq  [48]                    ; 0x000000FA: AD 30 00 00 00 
	addq                           ; 0x000000FF: 11 
	popq   eax                     ; 0x00000100: 4E 01 
	pushq  eax                     ; 0x00000102: 4D 01 
	pushq  eax                     ; 0x00000104: 4D 01 
	mulq                           ; 0x00000106: 13 
	popq   [72]                    ; 0x00000107: AE 48 00 00 00 
	push   0                       ; 0x0000010C: 81 00 00 00 00 
	pop    rcx                     ; 0x00000111: 42 07 
	jmp    L2                      ; 0x00000113: 19 73 01 00 00 

L4:

	pushq  [56]                    ; 0x00000118: AD 38 00 00 00 
	pushq  [64]                    ; 0x0000011D: AD 40 00 00 00 
	subq                           ; 0x00000122: 12 
	pushq  [40]                    ; 0x00000123: AD 28 00 00 00 
	addq                           ; 0x00000128: 11 
	popq   eax                     ; 0x00000129: 4E 01 
	pushq  [72]                    ; 0x0000012B: AD 48 00 00 00 
	pushq  [56]                    ; 0x00000130: AD 38 00 00 00 
	subq                           ; 0x00000135: 12 
	pushq  [64]                    ; 0x00000136: AD 40 00 00 00 
	subq                           ; 0x0000013B: 12 
	pushq  [48]                    ; 0x0000013C: AD 30 00 00 00 
	addq                           ; 0x00000141: 11 
	popq   ebx                     ; 0x00000142: 4E 02 
	pushq  eax                     ; 0x00000144: 4D 01 
	pushq  eax                     ; 0x00000146: 4D 01 
	mulq                           ; 0x00000148: 13 
	popq   [56]                    ; 0x00000149: AE 38 00 00 00 
	pushq  ebx                     ; 0x0000014E: 4D 02 
	pushq  ebx                     ; 0x00000150: 4D 02 
	mulq                           ; 0x00000152: 13 
	popq   [64]                    ; 0x00000153: AE 40 00 00 00 
	pushq  eax                     ; 0x00000158: 4D 01 
	pushq  ebx                     ; 0x0000015A: 4D 02 
	addq                           ; 0x0000015C: 11 
	popq   eax                     ; 0x0000015D: 4E 01 
	pushq  eax                     ; 0x0000015F: 4D 01 
	pushq  eax                     ; 0x00000161: 4D 01 
	mulq                           ; 0x00000163: 13 
	popq   [72]                    ; 0x00000164: AE 48 00 00 00 
	push   1                       ; 0x00000169: 81 01 00 00 00 
	push   rcx                     ; 0x0000016E: 41 07 
	add                            ; 0x00000170: 05 
	pop    rcx                     ; 0x00000171: 42 07 

L2:

	push   [4]                     ; 0x00000173: A1 04 00 00 00 
	int2flt                        ; 0x00000178: 23 
	push   rcx                     ; 0x00000179: 41 07 
	int2flt                        ; 0x0000017B: 23 
	jae    L3                      ; 0x0000017C: 1D 97 01 00 00 
	pushq  [56]                    ; 0x00000181: AD 38 00 00 00 
	pushq  [64]                    ; 0x00000186: AD 40 00 00 00 
	addq                           ; 0x0000018B: 11 
	push   [0]                     ; 0x0000018C: A1 00 00 00 00 
	int2flt                        ; 0x00000191: 23 
	jae    L4                      ; 0x00000192: 1D 18 01 00 00 

L3:

	push   [4]                     ; 0x00000197: A1 04 00 00 00 
	int2flt                        ; 0x0000019C: 23 
	push   rcx                     ; 0x0000019D: 41 07 
	int2flt                        ; 0x0000019F: 23 
	jb     L5                      ; 0x000001A0: 1E B1 01 00 00 
	push   0                       ; 0x000001A5: 81 00 00 00 00 
	pop    [rbp]                   ; 0x000001AA: 62 09 
	jmp    L6                      ; 0x000001AC: 19 B8 01 00 00 

L5:

	push   16777215                ; 0x000001B1: 81 FF FF FF 00 
	pop    [rbp]                   ; 0x000001B6: 62 09 

L6:

	push   3                       ; 0x000001B8: 81 03 00 00 00 
	push   rbp                     ; 0x000001BD: 41 09 
	add                            ; 0x000001BF: 05 
	pop    rbp                     ; 0x000001C0: 42 09 
	push   1                       ; 0x000001C2: 81 01 00 00 00 
	push   rax                     ; 0x000001C7: 41 05 
	add                            ; 0x000001C9: 05 
	pop    rax                     ; 0x000001CA: 42 05 

L1:

	pushq  scrx                    ; 0x000001CC: 4D 0B 
	pushq  rax                     ; 0x000001CE: 4D 05 
	jb     L7                      ; 0x000001D0: 1E 9D 00 00 00 
	push   1                       ; 0x000001D5: 81 01 00 00 00 
	push   rbx                     ; 0x000001DA: 41 06 
	add                            ; 0x000001DC: 05 
	pop    rbx                     ; 0x000001DD: 42 06 

L0:

	pushq  scry                    ; 0x000001DF: 4D 0C 
	pushq  rbx                     ; 0x000001E1: 4D 06 
	jb     L8                      ; 0x000001E3: 1E 91 00 00 00 
	push   80                      ; 0x000001E8: 81 50 00 00 00 
	pop    rbp                     ; 0x000001ED: 42 09 
	screen rbp                     ; 0x000001EF: 24 09 
	hlt                            ; 0x000001F1: 00 
