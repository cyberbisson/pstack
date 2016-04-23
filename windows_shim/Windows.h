// ===-- windows_shim/Windows.h --------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_WINDOWS_H
#define WINDOWS_SHIM_WINDOWS_H

#include "BaseTsd.h"
#include "WTypes.h"
#include "WinNT.h"
#include "WinBase.h"
#include "Error.h"
#include "WinError.h"
#include "verrsrc.h"

// MinWinDef.h
typedef int (*FARPROC)();

// LibLoaderAPI.h
inline BOOL FreeLibrary(HMODULE) { return TRUE; }
inline FARPROC GetProcAddress(HMODULE, LPCSTR) { return nullptr; }

// WinUser.h
#define SLE_ERROR       0x00000001
#define SLE_MINORERROR  0x00000002
#define SLE_WARNING     0x00000003

#endif // WINDOWS_SHIM_WINDOWS_H
