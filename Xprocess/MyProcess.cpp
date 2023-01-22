#include "pch.h"
#include "MyProcess.h"

BOOL IsPrefix(WCHAR* Text, WCHAR* modstr) {
    WCHAR* s1 = Text, * s2 = modstr;
    int len = lstrlenW(modstr);
    char s[10];
    if (len == 0)return 1;

    for (int i = 0; i < len; i++) {
        if (tolower(*s1) != tolower(*s2))return 0;
        s1++;
        s2++;
    }
    return 1;
}

QWORD FromHexStr(CString str) {
    int len = str.GetLength();
    QWORD Val = 0;
    for (int i = 0; i < len; i++) {
        Val *= 16;
        if (str[i] <= L'9') {
            Val += str[i] - L'0';
        }
        else {
            Val += str[i] - L'A' + 10;
        }
    }
    return Val;
}