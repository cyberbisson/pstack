// ===-- windows_shim/WinBase.h --------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_WINBASE_H
#define WINDOWS_SHIM_WINBASE_H

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF

#define FILE_NAME_OPENED 0x8
#define EXCEPTION_NONCONTINUABLE 0x1
#define INFINITE 0xFFFFFFFF
#define VOLUME_NAME_DOS 0x0

#define EXCEPTION_ACCESS_VIOLATION          0xC0000005L
#define EXCEPTION_DATATYPE_MISALIGNMENT     0x80000002L
#define EXCEPTION_BREAKPOINT                0x80000003L
#define EXCEPTION_SINGLE_STEP               0x80000004L
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     0xC000008CL
#define EXCEPTION_FLT_DENORMAL_OPERAND      0xC000008DL
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        0xC000008EL
#define EXCEPTION_FLT_INEXACT_RESULT        0xC000008FL
#define EXCEPTION_FLT_INVALID_OPERATION     0xC0000090L
#define EXCEPTION_FLT_OVERFLOW              0xC0000091L
#define EXCEPTION_FLT_STACK_CHECK           0xC0000092L
#define EXCEPTION_FLT_UNDERFLOW             0xC0000093L
#define EXCEPTION_INT_DIVIDE_BY_ZERO        0xC0000094L
#define EXCEPTION_INT_OVERFLOW              0xC0000095L
#define EXCEPTION_PRIV_INSTRUCTION          0xC0000096L
#define EXCEPTION_IN_PAGE_ERROR             0xC0000006L
#define EXCEPTION_ILLEGAL_INSTRUCTION       0xC000001DL
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  0xC0000025L
#define EXCEPTION_STACK_OVERFLOW            0xC00000FDL
#define EXCEPTION_INVALID_DISPOSITION       0xC0000026L

typedef DWORD (*PTHREAD_START_ROUTINE)(LPVOID);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD fUnicode;
    WORD nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO, *LPRIP_INFO;


typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

inline BOOL AdjustTokenPrivileges(
    HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD)
{ return FALSE; }

inline BOOL CloseHandle(HANDLE) { return TRUE; }

inline BOOL ContinueDebugEvent(DWORD , DWORD, DWORD) { return TRUE; }

inline BOOL DebugActiveProcess(DWORD) { return TRUE; }

inline BOOL DebugActiveProcessStop(DWORD) { return TRUE; }

inline BOOL DebugSetProcessKillOnExit(BOOL) { return TRUE; }

inline HANDLE GetCurrentProcess() { return nullptr; }

inline DWORD GetFinalPathNameByHandleA(HANDLE, LPSTR, DWORD, DWORD) { return 0; }
inline DWORD GetFinalPathNameByHandleW(HANDLE, LPWSTR, DWORD, DWORD)
{ return 0; }
#ifdef UNICODE
#   define GetFinalPathNameByHandle  GetFinalPathNameByHandleW
#else
#   define GetFinalPathNameByHandle  GetFinalPathNameByHandleA
#endif // !UNICODE

inline DWORD GetLastError() { return 0; }

inline BOOL GetThreadContext(HANDLE, LPCONTEXT) { return TRUE; }

inline HLOCAL LocalFree(HLOCAL hMem) { return hMem; }

inline BOOL LookupPrivilegeValueA(LPCSTR, LPCSTR, PLUID) { return TRUE; }
inline BOOL LookupPrivilegeValueW(LPCWSTR, LPCWSTR, PLUID) { return TRUE; }
#ifdef UNICODE
#   define LookupPrivilegeValue  LookupPrivilegeValueW
#else
#   define LookupPrivilegeValue  LookupPrivilegeValueA
#endif // !UNICODE

inline BOOL OpenProcessToken(HANDLE, DWORD, PHANDLE) { return TRUE; }

inline BOOL WaitForDebugEvent(LPDEBUG_EVENT, DWORD) { return TRUE; }

#ifndef UNICODE
#   define CreateFile CreateFileA
#else
#   define CreateFile CreateFileW
#endif
inline HANDLE CreateFileA(
    LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
{ return nullptr; }
inline HANDLE CreateFileW(
    LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)
{ return nullptr; }

#ifdef UNICODE
#   define CreateFileMapping CreateFileMappingW
#else
#   define CreateFileMapping CreateFileMappingA
#endif
inline HANDLE CreateFileMappingA(
    HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR)
{ return nullptr; }
inline HANDLE CreateFileMappingW(
    HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCWSTR)
{ return nullptr; };

inline LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, SIZE_T)
{ return nullptr; }

inline BOOL UnmapViewOfFile(LPCVOID) { return TRUE; }

#ifdef UNICODE
#define LoadLibrary  LoadLibraryW
#else
#define LoadLibrary  LoadLibraryA
#endif
inline HMODULE LoadLibraryA(LPCSTR) { return nullptr; }
inline HMODULE LoadLibraryW(LPCWSTR) { return nullptr; }

#endif // WINDOWS_SHIM_WINBASE_H
