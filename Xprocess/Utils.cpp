#include "pch.h"

#include<Windows.h>
#include "Utils.h"

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
BOOL IsWow64(HANDLE hProcess) {
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
    if (fnIsWow64Process) {
        fnIsWow64Process(hProcess, &bIsWow64);
    }
    return bIsWow64;
}