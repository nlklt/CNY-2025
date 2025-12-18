.586
.model flat, C
includelib kernel32.lib
includelib libucrt.lib
includelib Library.lib

extern write_int: PROC
extern write_str: PROC
extern get_time : PROC
extern get_date : PROC
extern _imp__ExitProcess@4: PROC

.stack 4096

.const
	L_str_16 db "6 / 3 = ", 0
	L_num_6 dd 6
	L_str_33 db "7 / 3 = ", 0
	L_num_7 dd 7
	L_str_49 db "5 / 3 * 8 = ", 0
	L_num_8 dd 8
	L_str_67 db "(4 + 5) / 3 * 8 * (3 - 10) = ", 0
	L_num_4 dd 4
	L_num_10 dd 10
	L_num_0 dd 0
	L_str_107 db "ret_Pstring", 0
	L_str_124 db "ret_OVint", 0
	L_str_144 db "ret_Eint", 0
	L_num_0o01 dd 1
	L_num_2_n dd -2
	L_num_3_n dd -3
	L_num_2 dd 2
	L_num_1 dd 1
	L_str_248 db "=== Std test ===", 0
	L_str_279 db "=== Variables test ===", 0
	L_num_0o3 dd 3
	L_num_5 dd 5
	L_str_304 db "=== Arithmetic test ===", 0
	L_str_321 db "=== Unary test ===", 0
	L_str_326 db "a2 = a--;", 0
	L_str_342 db "a2 = --a;", 0
	L_str_357 db "a2 = a++ * 2;", 0
	L_str_374 db "a2 = ++a * 2;", 0
	L_str_391 db "b2 = ++b;", 0
	L_str_417 db "=== Functions test ===", 0
	L_str_426 db "asdf", 0
	L_str_428 db "string", 0
	L_str_433 db "=== For test ===", 0
	L_str_451 db "-", 0
	L_num_3 dd 3
	L_str_470 db "-", 0

.data
	a$arithmetic_test dd 0
	b$arithmetic_test dd 0
	d$arithmetic_test dd 0
	c$arithmetic_test dd 0
	s$ret_Pstring dd 0
	o$ret_OVint dd 0
	a$ret_Eint dd 0
	b$ret_Eint dd 0
	a$functions_test dd 0
	b$functions_test dd 0
	s$functions_test dd 0
	oldps$functions_test dd 0
	n1$functions_test dd 0
	n2$functions_test dd 0
	mul$functions_test dd 0
	date$main dd 0
	time$main dd 0
	b$main dd 0
	a$main dd 0
	s$main dd 0
	a2$main dd 0
	b2$main dd 0
	i$0 dd 0
	j$1 dd 0

.code

arithmetic_test$global PROC
	push ebp
	mov ebp, esp
	mov a$arithmetic_test, eax
	mov b$arithmetic_test, ebx
	mov d$arithmetic_test, ecx
	push ebx
	push esi
	push edi
	push offset L_str_16
	call write_str
	add esp, 4
	push L_num_6
	push b$arithmetic_test
	pop ebx
	pop eax
	cdq
	idiv ebx
	push eax
	pop eax
	mov c$arithmetic_test, eax
	push c$arithmetic_test
	call write_int
	add esp, 4
	push offset L_str_33
	call write_str
	add esp, 4
	push L_num_7
	push b$arithmetic_test
	pop ebx
	pop eax
	cdq
	idiv ebx
	push eax
	pop eax
	mov c$arithmetic_test, eax
	push c$arithmetic_test
	call write_int
	add esp, 4
	push offset L_str_49
	call write_str
	add esp, 4
	push a$arithmetic_test
	push b$arithmetic_test
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
	pop eax
	mov c$arithmetic_test, eax
	push c$arithmetic_test
	call write_int
	add esp, 4
	push offset L_str_67
	call write_str
	add esp, 4
	push L_num_4
	push a$arithmetic_test
	pop ebx
	pop eax
	add eax, ebx
	push eax
	push b$arithmetic_test
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
	push b$arithmetic_test
	push L_num_10
	pop ebx
	pop eax
	sub eax, ebx
	push eax
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov c$arithmetic_test, eax
	push c$arithmetic_test
	call write_int
	add esp, 4
	push L_num_0
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
arithmetic_test$global ENDP

ret_Pstring$global PROC
	push ebp
	mov ebp, esp
	mov s$ret_Pstring, eax
	push ebx
	push esi
	push edi
	push offset L_str_107
	call write_str
	add esp, 4
	push s$ret_Pstring
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Pstring$global ENDP

ret_OVint$global PROC
	push ebp
	mov ebp, esp
	mov o$ret_OVint, eax
	push ebx
	push esi
	push edi
	push offset L_str_124
	call write_str
	add esp, 4
	push o$ret_OVint
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_OVint$global ENDP

ret_Eint$global PROC
	push ebp
	mov ebp, esp
	mov a$ret_Eint, eax
	mov b$ret_Eint, ebx
	push ebx
	push esi
	push edi
	push offset L_str_144
	call write_str
	add esp, 4
	push a$ret_Eint
	push b$ret_Eint
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Eint$global ENDP

functions_test$global PROC
	push ebp
	mov ebp, esp
	mov a$functions_test, eax
	mov b$functions_test, ebx
	mov s$functions_test, ecx
	push ebx
	push esi
	push edi
	mov eax, b$functions_test
	call ret_Pstring$global
	push eax
	call write_str
	add esp, 4
	mov eax, s$functions_test
	call ret_Pstring$global
	push eax
	pop eax
	mov oldps$functions_test, eax
	push oldps$functions_test
	call write_str
	add esp, 4
	mov eax, L_num_0o01
	call ret_OVint$global
	push eax
	call write_int
	add esp, 4
	push L_num_2_n
	pop eax
	mov n1$functions_test, eax
	push L_num_3_n
	pop eax
	mov n2$functions_test, eax
	mov eax, n1$functions_test
	mov ebx, n2$functions_test
	call ret_Eint$global
	push eax
	call write_int
	add esp, 4
	push L_num_2
	pop eax
	mov mul$functions_test, eax
	mov eax, n1$functions_test
	mov ebx, n2$functions_test
	call ret_Eint$global
	push eax
	push mul$functions_test
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov mul$functions_test, eax
	push mul$functions_test
	call write_int
	add esp, 4
	push L_num_1
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
functions_test$global ENDP

main PROC
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	push offset L_str_248
	call write_str
	add esp, 4
	call get_date
	push eax
	pop eax
	mov date$main, eax
	push date$main
	call write_str
	add esp, 4
	call get_time
	push eax
	pop eax
	mov time$main, eax
	push time$main
	call write_str
	add esp, 4
	push offset L_str_279
	call write_str
	add esp, 4
	push L_num_0o3
	pop eax
	mov b$main, eax
	push L_num_5
	pop eax
	mov a$main, eax
	push a$main
	call write_int
	add esp, 4
	push b$main
	call write_int
	add esp, 4
	push offset L_str_304
	call write_str
	add esp, 4
	mov eax, a$main
	mov ebx, b$main
	mov ecx, L_num_0
	call arithmetic_test$global
	push eax
	pop eax
	mov s$main, eax
	push offset L_str_321
	call write_str
	add esp, 4
	push offset L_str_326
	call write_str
	add esp, 4
	push a$main
	dec a$main
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_342
	call write_str
	add esp, 4
	dec a$main
	push a$main
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_357
	call write_str
	add esp, 4
	push a$main
	inc a$main
	push L_num_2
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_374
	call write_str
	add esp, 4
	inc a$main
	push a$main
	push L_num_2
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_391
	call write_str
	add esp, 4
	inc b$main
	push b$main
	pop eax
	mov b2$main, eax
	push b2$main
	call write_int
	add esp, 4
	push L_num_2
	pop eax
	not eax
	push eax
	pop eax
	mov b2$main, eax
	push b2$main
	call write_int
	add esp, 4
	push offset L_str_417
	call write_str
	add esp, 4
	mov eax, a$main
	mov ebx, offset L_str_426
	mov ecx, offset L_str_428
	call functions_test$global
	push eax
	pop eax
	mov s$main, eax
	push offset L_str_433
	call write_str
	add esp, 4

; --- Cycle Init ---
	mov eax, 0
	mov i$0, eax
CYCLE_START_1:
	mov eax, i$0
	cmp eax, 5
	jge CYCLE_END_1
	push i$0
	call write_int
	add esp, 4
	push offset L_str_451
	call write_str
	add esp, 4

; --- Cycle Init ---
	mov eax, 0
	mov j$1, eax
CYCLE_START_2:
	mov eax, j$1
	cmp eax, 3
	jge CYCLE_END_2
	push j$1
	call write_int
	add esp, 4

; --- Cycle Step ---
	inc j$1
	jmp CYCLE_START_2
CYCLE_END_2:
	push offset L_str_470
	call write_str
	add esp, 4

; --- Cycle Step ---
	inc i$0
	jmp CYCLE_START_1
CYCLE_END_1:
	push L_num_0
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	push eax
	call dword ptr [_imp__ExitProcess@4]
main ENDP

end main
