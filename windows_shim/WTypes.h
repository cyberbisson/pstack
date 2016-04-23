// ===-- windows_shim/WTypes.h ---------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_WTYPES_H
#define WINDOWS_SHIM_WTYPES_H

#include <cstdint>

typedef long BOOL; // Why.
#define FALSE 0
#define TRUE 1

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
typedef DWORD ULONG;
typedef unsigned long long ULONGLONG;
typedef uint64_t DWORD64, *PDWORD64;
typedef DWORD64 ULONG64;
typedef long LONG;
typedef void *PVOID, *LPVOID;
typedef void const *PCVOID, *LPCVOID;

typedef LONG HRESULT;

typedef void *HANDLE;
typedef void *HLOCAL;
typedef void *HMODULE;
typedef HANDLE *PHANDLE;
#define INVALID_HANDLE_VALUE nullptr

typedef char CHAR;
typedef char *PSTR, *LPSTR;
typedef char const *PCSTR, *LPCSTR;
typedef short SHORT;
typedef unsigned long SIZE_T;
typedef wchar_t *LPWSTR;
typedef wchar_t const *LPCWSTR;

#ifdef UNICODE
#   define TEXT(s) L##s
typedef wchar_t TCHAR;
#else
#   define TEXT(s) s
typedef char TCHAR;
#endif
typedef TCHAR *LPTSTR;
typedef TCHAR const *LPCTSTR;

#endif // WINDOWS_SHIM_WTYPES_H
