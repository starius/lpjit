#define LPJIT_X64 (defined(_M_X64) || defined(__amd64__))
#define LPJIT_WIN (defined(_WIN32))

||#if (LPJIT_X64 != X64) || (LPJIT_WIN != WIN)
#error "Wrong DynASM flags used: pass `-D X64` and/or `-D WIN`"
#endif

#include "lpjit_dasm.h"
#include "lpjit_compiler.h"
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

// additional labels
//#define LABEL_FAIL Dst->codesize
#define LABELS_NUM 1

#define getoffset(p) (((p) + 1)->offset)

#define CURRENT_I Dst->instruction

#define CURRENT_O lpjit_offsetOf(Dst, CURRENT_I)

typedef void(*InstructionCompiler)(CompilerState*);

typedef struct IC_Reg {
    Opcode opcode;
    InstructionCompiler compiler;
} IC_Reg;

static void lpjit_asmDefines(CompilerState* Dst) {
    // only X64 Linux
    |.define subject_begin, rbx
    |.define subject_current, r12
    |.define subject_end, r13
    |.define m_state, r14
    |.define tmp1, rbx
    |.define tmp2, rcx
    |.define tmp3, rdx
    |.define rArg1, rdi
    |.define rArg2, rsi
    //
    |.type mstate, MatchState, m_state
    //
    |.macro prologue
        | push subject_begin
        | push subject_current
        | push subject_end
        | push m_state
        | push tmp1
        | push tmp2
        | push tmp3
        | push rax // Integer return values
        | mov m_state, rArg1
        | lea subject_begin, [mstate->subject_begin]
        | lea subject_current, [mstate->subject_current]
        | lea subject_end, [mstate->subject_end]
    |.endmacro
    //
    |.macro epilogue
        // result
        | mov [mstate->subject_current], subject_current
        // restore registers
        | pop rax
        | pop tmp3
        | pop tmp2
        | pop tmp1
        | pop m_state
        | pop subject_end
        | pop subject_current
        | pop subject_begin
        | ret
    |.endmacro
}

static void IEnd_c(CompilerState* Dst) {
    // TODO
    //| epilogue
}

static void putFail(CompilerState* Dst) {
    // TODO
    | mov subject_current, 0
    //| epilogue
}

static void isSubjectOkEnd(CompilerState* Dst) {
    | cmp subject_current, subject_end
}

static void IAny_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jl >1
    putFail(Dst);
    |1:
    | inc subject_current
}

static void cmpCurrentByte(CompilerState* Dst) {
    | cmp byte [subject_current], Dst->instruction->i.aux
}

static void IChar_c(CompilerState* Dst) {
    isSubjectOkEnd(Dst);
    | jge >1
    cmpCurrentByte(Dst);
    | je >2
    |1:
    putFail(Dst);
    |2:
    | inc subject_current
}

static const IC_Reg INSTRUCTIONS[] = {
    {IEnd, IEnd_c},
    // {IGiveup, IGiveup_c},
    // {IRet, IRet_c},
    {IAny, IAny_c},
    // {ITestAny, ITestAny_c},
    {IChar, IChar_c},
    // {ITestChar, ITestChar_c},
    // {ISet, ISet_c},
    // {ITestSet, ITestSet_c},
    // {IBehind, IBehind_c},
    // {ISpan, ISpan_c},
    // {IJmp,  IJmp_c},
    // {IChoice, IChoice_c},
    // {ICall, ICall_c},
    // {ICommit, ICommit_c},
    // {IPartialCommit, IPartialCommit_c},
    // {IBackCommit, IBackCommit_c},
    // {IFailTwice, IFailTwice_c},
    // {IFail, IFail_c},
    // {ICloseRunTime, ICloseRunTime_c},
    // {ICloseCapture, ICloseCapture_c},
    // {IOpenCapture, IOpenCapture_c},
    // {IFullCapture, IFullCapture_c},
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

// function from lpeg (lpcode.c)
static int lpeg_sizei(const Instruction *i) {
    switch ((Opcode)i->i.code) {
        case ISet: case ISpan:
            return CHARSETINSTSIZE;
        case ITestSet:
            return CHARSETINSTSIZE + 1;
        case ITestChar: case ITestAny:
        case IChoice: case IJmp:
        case ICall: case IOpenCall:
        case ICommit: case IPartialCommit:
        case IBackCommit:
            return 2;
        default:
            return 1;
    }
}

static void lpjit_compileAll(CompilerState* Dst) {
    int codesize = Dst->pattern->codesize;
    Instruction* begin = Dst->pattern->code;;
    Instruction* end = begin + codesize;
    Dst->instruction = begin;
    while (Dst->instruction < end) {
        lpjit_compileInstruction(Dst);
        Dst->instruction += lpeg_sizei(Dst->instruction);
    }
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
    //| prologue
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