
// XprocessDlg.h: 头文件
//

#pragma once


// CXprocessDlg 对话框
class CXprocessDlg : public CDialogEx
{
// 构造
public:
	CXprocessDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_XPROCESS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl ProcessList;
	void ListProcess(WCHAR *,int);
	afx_msg void OnEnChangeEdit1();
	CEdit SearchText;
	CComboBox SortItem;
	afx_msg void OnBnClickedButton4();
	afx_msg int GetSelProcessId();
	afx_msg CString GetSelName();
	void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	CButton Lock;
	afx_msg void OnRclickList4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void KillProcess();
};
