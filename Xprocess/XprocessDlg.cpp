
// XprocessDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Xprocess.h"
#include "XprocessDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include <Windows.h>
#include <process.h>
#include <Psapi.h>
#include <vector>
#include <algorithm>
#include "DLLInject.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CXprocessDlg 对话框



CXprocessDlg::CXprocessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_XPROCESS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXprocessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST4, ProcessList);
	DDX_Control(pDX, IDC_EDIT1, SearchText);
	DDX_Control(pDX, IDC_COMBO1, SortItem);
	DDX_Control(pDX, IDC_CHECK1, Lock);
}

BEGIN_MESSAGE_MAP(CXprocessDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CXprocessDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON4, &CXprocessDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CXprocessDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CXprocessDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CXprocessDlg 消息处理程序

BOOL CXprocessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ProcessList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	ProcessList.InsertColumn(0, L"进程名", 0, 130);
	ProcessList.InsertColumn(1, L"进程ID", 0, 80);
	ProcessList.InsertColumn(2, L"进程路径", 0, 400);
	ListProcess(L"",0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CXprocessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXprocessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CXprocessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef struct AAA {
	int PID;
	WCHAR Name[260];
	WCHAR PATH[260];
}ProcessItem;

int SortByPID(ProcessItem a,ProcessItem b) {
	return a.PID < b.PID;
}
int SortByName(ProcessItem a, ProcessItem b) {
	return StrCmpCW(a.Name, b.Name) < 0;
}
int SortByPATH(ProcessItem a, ProcessItem b) {
	return StrCmpCW(a.PATH, b.PATH) < 0;
}

BOOL IsPrefix(WCHAR* Text, WCHAR* modstr) {
	WCHAR* s1 = Text, *s2 = modstr;
	int len=lstrlenW(modstr);
	char s[10];
	if (len == 0)return 1;

	for (int i = 0; i < len; i++) {
		if (tolower(*s1) != tolower(*s2))return 0;
		s1++;
		s2++;
	}
	return 1;
}

void CXprocessDlg::ListProcess(WCHAR * Text,int sort)
{
	std::vector<ProcessItem>v;
	ProcessList.DeleteAllItems();
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32 = { sizeof(pe32) };
	BOOL ret = Process32First(hSnap, &pe32);
	
	while (ret)
	{
		ProcessItem tmp;
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess) {
			GetModuleFileNameEx(hProcess, NULL, tmp.PATH, 260);
		}
		else {
			ret = Process32Next(hSnap, &pe32);
			continue;
		}
		CloseHandle(hProcess);
		if (!IsPrefix(pe32.szExeFile, Text)) {
			ret = Process32Next(hSnap, &pe32);
			continue;
		}
		tmp.PID = pe32.th32ProcessID;
		StrCpyW(tmp.Name, pe32.szExeFile);

		

		v.push_back(tmp);
		ret = Process32Next(hSnap, &pe32);
		
	}
	WCHAR PID[50];
	switch (sort) {
	case 0:
		std::sort(v.begin(), v.end(), SortByName);
		break;
	case 1:
		std::sort(v.begin(), v.end(), SortByPID);
		break;
	case 2:
		std::sort(v.begin(), v.end(), SortByPATH);
	default:
		std::sort(v.begin(), v.end(), SortByName);
	}
	ULONG INDEX = 0;
	for (auto item : v) {
		wsprintf(PID, L"%d", item.PID);
		ProcessList.InsertItem(INDEX, item.Name);
		ProcessList.SetItemText(INDEX, 1, PID);
		ProcessList.SetItemText(INDEX, 2, item.PATH);
		INDEX++;
	}

}



void CXprocessDlg::OnEnChangeEdit1()
{
	CString s;	
	GetDlgItemTextW(IDC_EDIT1,s);
	const size_t newsizew = (s.GetLength() + 1) * 2;
	WCHAR* n2stringw = new wchar_t[newsizew];
	wcscpy_s(n2stringw, newsizew, s);
	int index = SortItem.GetCurSel();
	ListProcess(n2stringw,index);
}


void CXprocessDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s;
	GetDlgItemTextW(IDC_EDIT1, s);
	const size_t newsizew = (s.GetLength() + 1) * 2;
	WCHAR* n2stringw = new wchar_t[newsizew];
	wcscpy_s(n2stringw, newsizew, s);
	int index = SortItem.GetCurSel();
	ListProcess(n2stringw, index);
}

DLLInject* DLLdlg = NULL;
WCHAR DLLPATH[260];
int InjectType;//最高位表示执行注入。

void InjectModule(WCHAR* szPath,int dwProcessId, WCHAR* FileName)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (!hProcess) {
		MessageBox(NULL, L"打开句柄失败，可能没有权限", FileName, MB_OK);
		goto process;
	}
	LPVOID lpAddress = VirtualAllocEx(hProcess, NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!lpAddress) {
		MessageBox(NULL, L"分配远程空间失败，可能没有权限", FileName, MB_OK);
		goto process;
	}
	size_t dwWriteLength = 0;
	BOOL res=WriteProcessMemory(hProcess, lpAddress, szPath, (wcslen(szPath) + 1) * 2, &dwWriteLength);
	if (!res) {
		MessageBox(NULL, L"写失败", FileName, MB_OK);
		goto free;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpAddress, NULL, NULL);
	if (!hThread) {
		MessageBox(NULL, L"远程线程创建失败", FileName, MB_OK);
		goto thread;
	}
	DWORD ReturnType=WaitForSingleObject(hThread, 8000);
	if (ReturnType == WAIT_TIMEOUT) {
		MessageBox(NULL, L"线程超时返回", FileName, MB_OK);
	}
	else {
		MessageBox(NULL, L"注入完成", FileName, MB_OK);
	}
thread:
	CloseHandle(hThread);
free:
	VirtualFreeEx(hProcess, lpAddress, 0, MEM_RELEASE);
process:
	CloseHandle(hProcess);
	
	return;
}

void CXprocessDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	int check = Lock.GetCheck();
	if (!check) {
		DLLdlg = new DLLInject(DLLPATH, &InjectType);
		DLLdlg->DoModal();
		delete DLLdlg;
		DLLdlg = NULL;
		goto inject;
	}
	else{
		if (!StrCmpW(DLLPATH,L"")) {
			DLLdlg = new DLLInject(DLLPATH, &InjectType);
			DLLdlg->DoModal();
			delete DLLdlg;
			DLLdlg = NULL;
		}
		else {
			InjectType |= 0x80000000;
			goto inject;
		}
	}
	return;
inject:
	if (InjectType & 0x80000000) {
		int i = ProcessList.GetSelectionMark();//获得选中行的行数
		CString s = ProcessList.GetItemText(i, 1);//(行，列)
		DWORD dwProcessId = _ttoi(s);
		s = ProcessList.GetItemText(i, 0);
		const size_t newsizew = (s.GetLength() + 1) * 2;
		WCHAR* FileName = new wchar_t[newsizew];
		wcscpy_s(FileName, newsizew, s);
		InjectModule(DLLPATH, dwProcessId, FileName);
	}
	InjectType &= 0x7fffffff;
}


void CXprocessDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox(NULL, DLLPATH);
}
