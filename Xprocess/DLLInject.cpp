// DLLInject.cpp: 实现文件
//

#include "pch.h"
#include "Xprocess.h"
#include "afxdialogex.h"
#include "DLLInject.h"


// DLLInject 对话框

IMPLEMENT_DYNAMIC(DLLInject, CDialogEx)

DLLInject::DLLInject(WCHAR* PATH, int* Type)
	: CDialogEx(IDD_DIALOG1, nullptr)
{
	this->DLLPATH = PATH;
	this->InjectType = Type;
	//SetDlgItemTextW(IDC_EDIT1, PATH);
}

DLLInject::~DLLInject()
{
}

void DLLInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, InjectMethod);
}


BEGIN_MESSAGE_MAP(DLLInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &DLLInject::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &DLLInject::OnBnClickedButton3)
END_MESSAGE_MAP()


// DLLInject 消息处理程序


void DLLInject::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s;
	GetDlgItemTextW(IDC_EDIT1, s);
	const size_t newsizew = (s.GetLength() + 1) * 2;
	WCHAR* n2stringw = new wchar_t[newsizew];
	wcscpy_s(n2stringw, newsizew, s);
	StrCpyW(this->DLLPATH, n2stringw);
	*(this->InjectType) = InjectMethod.GetCurSel()|0x80000000;
	//DestroyWindow();
	EndDialog(0);
}


BOOL DLLInject::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemTextW(IDC_EDIT1, this->DLLPATH);
	InjectMethod.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void DLLInject::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(true, L".dll", L"*.dll", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"dll Files (*. dll)|*.dll||", NULL);
	if (fileDlg.DoModal() == IDOK) {
		CString s=fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, s);
	}
}
