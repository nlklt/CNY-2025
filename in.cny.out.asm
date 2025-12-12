.586
.model flat, stdcall
.stack 4096
includelib kernel32.lib
includelib Library.lib

.const
	newline byte 13, 10, 0
	true_str byte 'true', 0
	false_str byte 'false', 0

	Lnum_0 sdword 1
	Lnum_1 sdword 2
	Lnum_2 sdword 2
	Lnum_3 sdword 3
	Lnum_4 sdword 4
	Lchar_5 dword 65
	Lstr_6 byte 'Global str', 0
	Lnum_7 sdword 83
	Lstr_8 byte 'Enter sum', 0
	Lchar_9 dword 89
	Lnum_10 sdword 10
	Lstr_11 byte 'Тест кириллицы', 0
	Lnum_12 sdword 2
	Lnum_13 sdword 1
	Lnum_14 sdword 1
	Lnum_15 sdword 3
	Lstr_16 byte 0
	Lchar_17 dword 48
	Lnum_18 sdword 0

.data
	switch_res sdword 0
	a_global_0 sdword 0
	b_global_2 sdword 0
	c_global_4 sdword 0
	d_global_6 sdword 0
	e_global_8 sdword 0
	G2_global_10 dword 0
	G3_global_12 dword ?
	OCt_global_14 sdword 0
	e_global_16 sdword 0
	a_sum_18 sdword 0
	b_sum_19 sdword 0
	c_sum_22 sdword 0
	x_get_char_24 sdword 0
	x_main_26 sdword 0
	S_main_28 dword ?
	LMin_main_30 sdword 0
	i__0_33 sdword 0
	x__0_36 sdword 0
	T_main_37 dword ?
	D_main_39 dword ?
	Sym_main_41 dword 0
	E_main_43 dword ?
	Zero_main_45 dword 0


extrn ExitProcess:proc
extrn write_int:proc
extrn write_str:proc


.code
sum:
	push offset Lstr_8
	call write_str
	add esp, 4
	push offset newline
	call write_str
	add esp, 4
	push 0
	pop eax
	push a_sum_18
	push b_sum_19
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov c_sum_22, eax
	push c_sum_22
	pop eax
	ret
get_char:
	push Lchar_9
	pop eax
	ret
main:
	push Lnum_10
	pop eax
	mov x_main_26, eax
	push offset Lstr_11
	pop eax
	mov S_main_28, eax
	push x_main_26
	pop eax
	mov LMin_main_30, eax
	push x_main_26
	push a_global_0
	call sum
	push eax
	push Lnum_12
	pop ebx
	pop eax
	imul ebx
	push eax
	push Lnum_13
	pop ebx
	pop eax
	test ebx, ebx
	jnz DIV_OK_0
	push 0
	jmp DIV_END_0
DIV_OK_0:
	cdq
	idiv ebx
DIV_END_0:
	push eax
	pop eax
	mov x_main_26, eax
	push 0
	call ExitProcess
end
