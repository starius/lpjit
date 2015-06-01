#define LPJIT_X64 (defined(_M_X64) || defined(__amd64__))
#define LPJIT_WIN (defined(_WIN32))

||#if (LPJIT_X64 != X64) || (LPJIT_WIN != WIN)
#error "Wrong DynASM flags used: pass `-D X64` and/or `-D WIN`"
#endif

#include "lpjit_dasm.h"
#include "lpjit_compiler.h"
#include "lpjit_match.h"
#include "dynasm/dasm_x86.h"

|.if X64
    |.arch x64
|.else
    |.arch x86
|.endif

#include <lua.h>
#include <lauxlib.h>

#include "lpjit.h"
#include "lpjit_lpeg.h"
#include "lpjit_compile.h"
#include "lpjit_memory.h"
#include "lpjit_matcher.h"

// stack:
// | push some_label
// | push captop
// | push scurrent

// | pop scurrent
// | pop captop
// | pop some_label

// additional labels
#define LABEL_GIVEUP Dst->pattern->codesize
#define LABELS_NUM 1

#define getoffset(p) (((p) + 1)->offset)

#define GETOFFSET getoffset(Dst->instruction)

#define CURRENT_I Dst->instruction

#define CURRENT_O lpjit_offsetOf(Dst, CURRENT_I)

#define POINTED_O CURRENT_O + GETOFFSET

#define POINTED_I lpjit_fromOffset(Dst, POINTED_O)

typedef void(*InstructionCompiler)(CompilerState*);

typedef struct IC_Reg {
    Opcode opcode;
    InstructionCompiler compiler;
} IC_Reg;

static void lpjit_asmDefines(CompilerState* Dst) {
    // only X64 Linux
    |.define scurrent, r12
    |.define send, r13
    |.define l, r8
    |.define m_state, r14
    |.define captop, r11
    |.define tmp1, rbx
    |.define top_capture, tmp1
    |.define tmp1D, ebx
    |.define tmp1B, bl
    |.define tmp2, rcx
    |.define tmp2B, cl
    |.define tmp3, rdx
    |.define tmp3B, dl
    |.define rArg1, rdi
    |.define rArg2, rsi
    |.define rArg3, rdx
    |.define rArg4, rcx
    |.define rArg5, r8
    |.define rArg6, r9
    //
    |.type mstate, MatchState, m_state
    |.type topcapture, Capture, top_capture
    //
    |.macro prepcall1, arg1
        | mov rArg1, arg1
    |.endmacro
    |.define postcall, .nop
    //
    |.macro prologue
        | push scurrent
        | push send
        | push m_state
        | push captop
        | push tmp1
        | push tmp2
        | push tmp3
        | push rax // Integer return values
        | mov l, rArg1
        | mov m_state, rArg2
        | mov scurrent, mstate->subject_current
        | mov send, mstate->subject_end
        | mov captop, 0
        | mov mstate->stack_pos, rsp
        | lea tmp1, [=>LABEL_GIVEUP]
        | push tmp1
        | push captop
        | push scurrent
    |.endmacro
    //
    |.macro epilogue
        | mov rsp, mstate->stack_pos
        // result
        | mov mstate->subject_current, scurrent
        // restore registers
        | pop rax
        | pop tmp3
        | pop tmp2
        | pop tmp1
        | pop captop
        | pop m_state
        | pop send
        | pop scurrent
        | ret
    |.endmacro
}

static void loadTopCapture(CompilerState* Dst) {
    | imul top_capture, captop, sizeof(Capture)
    | add top_capture, mstate->capture
}

static void loadPreTopCapture(CompilerState* Dst) {
    | imul top_capture, captop, sizeof(Capture)
    | add top_capture, mstate->capture
    | sub top_capture, sizeof(Capture)
}

static void IEnd_c(CompilerState* Dst) {
    loadTopCapture(Dst);
    | mov byte topcapture->kind, Cclose
    | mov aword topcapture->s, NULL
    | epilogue
}

static void IGiveup_c(CompilerState* Dst) {
    | mov scurrent, NULL
    | epilogue
}

static void IRet_c(CompilerState* Dst) {
    | pop tmp1
    | pop tmp1
    | pop tmp2 // label
    | jmp tmp2
}

static void putFail(CompilerState* Dst) {
    //| mov tmp1, captop
    |9:
    | pop scurrent
    | pop captop
    | pop tmp2 // label
    | cmp scurrent, NULL
    | je <9
    //| cmp ndyncap, 0
    //| jle >8
    // TODO call removedyncap
    //|8:
    | jmp tmp2
}

static void isSubjectOkEnd(CompilerState* Dst) {
    | cmp scurrent, send
}

static void jmpPointed(CompilerState* Dst) {
    | lea tmp1, [=>POINTED_O]
    | jmp tmp1
}

static void IAny_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jl >1
    putFail(Dst);
    |1:
    | inc scurrent
}

static void ITestAny_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jl >1
    jmpPointed(Dst);
    |1:
}

static void cmpCurrentByte(CompilerState* Dst) {
    | cmp byte [scurrent], Dst->instruction->i.aux
}

static void IChar_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jge >1
    cmpCurrentByte(Dst);
    | je >2
    |1:
    putFail(Dst);
    |2:
    | inc scurrent
}

static void ITestChar_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jge >1
    cmpCurrentByte(Dst);
    | je >2
    |1:
    jmpPointed(Dst);
    |2:
}

static void isInSet(CompilerState* Dst, int shift) {
    // see lpeg, macro testchar
    | mov tmp1, 0
    | mov tmp1B, [scurrent]
    | shr tmp1B, 3
    Instruction* next = Dst->instruction + shift;
    const char* st = next->buff;
    | mov tmp1B, [tmp1 + st]
    | mov tmp2B, [scurrent]
    | and tmp2B, 7
    | mov tmp3B, 1
    | shl tmp3B, tmp2B
    | and tmp1B, tmp3B
}

static void ISet_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jge >1
    isInSet(Dst, 1);
    | jnz >2
    |1:
    putFail(Dst);
    |2:
    | inc scurrent
}

static void ITestSet_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jge >1
    isInSet(Dst, 2);
    | jnz >2
    |1:
    jmpPointed(Dst);
    |2:
}

static void IBehind_c(CompilerState* Dst) {
    // n > s - o    => FAIL
    // s - o >= n   => not Fail
    int n = Dst->instruction->i.aux;
    | mov tmp1, scurrent
    | sub tmp1, mstate->subject_begin
    | cmp tmp1D, n
    | jge >1
    putFail(Dst);
    |1:
    | sub scurrent, n
}

static void ISpan_c(CompilerState* Dst) {
    |1:
    isSubjectOkEnd(Dst);
    | jge >2
    isInSet(Dst, 1);
    | jz >2
    | inc scurrent
    | jmp <1
    |2:
}

static void IJmp_c(CompilerState* Dst) {
    jmpPointed(Dst);
}

static void IChoice_c(CompilerState* Dst) {
    | lea tmp1, [=>POINTED_O]
    | push tmp1
    | push captop
    | push scurrent
}

static void ICall_c(CompilerState* Dst) {
    | lea tmp1, [>7]
    | push tmp1
    | push captop
    | mov tmp1, NULL
    | push tmp1 // scurrent
    jmpPointed(Dst);
    |7:
}

static void ICommit_c(CompilerState* Dst) {
    | pop tmp1
    | pop tmp1
    | pop tmp1
    jmpPointed(Dst);
}

static void IPartialCommit_c(CompilerState* Dst) {
    | pop tmp1 // scurrent
    | pop tmp1 // captop
    | push captop
    | push scurrent
    jmpPointed(Dst);
}

static void IBackCommit_c(CompilerState* Dst) {
    | pop scurrent
    | pop captop
    | pop tmp2 // label, ignored
    jmpPointed(Dst);
}

static void IFailTwice_c(CompilerState* Dst) {
    | pop tmp1
    | pop tmp1
    | pop tmp1
    putFail(Dst);
}

static void IFail_c(CompilerState* Dst) {
    putFail(Dst);
}

static void putDoubleCap(CompilerState* Dst) {
    | mov mstate->cap_top, captop
    | prepcall1 m_state
    | call &lpjit_doubleCap
    | postcall 1
}

static void checkCaptop(CompilerState* Dst) {
    | mov tmp3, mstate->capsize
    | cmp captop, tmp3
    | jl >6
    putDoubleCap(Dst);
    |6:
}

static void putPushCapture(CompilerState* Dst) {
    // loadTopCapture(Dst); // already loaded
    | mov word topcapture->idx, Dst->instruction->i.key
    | mov byte topcapture->kind, getkind(Dst->instruction)
    | inc captop
    checkCaptop(Dst);
}

static void ICloseCapture_c(CompilerState* Dst) {
    loadPreTopCapture(Dst);
    | cmp byte topcapture->siz, 0
    | jne >2
    | mov tmp3, scurrent
    | sub tmp3, topcapture->s
    | cmp tmp3, UCHAR_MAX
    | jge >2
    // then
    | inc tmp3
    | mov byte topcapture->siz, tmp3B
    | jmp >4
    // else
    |2:
    loadTopCapture(Dst);
    | mov byte topcapture->siz, 1
    | mov aword topcapture->s, scurrent
    putPushCapture(Dst);
    |4:
}

static void IOpenCapture_c(CompilerState* Dst) {
    loadTopCapture(Dst);
    | mov byte topcapture->siz, 0
    | mov aword topcapture->s, scurrent
    putPushCapture(Dst);
}

static void IFullCapture_c(CompilerState* Dst) {
    loadTopCapture(Dst);
    int cap_size = getoff(Dst->instruction);
    int siz1 = cap_size + 1;
    | mov byte topcapture->siz, siz1;
    | mov tmp2, scurrent
    | sub tmp2, cap_size
    | mov aword topcapture->s, tmp2
    putPushCapture(Dst);
}

static const IC_Reg INSTRUCTIONS[] = {
    {IEnd, IEnd_c},
    {IGiveup, IGiveup_c},
    {IRet, IRet_c},
    {IAny, IAny_c},
    {ITestAny, ITestAny_c},
    {IChar, IChar_c},
    {ITestChar, ITestChar_c},
    {ISet, ISet_c},
    {ITestSet, ITestSet_c},
    {IBehind, IBehind_c},
    {ISpan, ISpan_c},
    {IJmp,  IJmp_c},
    {IChoice, IChoice_c},
    {ICall, ICall_c},
    {ICommit, ICommit_c},
    {IPartialCommit, IPartialCommit_c},
    {IBackCommit, IBackCommit_c},
    {IFailTwice, IFailTwice_c},
    {IFail, IFail_c},
    // {ICloseRunTime, ICloseRunTime_c},
    {ICloseCapture, ICloseCapture_c},
    {IOpenCapture, IOpenCapture_c},
    {IFullCapture, IFullCapture_c},
    {0, NULL},
};

void lpjit_compileInstruction(CompilerState* Dst) {
    Opcode opcode = Dst->instruction->i.code;
    const IC_Reg* c;
    for (c = INSTRUCTIONS; c->compiler != NULL; ++c) {
        if (c->opcode == opcode) {
            | =>CURRENT_O:
            c->compiler(Dst);
            return;
        }
    }
    luaL_error(Dst->L, "Bad opcode %d", (int)opcode);
}

static void lpjit_compileAll(CompilerState* Dst) {
    int codesize = Dst->pattern->codesize;
    Instruction* begin = Dst->pattern->code;
    Instruction* end = begin + codesize;
    Dst->instruction = begin;
    while (Dst->instruction < end) {
        lpjit_compileInstruction(Dst);
        Dst->instruction += lpeg_sizei(Dst->instruction);
    }
    | =>LABEL_GIVEUP:
    IGiveup_c(Dst);
}

void lpjit_compile(lua_State* L,
                   Matcher* matcher,
                   Pattern* pattern) {
    luaL_argcheck(L, pattern->code, 1,
            "Pattern is not compiled");
    CompilerState cstate;
    CompilerState* Dst = &cstate;
    lpjit_compilerInit(Dst, L, pattern);
    |.section code
    dasm_init(Dst, DASM_MAXSECTION);
    // copy d to matcher. It will be freed in matcher's __gc
    matcher->d = cstate.d;
    |.globals lbl_
    void* labels[lbl__MAX];
    dasm_setupglobal(Dst, labels, lbl__MAX);
    |.actionlist lpjit_actions
    dasm_setup(Dst, lpjit_actions);
    // preserve labels for all instructions
    dasm_growpc(Dst, pattern->codesize + LABELS_NUM);
    lpjit_asmDefines(Dst);
    // TODO
    |.code
    |->lpjit_main:
    | prologue
    lpjit_compileAll(Dst);
    //
    dasm_link(Dst, &matcher->buffer_size);
    matcher->buffer = lpjit_allocate(matcher->buffer_size);
    dasm_encode(Dst, matcher->buffer);
    lpjit_protect(matcher->buffer, matcher->buffer_size);
    dasm_free(Dst);
    matcher->d = 0; // prevent double-free in matcher's __gc
    matcher->impl = labels[lbl_lpjit_main];
}
