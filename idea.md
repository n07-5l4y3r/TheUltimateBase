```c++
typedef struct __sCmd {
    unsigned __int64 result;
    unsigned __int64 function;
    unsigned __int64 param2;
    unsigned __int64 param3;
    unsigned __int64 param4;
} sCmd;

unsigned __int64 call_gate(sCmd* pCmd)
{
    pCmd->result = reinterpret_cast<
    unsigned __int64(*)(
        unsigned __int64,
        unsigned __int64,
        unsigned __int64
    )>(pCmd->function)(
        pCmd->param2,
        pCmd->param3,
        pCmd->param4);
    return pCmd->result;
}
```


```asm
pCmd$ = 48
unsigned __int64 call_gate(__sCmd *) PROC           ; call_gate
$LN4:
        push    rbx
        sub     rsp, 32                             ; 00000020H
        mov     r8, QWORD PTR [rcx+32]
        mov     rbx, rcx
        mov     rdx, QWORD PTR [rcx+24]
        mov     rcx, QWORD PTR [rcx+16]
        call    QWORD PTR [rbx+8]
        mov     QWORD PTR [rbx], rax
        add     rsp, 32                             ; 00000020H
        pop     rbx
        ret     0
unsigned __int64 call_gate(__sCmd *) ENDP           ; call_gate
```