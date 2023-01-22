#pragma once
#include "afxdialogex.h"


// XModuleDlg 对话框

class XModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(XModuleDlg)

public:
	XModuleDlg(int PID,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~XModuleDlg();
	int PID;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl ModuleList;
	void InitList();
	QWORD GetModuleBase();
	QWORD GetModuleSize();
	CString GetModuleName();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
	CButton HexDump;
};
