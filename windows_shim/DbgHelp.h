// ===-- windows_shim/DbgHelp.h --------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_DBGHELP_H
#define WINDOWS_SHIM_DBGHELP_H

#define MAX_SYM_NAME 2000

#define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS    0x00000800
#define SYMOPT_DEBUG                     0x80000000
#define SYMOPT_DEFERRED_LOADS            0x00000004
#define SYMOPT_FAIL_CRITICAL_ERRORS      0x00000200
#define SYMOPT_INCLUDE_32BIT_MODULES     0x00002000
#define SYMOPT_LOAD_LINES                0x00000010
#define SYMOPT_NO_PROMPTS                0x00080000

#define UNDNAME_COMPLETE 0

typedef struct _SYMBOL_INFO {
    ULONG SizeOfStruct;
    ULONG TypeIndex;
    ULONG64 Reserved[2];
    ULONG Index;
    ULONG Size;
    ULONG64 ModBase;
    ULONG Flags;
    ULONG64 Value;
    ULONG64 Address;
    ULONG Register;
    ULONG Scope;
    ULONG Tag;
    ULONG NameLen;
    ULONG MaxNameLen;
    CHAR Name[1];
} SYMBOL_INFO, *PSYMBOL_INFO;

typedef struct _KDHELP64 {
    DWORD64 Thread;
    DWORD ThCallbackStack;
    DWORD ThCallbackBStore;
    DWORD NextCallback;
    DWORD FramePointer;
    DWORD64 KiCallUserMode;
    DWORD64 KeUserCallbackDispatcher;
    DWORD64 SystemRangeStart;
    DWORD64 KiUserExceptionDispatcher;
    DWORD64 StackBase;
    DWORD64 StackLimit;
    DWORD64 Reserved[5];
} KDHELP64, *PKDHELP64;

typedef enum {
    AddrMode1616,
    AddrMode1632,
    AddrModeReal,
    AddrModeFlat
} ADDRESS_MODE;

typedef struct _tagADDRESS64 {
    DWORD64       Offset;
    WORD          Segment;
    ADDRESS_MODE  Mode;
} ADDRESS64, *LPADDRESS64;

typedef struct _tagSTACKFRAME64 {
    ADDRESS64   AddrPC;               // program counter
    ADDRESS64   AddrReturn;           // return address
    ADDRESS64   AddrFrame;            // frame pointer
    ADDRESS64   AddrStack;            // stack pointer
    ADDRESS64   AddrBStore;           // backing store pointer
    PVOID       FuncTableEntry;       // pointer to pdata/fpo or NULL
    DWORD64     Params[4];            // possible arguments to the function
    BOOL        Far;                  // WOW far call
    BOOL        Virtual;              // is this a virtual frame?
    DWORD64     Reserved[3];
    KDHELP64    KdHelp;
} STACKFRAME64, *LPSTACKFRAME64;


typedef BOOL (*PREAD_PROCESS_MEMORY_ROUTINE64)(
    HANDLE, DWORD64, PVOID, DWORD, LPDWORD);
typedef PVOID (*PFUNCTION_TABLE_ACCESS_ROUTINE64)(HANDLE, DWORD64);
typedef DWORD64 (*PGET_MODULE_BASE_ROUTINE64)(HANDLE, DWORD64);
typedef DWORD64 (*PTRANSLATE_ADDRESS_ROUTINE64)(HANDLE, HANDLE, LPADDRESS64);

inline BOOL StackWalk64(
    DWORD,
    HANDLE,
    HANDLE,
    LPSTACKFRAME64,
    PVOID,
    PREAD_PROCESS_MEMORY_ROUTINE64,
    PFUNCTION_TABLE_ACCESS_ROUTINE64,
    PGET_MODULE_BASE_ROUTINE64,
    PTRANSLATE_ADDRESS_ROUTINE64)
{ return FALSE; }

inline BOOL SymCleanup(HANDLE) { return TRUE; }

inline BOOL SymFromAddr(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO) { return TRUE; }

inline PVOID SymFunctionTableAccess64(HANDLE, DWORD64) { return nullptr; }

inline DWORD64 SymGetModuleBase64(HANDLE, DWORD64) { return 0; }

inline DWORD SymGetOptions() { return 0; }

inline BOOL SymInitialize(HANDLE, PCSTR, BOOL) { return TRUE; }

inline DWORD SymSetOptions(DWORD) { return 0; }

inline DWORD UnDecorateSymbolName(PCSTR, PSTR, DWORD, DWORD) { return 0; }

#endif // WINDOWS_SHIM_DBGHELP_H
