#pragma once
#include "afxwin.h"
#include <set>
#include <string>
#include "afxcmn.h"


// CDlgAddedLabel 对话框

class CDlgAddedLabel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAddedLabel)

public:
	CDlgAddedLabel(std::set<std::string>& labels, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddedLabel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECT_LABEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString mEditLabel;
	std::set<std::string>& mlabels;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CListCtrl mList;
	afx_msg void OnNMClickListAddedLabels(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListAddedLabels(NMHDR *pNMHDR, LRESULT *pResult);
};
