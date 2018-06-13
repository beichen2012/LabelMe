// DlgAddedLabel.cpp : 实现文件
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "DlgAddedLabel.h"
#include "afxdialogex.h"


// CDlgAddedLabel 对话框

IMPLEMENT_DYNAMIC(CDlgAddedLabel, CDialogEx)

CDlgAddedLabel::CDlgAddedLabel(std::set<std::string>& labels, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SELECT_LABEL, pParent),
	mlabels(labels)
{
	
}

CDlgAddedLabel::~CDlgAddedLabel()
{
}

void CDlgAddedLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT_LABEL, mEditLabel);
	DDX_Control(pDX, IDC_LIST_ADDED_LABELS, mList);
}


BEGIN_MESSAGE_MAP(CDlgAddedLabel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgAddedLabel::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ADDED_LABELS, &CDlgAddedLabel::OnNMClickListAddedLabels)
END_MESSAGE_MAP()


// CDlgAddedLabel 消息处理程序


void CDlgAddedLabel::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CDialogEx::OnOK();
}


BOOL CDlgAddedLabel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  
	CRect rect;
	mList.GetClientRect(rect);
	mList.SetExtendedStyle(mList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int lw = rect.Width() / 20;
	mList.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mList.InsertColumn(1, _T("序号"), LVCFMT_LEFT, lw * 3);
	mList.InsertColumn(2, _T("名称"), LVCFMT_LEFT, lw * 16);
	mList.DeleteAllItems();
	int k = 0;
	for (auto& i : mlabels)
	{
		mList.InsertItem(k, _T(""));
		mList.SetItemText(k, 1, CString(std::to_string(k).c_str()));
		mList.SetItemText(k, 2, CString(i.c_str()));
		k++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgAddedLabel::OnNMClickListAddedLabels(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	//
	int idx = pNMItemActivate->iItem;
	if (idx > mlabels.size() - 1)
		return;

	//
	CString txt = mList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

	GetDlgItem(IDC_EDIT_INPUT_LABEL)->SetWindowTextW(txt);
}