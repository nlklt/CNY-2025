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
	L_str_16 db "; 6 / 3 = ", 0
	L_num_6 dd 6
	L_str_33 db "; 7 / 3 = ", 0
	L_num_7 dd 7
	L_str_49 db "; 5 / 3 * 8 = ", 0
	L_num_8 dd 8
	L_str_67 db "; (4 + 5) / 3 * 8 * (0 - 10) = ", 0
	L_num_4 dd 4
	L_num_10 dd 10
	L_num_0 dd 0
	L_str_105 db "ret_Lstring", 0
	L_str_109 db "+/- 0", 0
	L_str_122 db "ret_Vstring", 0
	L_str_128 db "privet", 0
	L_str_144 db "ret_Lint", 0
	L_num_200_n dd -200
	L_str_159 db "ret_Vint", 0
	L_num_222_n dd -222
	L_str_184 db "ret_Eint1", 0
	L_str_206 db "ret_Eint2", 0
	L_str_242 db "1", 0
	L_num_2 dd 2
	L_num_11 dd 11
	L_num_22 dd 22
	L_num_11_n dd -11
	L_num_22_n dd -22
	L_str_292 db "done", 0
	L_str_302 db "=== Std test ===", 0
	L_str_333 db "-----------------------", 0
	L_str_338 db "done", 0
	L_str_343 db "-----------------------", 0
	L_num_5 dd 5
	L_num_3 dd 3
	L_str_358 db "=== Arithmetic test ===", 0
	L_str_375 db "-----------------------", 0
	L_str_385 db "-----------------------", 0
	L_str_390 db "=== Unary test ===", 0
	L_str_413 db "a2 = a--;", 0
	L_str_429 db "a2 = --a;", 0
	L_str_444 db "b2 = ++b;", 0
	L_str_465 db "-----------------------", 0
	L_str_470 db "done", 0
	L_str_475 db "-----------------------", 0
	L_str_480 db "=== Functions test ===", 0
	L_str_489 db "asdf", 0
	L_str_491 db "0", 0
	L_str_496 db "----------------------", 0
	L_str_506 db "----------------------", 0
	L_str_511 db "=== For test ===", 0
	L_str_529 db "-", 0
	L_str_548 db "-", 0
	L_str_554 db "-----------------------", 0
	L_str_559 db "done", 0
	L_str_564 db "-----------------------", 0

.data
	a$arithmetic_test dd 0
	b$arithmetic_test dd 0
	d$arithmetic_test dd 0
	c$arithmetic_test dd 0
	s$ret_Vstring dd 0
	a$ret_Vstring dd 0
	a$ret_Lint dd 0
	n$ret_Vint dd 0
	a$ret_Eint1 dd 0
	b$ret_Eint1 dd 0
	a$ret_Eint2 dd 0
	b$ret_Eint2 dd 0
	a$functions_test dd 0
	b$functions_test dd 0
	s$functions_test dd 0
	n1$functions_test dd 0
	n2$functions_test dd 0
	date$main dd 0
	time$main dd 0
	a$main dd 0
	b$main dd 0
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
	push d$arithmetic_test
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

ret_Lstring$global PROC
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	push offset L_str_105
	call write_str
	add esp, 4
	push offset L_str_109
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Lstring$global ENDP

ret_Vstring$global PROC
	push ebp
	mov ebp, esp
	mov s$ret_Vstring, eax
	push ebx
	push esi
	push edi
	push offset L_str_122
	call write_str
	add esp, 4
	push offset L_str_128
	pop eax
	mov a$ret_Vstring, eax
	push a$ret_Vstring
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Vstring$global ENDP

ret_Lint$global PROC
	push ebp
	mov ebp, esp
	mov a$ret_Lint, eax
	push ebx
	push esi
	push edi
	push offset L_str_144
	call write_str
	add esp, 4
	push L_num_200_n
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Lint$global ENDP

ret_Vint$global PROC
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	push offset L_str_159
	call write_str
	add esp, 4
	push L_num_222_n
	pop eax
	mov n$ret_Vint, eax
	push n$ret_Vint
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Vint$global ENDP

ret_Eint1$global PROC
	push ebp
	mov ebp, esp
	mov a$ret_Eint1, eax
	mov b$ret_Eint1, ebx
	push ebx
	push esi
	push edi
	push offset L_str_184
	call write_str
	add esp, 4
	push a$ret_Eint1
	push b$ret_Eint1
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Eint1$global ENDP

ret_Eint2$global PROC
	push ebp
	mov ebp, esp
	mov a$ret_Eint2, eax
	mov b$ret_Eint2, ebx
	push ebx
	push esi
	push edi
	push offset L_str_206
	call write_str
	add esp, 4
	push a$ret_Eint2
	push b$ret_Eint2
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
ret_Eint2$global ENDP

functions_test$global PROC
	push ebp
	mov ebp, esp
	mov a$functions_test, eax
	mov b$functions_test, ebx
	mov s$functions_test, ecx
	push ebx
	push esi
	push edi
	call ret_Lstring$global
	push eax
	call write_str
	add esp, 4
	mov eax, offset L_str_242
	call ret_Vstring$global
	push eax
	call write_str
	add esp, 4
	mov eax, L_num_2
	call ret_Lint$global
	push eax
	call write_int
	add esp, 4
	call ret_Vint$global
	push eax
	call write_int
	add esp, 4
	mov eax, L_num_11
	mov ebx, L_num_22
	call ret_Eint1$global
	push eax
	call write_int
	add esp, 4
	push L_num_11_n
	pop eax
	mov n1$functions_test, eax
	push L_num_22_n
	pop eax
	mov n2$functions_test, eax
	mov eax, n1$functions_test
	mov ebx, n2$functions_test
	call ret_Eint2$global
	push eax
	call write_int
	add esp, 4
	push offset L_str_292
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
	push offset L_str_302
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
	push offset L_str_333
	call write_str
	add esp, 4
	push offset L_str_338
	call write_str
	add esp, 4
	push offset L_str_343
	call write_str
	add esp, 4
	push L_num_5
	pop eax
	mov a$main, eax
	push L_num_3
	pop eax
	mov b$main, eax
	push offset L_str_358
	call write_str
	add esp, 4
	mov eax, a$main
	mov ebx, b$main
	mov ecx, L_num_0
	call arithmetic_test$global
	push eax
	pop eax
	mov s$main, eax
	push offset L_str_375
	call write_str
	add esp, 4
	push s$main
	call write_str
	add esp, 4
	push offset L_str_385
	call write_str
	add esp, 4
	push offset L_str_390
	call write_str
	add esp, 4
	push L_num_5
	pop eax
	mov a$main, eax
	push L_num_3
	pop eax
	mov b$main, eax
	push a$main
	call write_int
	add esp, 4
	push b$main
	call write_int
	add esp, 4
	push offset L_str_413
	call write_str
	add esp, 4
	push a$main
	dec a$main
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_429
	call write_str
	add esp, 4
	dec a$main
	push a$main
	pop eax
	mov a2$main, eax
	push a2$main
	call write_int
	add esp, 4
	push offset L_str_444
	call write_str
	add esp, 4
	inc b$main
	push b$main
	pop eax
	mov b2$main, eax
	push b2$main
	call write_int
	add esp, 4
	pop eax
	not eax
	push eax
	pop eax
	mov b2$main, eax
	push offset L_str_465
	call write_str
	add esp, 4
	push offset L_str_470
	call write_str
	add esp, 4
	push offset L_str_475
	call write_str
	add esp, 4
	push offset L_str_480
	call write_str
	add esp, 4
	mov eax, a$main
	mov ebx, offset L_str_489
	mov ecx, offset L_str_491
	call functions_test$global
	push eax
	pop eax
	mov s$main, eax
	push offset L_str_496
	call write_str
	add esp, 4
	push s$main
	call write_str
	add esp, 4
	push offset L_str_506
	call write_str
	add esp, 4
	push offset L_str_511
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
	push offset L_str_529
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
	push offset L_str_548
	call write_str
	add esp, 4

; --- Cycle Step ---
	inc i$0
	jmp CYCLE_START_1
CYCLE_END_1:
	push offset L_str_554
	call write_str
	add esp, 4
	push offset L_str_559
	call write_str
	add esp, 4
	push offset L_str_564
	call write_str
	add esp, 4
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
