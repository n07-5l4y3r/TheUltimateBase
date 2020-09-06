```c++
typedef unsigned __int64 uint64_t;
typedef struct __sCmd {
    uint64_t case_number;
	uint64_t reserved;
	uint64_t qRet;
	uint64_t qFunction;
    uint64_t qParam2;
	uint64_t qParam3;
    uint64_t qParam4;
} sCmd;

unsigned __int64 call_gate(sCmd* pCmd)
{
    pCmd->qRet = reinterpret_cast<
    unsigned __int64(*)(
        unsigned __int64,
        unsigned __int64,
        unsigned __int64
    )>(pCmd->qFunction)(
        pCmd->qParam2,
        pCmd->qParam3,
        pCmd->qParam4);
    return 0ui64;
}
```


```asm
pCmd$ = 48
unsigned __int64 call_gate(__sCmd *) PROC           ; call_gate
$LN4:
        push    rbx
        sub     rsp, 32                             ; 00000020H
        mov     r8, QWORD PTR [rcx+48]
        mov     rbx, rcx
        mov     rdx, QWORD PTR [rcx+40]
        mov     rcx, QWORD PTR [rcx+32]
        call    QWORD PTR [rbx+24]
        mov     QWORD PTR [rbx+16], rax
        xor     eax, eax
        add     rsp, 32                             ; 00000020H
        pop     rbx
        ret     0
unsigned __int64 call_gate(__sCmd *) ENDP           ; call_gate
```


```asm
arg_18$ = 18h
arg_10$ = 10h
$call_gate_shellcode:
	mov		r8,						QWORD PTR [rdi+30h]			;mov  rcx, pCmd->qParam4
	mov		rdx,					QWORD PTR [rdi+28h]			;mov  rcx, pCmd->qParam3
	mov		rcx,					QWORD PTR [rdi+20h]			;mov  rcx, pCmd->qParam2
	call	QWORD PTR [rdi+18h]									;call pCmd->qFunction
	xor		rbx,					rbx							;mov  rbx, 0
	mov     QWORD PTR [rdi+10h],	rax							;mov  pCmd->qRet, rax
	mov		rax,					rbx							;mov  rax, 0
	mov     rdi, 					QWORD PTR [rsp+38h+arg_18]	;
	mov     rbx, 					QWORD PTR [rsp+38h+arg_10]	;
	add		rsp,					38h							;
	retn														;
```


```asm
    jmp		$call_gate_shellcode
```