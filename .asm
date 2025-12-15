.386
.model flat, stdcall		
includelib kernel32.lib	
includelib libucrt.lib	
includelib Library.lib	

ExitProcess PROTO: dword	
write_int PROTO C :sword
write_str PROTO C :ptr byte
get_time PROTO C	
get_date PROTO C
.stack 4096 

.const
	L_str_16 byte "; 6 / 3 = " , 0
	L_num_6 sdword 6
	L_str_33 byte "; 9 / 3 = " , 0
	L_num_9 sdword 9
	L_str_49 byte "; 7 / 3 = " , 0
	L_num_7 sdword 7
	L_str_65 byte "; 5 / 3 * 8 = " , 0
	L_num_8 sdword 8
	L_str_83 byte "; 5 / (3 * 8) = " , 0
	L_str_103 byte "; 4 + 5 / 3 * 8 = " , 0
	L_num_4 sdword 4
	L_str_117 byte "; (4 + 5) / 3 * 8 * (0 - 10) = " , 0
	L_num_10_n sdword -10
	L_str_144 byte "; 4 / 3 = " , 0
	L_str_159 byte "done" , 0
	L_str_170 byte "ret_Lstring" , 0
	L_str_174 byte "+/- 0" , 0
	L_str_187 byte "ret_Vstring" , 0
	L_str_193 byte "privet" , 0
	L_str_209 byte "ret_Lint" , 0
	L_num_200_n sdword -200
	L_str_224 byte "ret_Vint" , 0
	L_num_222_n sdword -222
	L_str_249 byte "ret_Eint1" , 0
	L_str_271 byte "ret_Eint2" , 0
	L_str_307 byte "1" , 0
	L_num_2 sdword 2
	L_num_11 sdword 11
	L_num_22 sdword 22
	L_num_11_n sdword -11
	L_num_22_n sdword -22
	L_str_355 byte "done" , 0
	L_str_365 byte "=== Std test ===" , 0
	L_str_396 byte "-----------------------" , 0
	L_str_401 byte "done" , 0
	L_str_406 byte "-----------------------" , 0
	L_str_411 byte "=== For test ===" , 0
	L_num_0 sdword 0
	L_num_5 sdword 5
	L_str_429 byte "-" , 0
	L_num_3 sdword 3
	L_str_448 byte "-" , 0
	L_str_454 byte "-----------------------" , 0
	L_str_459 byte "done" , 0
	L_str_464 byte "-----------------------" , 0
	L_str_469 byte "=== Unary test ===" , 0
	L_num_1 sdword 1
	L_str_556 byte "-----------------------" , 0
	L_str_561 byte "done" , 0
	L_str_566 byte "-----------------------" , 0
	L_str_571 byte "=== Arithmetic test ===" , 0
	L_str_588 byte "-----------------------" , 0
	L_str_598 byte "-----------------------" , 0
	L_str_603 byte "=== Functions test ===" , 0
	L_str_616 byte "asdf" , 0
	L_str_621 byte "----------------------" , 0
	L_str_631 byte "----------------------" , 0

.data
	arithmetic_test$c sdword 0
	ret_Vstring$a dword ?
	ret_Vint$n sdword 0
	main$date dword ?
	main$time dword ?
	0$i sdword 0
	1$j sdword 0
	main$a sdword 0
	main$b sdword 0
	main$s dword ?

.code
global$arithmetic_test PROC, arithmetic_test$d : sdword, arithmetic_test$b : sdword, arithmetic_test$a : sdword
	push ebp

	push ebx
	push esi
	push edi


push offset L_str_16
call write_str
add esp, 4
push L_num_6
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_33
call write_str
add esp, 4
push L_num_9
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_49
call write_str
add esp, 4
push L_num_7
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_65
call write_str
add esp, 4
push arithmetic_test$a
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
push L_num_8
pop ebx
pop eax
imul eax, ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_83
call write_str
add esp, 4
push arithmetic_test$a
push arithmetic_test$b
push L_num_8
pop ebx
pop eax
imul eax, ebx
push eax
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_103
call write_str
add esp, 4
push L_num_4
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_117
call write_str
add esp, 4
push L_num_4
push arithmetic_test$a
pop ebx
pop eax
add eax, ebx
push eax
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
push L_num_8
pop ebx
pop eax
imul eax, ebx
push eax
push arithmetic_test$d
push L_num_10_n
pop ebx
pop eax
imul eax, ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4

push offset L_str_144
call write_str
add esp, 4
push L_num_4
push arithmetic_test$c
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c

push arithmetic_test$c
call write_int
add esp, 4
	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, L_str_159
	ret 12
global$arithmetic_test ENDP;------------------------------


global$ret_Lstring PROC, print : dword
	push ebp

	push ebx
	push esi
	push edi

	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, L_str_174
	ret
global$ret_Lstring ENDP;------------------------------


global$ret_Vstring PROC, ret_Vstring$s : dword
	push ebp

	push ebx
	push esi
	push edi


push offset L_str_187
call write_str
add esp, 4
mov ret_Vstring$a, offset L_str_193
	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, ret_Vstring$a
	ret 4
global$ret_Vstring ENDP;------------------------------


global$ret_Lint PROC, ret_Lint$a : sdword
	push ebp

	push ebx
	push esi
	push edi


push offset L_str_209
call write_str
add esp, 4
	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, L_num_200_n
	ret 4
global$ret_Lint ENDP;------------------------------


global$ret_Vint PROC, print : dword
	push ebp

	push ebx
	push esi
	push edi

push L_num_222_n
pop ret_Vint$n

	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, ret_Vint$n
	ret
global$ret_Vint ENDP;------------------------------


global$ret_Eint1 PROC, ret_Eint1$b : sdword, ret_Eint1$a : sdword
	push ebp

	push ebx
	push esi
	push edi


push offset L_str_249
call write_str
add esp, 4
	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, ret_Eint1$a
	ret 8
global$ret_Eint1 ENDP;------------------------------


global$ret_Eint2 PROC, ret_Eint2$b : sdword, ret_Eint2$a : sdword
	push ebp

	push ebx
	push esi
	push edi


push offset L_str_271
call write_str
add esp, 4
