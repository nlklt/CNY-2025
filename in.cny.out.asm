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
	L_num_6 sdword 6
	L_str_20 byte "; 6 / 3 = " , 0
	L_num_9 sdword 9
	L_str_36 byte "; 9 / 3 = " , 0
	L_num_7 sdword 7
	L_str_52 byte "; 7 / 3 = " , 0
	L_num_8 sdword 8
	L_str_70 byte "; 5 / 3 * 8 = " , 0
	L_str_90 byte "; 5 / (3 * 8) = " , 0
	L_num_4 sdword 4
	L_str_110 byte "; 4 + 5 / 3 * 8 = " , 0
	L_str_132 byte "; (4 + 5) / 3 * 8 = " , 0
	L_str_148 byte "; 4 / 3 = " , 0
	L_num_0 sdword 0
	L_num_5 sdword 5
	L_num_3 sdword 3
	L_str_184 byte "; 6 / 3 = " , 0
	L_str_200 byte "; 9 / 3 = " , 0
	L_str_216 byte "; 7 / 3 = " , 0
	L_str_234 byte "; 5 / 3 * 8 = " , 0
	L_str_254 byte "; 5 / (3 * 8) = " , 0
	L_str_274 byte "; 4 + 5 / 3 * 8 = " , 0
	L_str_296 byte "; (4 + 5) / 3 * 8 = " , 0
	L_str_320 byte "; (4 + 5) / (3 * 8) = " , 0
	L_str_330 byte "\------------\" , 0
	L_str_344 byte "\------------\" , 0
	L_num_10 sdword 10

.data
	arithmetic_test$c sdword 0
	main$a sdword 0
	main$b sdword 0
	main$c sdword 0

.code
global$arithmetic_test PROC, arithmetic_test$b : sdword, arithmetic_test$a : sdword
	push ebp

	push ebx
	push esi
	push edi

push L_num_6
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c


push offset L_str_20
call write_str
add esp, 4
push arithmetic_test$c
call write_int
add esp, 4
push L_num_9
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c


push offset L_str_36
call write_str
add esp, 4
push arithmetic_test$c
call write_int
add esp, 4
push L_num_7
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c


push offset L_str_52
call write_str
add esp, 4
push arithmetic_test$c
call write_int
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


push offset L_str_70
call write_str
add esp, 4
push arithmetic_test$c
call write_int
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


push offset L_str_90
call write_str
add esp, 4
push arithmetic_test$c
call write_int
add esp, 4
push L_num_4
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
pop ebx
pop eax
add eax, ebx
push eax
pop arithmetic_test$c


push offset L_str_110
call write_str
add esp, 4
push arithmetic_test$c
call write_int
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
pop arithmetic_test$c


push offset L_str_132
call write_str
add esp, 4
push arithmetic_test$c
call write_int
add esp, 4
push L_num_4
push arithmetic_test$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop arithmetic_test$c


push offset L_str_148
call write_str
add esp, 4
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
	mov eax, L_num_0
	ret 8
global$arithmetic_test ENDP;------------------------------


;----------- MAIN ------------
main PROC

push L_num_5
pop main$a

push L_num_3
pop main$b

push L_num_6
push main$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop main$c


push offset L_str_184
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push L_num_9
push main$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop main$c


push offset L_str_200
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push L_num_7
push main$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop main$c


push offset L_str_216
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push main$a
push main$b
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
pop main$c


push offset L_str_234
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push main$a
push main$b
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
pop main$c


push offset L_str_254
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push L_num_4
push main$a
push main$b
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
pop ebx
pop eax
add eax, ebx
push eax
pop main$c


push offset L_str_274
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push L_num_4
push main$a
pop ebx
pop eax
add eax, ebx
push eax
push main$b
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
pop main$c


push offset L_str_296
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
push L_num_4
push main$a
pop ebx
pop eax
add eax, ebx
push eax
push main$b
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
pop main$c


push offset L_str_320
call write_str
add esp, 4
push main$c
call write_int
add esp, 4

push offset L_str_330
call write_str
add esp, 4
push main$a
push main$b
call global$arithmetic_test
push eax
pop main$c


push offset L_str_344
call write_str
add esp, 4
push main$c
call write_int
add esp, 4
	; --- восстановить регистры --- 
	pop edi
	pop esi
	pop ebx
	; -----------------------------
	mov esp, ebp
	pop ebp
	mov eax, L_num_10
	; main end (no ret/ENDP here)

push 0
call ExitProcess
main ENDP
end main