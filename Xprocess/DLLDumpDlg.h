#pragma once
#include "afxdialogex.h"
#define WRITE 0x1
#define EXECUTE 0x2
#define READONLY 0x4
#define REMEMBERME 0x40000000
#define DUMP 0x80000000

// DLLDumpDlg 对话框

class DLLDumpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DLLDumpDlg)

public:
	DLLDumpDlg(int* Option,WCHAR *PATH,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DLLDumpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButton WriteCheck;
	CButton ExecuteCheck;
	CButton RememberMe;
	int* Option;
	WCHAR* PATH;
	afx_msg void OnBnClickedButton1();
	CButton ReadOnly;
	afx_msg void OnBnClickedButton2();
};
