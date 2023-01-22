// XModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "Xprocess.h"
#include "afxdialogex.h"
#include "XModuleDlg.h"
#include <TlHelp32.h>

// XModuleDlg 对话框

IMPLEMENT_DYNAMIC(XModuleDlg, CDialogEx)

XModuleDlg::XModuleDlg(int PID,CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{
	this->PID = PID;
}

XModuleDlg::~XModuleDlg()
{
}

void XModuleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST2, ModuleList);
    DDX_Control(pDX, IDC_CHECK1, HexDump);
}


BEGIN_MESSAGE_MAP(XModuleDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON2, &XModuleDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// XModuleDlg 消息处理程序



void XModuleDlg::InitList() {
    HANDLE ths = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->PID);
    WCHAR Base[260];
    WCHAR Size[260];
    ULONG INDEX = 0;
    if (ths != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 me;
        me.dwSize = sizeof(me);

        if (Module32First(ths, &me))
        {
            do
            {
                wsprintf(Base, L"%p", me.modBaseAddr);
                wsprintf(Size, L"%p", me.modBaseSize);
                ModuleList.InsertItem(INDEX, me.szModule);
                ModuleList.SetItemText(INDEX, 1, Base);
                ModuleList.SetItemText(INDEX, 2, Size);
                ModuleList.SetItemText(INDEX, 3, me.szExePath);
                INDEX++;
            } while (Module32Next(ths, &me));

        }
        CloseHandle(ths);
    }
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

QWORD XModuleDlg::GetModuleBase() {
    int i = ModuleList.GetSelectionMark();//获得选中行的行数
    CString s = ModuleList.GetItemText(i, 1);//(行，列)
    return FromHexStr(s);
}

QWORD XModuleDlg::GetModuleSize() {
    int i = ModuleList.GetSelectionMark();//获得选中行的行数
    CString s = ModuleList.GetItemText(i, 2);//(行，列)
    return FromHexStr(s);
}

CString XModuleDlg::GetModuleName() {
    int i = ModuleList.GetSelectionMark();//获得选中行的行数
    CString s = ModuleList.GetItemText(i, 0);//(行，列)
    return s;
}

BOOL XModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	ModuleList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ModuleList.InsertColumn(0, L"模块名称", 0, 130);
	ModuleList.InsertColumn(1, L"模块基址", 0, 130);
	ModuleList.InsertColumn(2, L"模块大小", 0, 130);
    ModuleList.InsertColumn(3, L"模块路径", 0, 330);
    InitList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void XModuleDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    CString ModuleName = GetModuleName();
    WCHAR FileName[260];
    const size_t newsizew = (ModuleName.GetLength() + 1) * 2;
    WCHAR* tmp = new wchar_t[newsizew];
    wcscpy_s(tmp, newsizew, ModuleName);
    wsprintf(FileName, L"C:\\%d-%s.dump",this->PID,tmp);//PID-MODULENAME.dump的命名
    FILE* fd = _wfopen(FileName, L"wb+");
    //MessageBoxExW(NULL, FileName, ModuleName, MB_OK, 0);
    QWORD Base = GetModuleBase();
    QWORD Size = GetModuleSize();
    BYTE buf[0x1000];
    BYTE ENTER = 10;
    SIZE_T ReadSize;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->PID);
    if (HexDump.GetCheck()) {//十六进制转储
        char hex[5];
        for (QWORD i = Base; i < Base + Size; i+=0x1000) {
            ReadProcessMemory(hProcess, (LPVOID)i, &buf, 0x1000, &ReadSize);
            for (QWORD j = 0; j < 0x1000; j++) {
                if (j % 16 == 0) {
                    fprintf(fd,"Offset:0x%p: ", i + j - Base);
                }
                sprintf(hex, "%02x ", buf[j]);
                fwrite(hex, 1, 3, fd);
                if (j % 16 == 15) {
                    fwrite(&ENTER, 1, 1, fd);
                }
            }
        }
    }
    else {
        for (QWORD i = Base; i < Base + Size; i+=0x1000) {
            ReadProcessMemory(hProcess, (LPVOID)i, buf, 0x1000, &ReadSize);
            fwrite(buf, 1, 0x1000, fd);
        }
    }
    delete tmp;
    fclose(fd);
    MessageBoxExW(NULL, L"转储完毕", ModuleName, MB_OK, 0);
}
