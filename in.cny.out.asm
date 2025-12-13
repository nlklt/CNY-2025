.586
.model flat, stdcall		
includelib kernel32.lib	
includelib libucrt.lib	
includelib Library.lib	

ExitProcess PROTO: dword	
EXTRN write_int: proc
EXTRN write_str: proc
.stack 4096 
.const
	L_str_20 byte "; 5 + 3 = " , 0
	L_str_37 byte "; 5 - 3 = " , 0
	L_str_54 byte "; 5 * 3 = " , 0
	L_str_71 byte "; 5 / 3 = " , 0
	L_num_8 sdword 8
	L_str_90 byte "; 5 / 3 * 8 = " , 0
	L_str_111 byte "; 5 / (3 * 8) = " , 0
	L_num_4 sdword 4
	L_str_132 byte "; 4 + 5 / 3 * 8 = " , 0
	L_str_155 byte "; (4 + 5) / 3 * 8 = " , 0
	L_str_180 byte "; (4 + 5) / (3 * 8) = " , 0
	L_num_0 sdword 0
	L_num_4_n sdword -4
	L_num_0o12_n sdword -10
	L_str_210 byte "פûגא" , 0
	L_str_215 byte 0, 0
	L_num_12 sdword 12
	L_num_0o12 sdword 10
	L_str_235 byte "s" , 0
	L_str_240 byte "abcd" , 0
	L_num_5 sdword 5
	L_num_3 sdword 3
	L_num_10 sdword 10
.data
	global$c sdword 0
	global$a_int sdword 0
	global$b_int sdword 0
	global$e_int sdword 0
	global$c_string dword ?
	global$d_string dword ?
	global$c_int sdword 0
	global$d_int sdword 0
	global$a_string dword ?
	global$b_string dword ?
	global$res sdword 0
.code
;----------- MAIN ------------
main PROC

global$arithmetic_test PROC,	global$a : sdword, global$b : sdword, 

push global$a
push global$b
pop ebx
pop eax
add eax, ebx
push eax
pop global$c


push offset L_str_20
call write_str
push global$c
call write_int
push global$a
push global$b
pop ebx
pop eax
sub eax, ebx
push eax
pop global$c


push offset L_str_37
call write_str
push global$c
call write_int
push global$a
push global$b
pop ebx
pop eax
imul eax, ebx
push eax
pop global$c


push offset L_str_54
call write_str
push global$c
call write_int
push global$a
push global$b
pop ebx
pop eax
cdq
idiv ebx
push eax
pop global$c


push offset L_str_71
call write_str
push global$c
call write_int
push global$a
push global$b
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
pop global$c


push offset L_str_90
call write_str
push global$c
call write_int
