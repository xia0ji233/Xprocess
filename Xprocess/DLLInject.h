#pragma once
#include "afxdialogex.h"


// DLLInject 对话框

class DLLInject : public CDialogEx
{
	DECLARE_DYNAMIC(DLLInject)

public:
	DLLInject(WCHAR* DLLPATH , int* InjectType);   // 标准构造函数
	virtual ~DLLInject();
	WCHAR* DLLPATH;
	int* InjectType;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CComboBox InjectMethod;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
};
