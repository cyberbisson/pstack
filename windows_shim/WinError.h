// ===-- windows_shim/WinError.h -------------------------- -*- C++ -*- --=== //

#ifndef WINDOWS_SHIM_WINERROR_H
#define WINDOWS_SHIM_WINERROR_H

#define ERROR_INVALID_ADDRESS            487L

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr)    (((HRESULT)(hr)) < 0)

#endif // WINDOWS_SHIM_WINERROR_H
