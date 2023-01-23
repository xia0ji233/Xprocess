// XModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "Xprocess.h"
#include "afxdialogex.h"
#include "XModuleDlg.h"
#include "MyProcess.h"
#include "DLLDumpDlg.h"
#include <TlHelp32.h>
#include <vector>
#include <algorithm>

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
    DDX_Control(pDX, IDC_COMBO1, SortItem);
}


BEGIN_MESSAGE_MAP(XModuleDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON2, &XModuleDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &XModuleDlg::OnBnClickedButton3)
    ON_EN_CHANGE(IDC_EDIT1, &XModuleDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// XModuleDlg 消息处理程序

typedef struct BBB{
    WCHAR Name[260];
    QWORD Base;
    QWORD Size;
    WCHAR PATH[260];
}ModuleItem;

int SortByName(ModuleItem a, ModuleItem b) {
    return StrCmpCW(a.Name, b.Name) < 0;
}
int SortByBase(ModuleItem a, ModuleItem b) {
    return a.Base < b.Base;
}
int SortBySize(ModuleItem a, ModuleItem b) {
    return a.Size < b.Size;
}
int SortByPATH(ModuleItem a, ModuleItem b) {
    return StrCmpCW(a.PATH, b.PATH) < 0;
}



void XModuleDlg::ListModule(WCHAR *Text,int sort) {
    ModuleList.DeleteAllItems();
    std::vector<ModuleItem>v;
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
                ModuleItem tmp;
                if (!IsPrefix(me.szModule, Text)) {
                    continue;
                }
                tmp.Base =(QWORD) me.modBaseAddr;
                tmp.Size = me.modBaseSize;
                StrCpyW(tmp.Name, me.szModule);
                StrCpyW(tmp.PATH, me.szExePath);
                v.push_back(tmp);
               

                //INDEX++;
            } while (Module32Next(ths, &me));

        }
        CloseHandle(ths);
        switch (sort) {
        case 0:
            std::sort(v.begin(), v.end(), SortByName);
            break;
        case 1:
            std::sort(v.begin(), v.end(), SortByBase);
            break;
        case 2:
            std::sort(v.begin(), v.end(), SortBySize);
            break;
        default:
            std::sort(v.begin(), v.end(), SortByPATH);
            break;
        }

        for (auto item : v) {
            wsprintf(Base, L"%p", item.Base);
            wsprintf(Size, L"%p", item.Size);
            ModuleList.InsertItem(INDEX, item.Name);
            ModuleList.SetItemText(INDEX, 1, Base);
            ModuleList.SetItemText(INDEX, 2, Size);
            ModuleList.SetItemText(INDEX, 3, item.PATH);
            INDEX++;
        }
    }
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
    ListModule(L"",0);
    SortItem.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int DumpOption=0x00000000;
WCHAR PATH[260];

void XModuleDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    if (REMEMBERME & ~DumpOption) {
        DLLDumpDlg* Dlg = new DLLDumpDlg(&DumpOption,PATH);
        Dlg->DoModal();
        if (DUMP & ~DumpOption) {
            return;
        }
    }

    CString ModuleName = GetModuleName();
    WCHAR FileName[260];
    const size_t newsizew = (ModuleName.GetLength() + 1) * 2;
    WCHAR* tmp = new wchar_t[newsizew];
    wcscpy_s(tmp, newsizew, ModuleName);
    wsprintf(FileName, L"%s%d-%s.dump",PATH,this->PID,tmp);//PID-MODULENAME.dump的命名

    FILE* fd = _wfopen(FileName, L"wb+");
    //MessageBoxExW(NULL, FileName, ModuleName, MB_OK, 0);
    QWORD Base = GetModuleBase();
    QWORD Size = GetModuleSize();
    BYTE buf[0x1000];
    BYTE ENTER = 10;
    SIZE_T ReadSize;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->PID);
    MEMORY_BASIC_INFORMATION MemInfo;
     char hex[5];
    for (QWORD i = Base; i < Base + Size; i+=0x1000) {
        VirtualQueryEx(hProcess, (LPCVOID)i, &MemInfo, sizeof(MemInfo));
        if (MemInfo.State != MEM_COMMIT) {
            continue;
        }
        if (MemInfo.Protect == PAGE_EXECUTE_READ && (DumpOption & EXECUTE)) {
            goto dump;
        }
        if (MemInfo.Protect == PAGE_READWRITE && (DumpOption & WRITE)) {
            goto dump;
        }
        if (MemInfo.Protect == PAGE_READONLY && (DumpOption & READONLY)) {
            goto dump;
        }
        continue; 
    dump:
        ReadProcessMemory(hProcess, (LPVOID)i, &buf, 0x1000, &ReadSize);
        if (HexDump.GetCheck()) {//十六进制转储
            for (QWORD j = 0; j < 0x1000; j++) {
                if (j % 16 == 0) {
                    fprintf(fd, "Offset:0x%p: ", i + j - Base);
                }
                sprintf(hex, "%02x ", buf[j]);
                fwrite(hex, 1, 3, fd);
                if (j % 16 == 15) {
                    fwrite(&ENTER, 1, 1, fd);
                }
            }
        }
        else {
            fwrite(buf, 1, 0x1000, fd);
        }
    }
    DumpOption &= ~DUMP;
    delete tmp;
    fclose(fd);
    WCHAR Message[350];
    wsprintf(Message, L"文件已转储到 %s 中", FileName);
    MessageBoxExW(NULL, Message, ModuleName, MB_OK, 0);
}


void XModuleDlg::OnEnChangeEdit1()
{
    // TODO:  在此添加控件通知处理程序代码
    CString s;
    GetDlgItemTextW(IDC_EDIT1, s);
    const size_t newsizew = (s.GetLength() + 1) * 2;
    WCHAR* n2stringw = new wchar_t[newsizew];
    wcscpy_s(n2stringw, newsizew, s);
    int index = SortItem.GetCurSel();
    ListModule(n2stringw, index);
}

void XModuleDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    OnEnChangeEdit1();
}



