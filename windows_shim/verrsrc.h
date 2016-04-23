// ===-- windows_shim/verrsrc.h --------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_VERRSRC_H
#define WINDOWS_SHIM_VERRSRC_H

/* ----- VS_VERSION.dwFileFlags ----- */
#ifndef _MAC
#define VS_FFI_SIGNATURE        0xFEEF04BDL
#else
#define VS_FFI_SIGNATURE        0xBD04EFFEL
#endif
#define VS_FFI_STRUCVERSION     0x00010000L
#define VS_FFI_FILEFLAGSMASK    0x0000003FL

/* ----- VS_VERSION.dwFileFlags ----- */
#define VS_FF_DEBUG             0x00000001L
#define VS_FF_PRERELEASE        0x00000002L
#define VS_FF_PATCHED           0x00000004L
#define VS_FF_PRIVATEBUILD      0x00000008L
#define VS_FF_INFOINFERRED      0x00000010L
#define VS_FF_SPECIALBUILD      0x00000020L

/* ----- VS_VERSION.dwFileOS ----- */
#define VOS_UNKNOWN             0x00000000L
#define VOS_DOS                 0x00010000L
#define VOS_OS216               0x00020000L
#define VOS_OS232               0x00030000L
#define VOS_NT                  0x00040000L
#define VOS_WINCE               0x00050000L

#define VOS__BASE               0x00000000L
#define VOS__WINDOWS16          0x00000001L
#define VOS__PM16               0x00000002L
#define VOS__PM32               0x00000003L
#define VOS__WINDOWS32          0x00000004L

#define VOS_DOS_WINDOWS16       0x00010001L
#define VOS_DOS_WINDOWS32       0x00010004L
#define VOS_OS216_PM16          0x00020002L
#define VOS_OS232_PM32          0x00030003L
#define VOS_NT_WINDOWS32        0x00040004L

/* ----- VS_VERSION.dwFileType ----- */
#define VFT_UNKNOWN             0x00000000L
#define VFT_APP                 0x00000001L
#define VFT_DLL                 0x00000002L
#define VFT_DRV                 0x00000003L
#define VFT_FONT                0x00000004L
#define VFT_VXD                 0x00000005L
#define VFT_STATIC_LIB          0x00000007L

typedef struct tagVS_FIXEDFILEINFO
{
    DWORD dwSignature;
    DWORD dwStrucVersion;
    /*DWORD*/ uint32_t dwFileVersionMS;
    DWORD dwFileVersionLS;
    DWORD dwProductVersionMS;
    DWORD dwProductVersionLS;
    DWORD dwFileFlagsMask;
    DWORD dwFileFlags;
    DWORD dwFileOS;
    DWORD dwFileType;
    DWORD dwFileSubtype;
    DWORD dwFileDateMS;
    DWORD dwFileDateLS;
} VS_FIXEDFILEINFO;

inline DWORD GetFileVersionInfoSizeA(LPCSTR, LPDWORD) { return 0; }
inline DWORD GetFileVersionInfoSizeW(LPCWSTR, LPDWORD) { return 0; }
#ifdef UNICODE
#   define GetFileVersionInfoSize GetFileVersionInfoSizeW
#else
#   define GetFileVersionInfoSize GetFileVersionInfoSizeA
#endif // !UNICODE

/* Read version info into buffer */
inline BOOL GetFileVersionInfoA(LPCSTR, DWORD, DWORD, LPVOID)  { return 0; }
inline BOOL GetFileVersionInfoW(LPCWSTR, DWORD, DWORD, LPVOID) { return 0; }
#ifdef UNICODE
#   define GetFileVersionInfo GetFileVersionInfoW
#else
#   define GetFileVersionInfo GetFileVersionInfoA
#endif // !UNICODE

inline BOOL VerQueryValueA(LPCVOID, LPCSTR, LPVOID *, PUINT)  { return 0; }
inline BOOL VerQueryValueW(LPCVOID, LPCWSTR, LPVOID *, PUINT) { return 0; }
#ifdef UNICODE
#   define VerQueryValue VerQueryValueW
#else
#   define VerQueryValue VerQueryValueA
#endif // !UNICODE

#endif // WINDOWS_SHIM_VERRSRC_H
