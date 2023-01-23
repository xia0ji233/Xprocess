// DLLDumpDlg.cpp: 实现文件
//

#include "pch.h"
#include "Xprocess.h"
#include "afxdialogex.h"
#include "DLLDumpDlg.h"


// DLLDumpDlg 对话框

IMPLEMENT_DYNAMIC(DLLDumpDlg, CDialogEx)

DLLDumpDlg::DLLDumpDlg(int* Option,CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	this->Option = Option;
}

DLLDumpDlg::~DLLDumpDlg()
{
}

void DLLDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, WriteCheck);
	DDX_Control(pDX, IDC_CHECK2, ExecuteCheck);
	DDX_Control(pDX, IDC_CHECK3, RememberMe);
	DDX_Control(pDX, IDC_CHECK4, ReadOnly);
}


BEGIN_MESSAGE_MAP(DLLDumpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &DLLDumpDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// DLLDumpDlg 消息处理程序


BOOL DLLDumpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	WriteCheck.SetCheck(1);
	ExecuteCheck.SetCheck(1);
	ReadOnly.SetCheck(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void DLLDumpDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	int Option = 0;
	if (WriteCheck.GetCheck()) {
		Option |= WRITE;
	}
	if (ExecuteCheck.GetCheck()) {
		Option |= EXECUTE;
	}
	if (ReadOnly.GetCheck()) {
		Option |= READONLY;
	}
	if (RememberMe.GetCheck()) {
		int ret=MessageBoxEx(NULL, L"勾选了记住选项，直到本次运行结束前您都不可以更改dump选项，确定吗？",L"警告", MB_YESNO,0);
		if (ret==7) {
			return;
		}
		Option |= REMEMBERME;
	}
	Option |= DUMP;
	*(this->Option) = Option;
	EndDialog(0);
}
