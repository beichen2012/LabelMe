
// LabelMeWinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "LabelMeWinDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabelMeWinDlg 对话框



CLabelMeWinDlg::CLabelMeWinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LABELMEWIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_FRAME);
}

void CLabelMeWinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN, mBtnOpen);
	DDX_Control(pDX, IDC_BTN_OPEN_DIR, mBtnOpenDir);
	DDX_Control(pDX, IDC_BTN_NEXT_IMAGE, mBtnNextImage);
	DDX_Control(pDX, IDC_BTN_PREV_IMAGE, mBtnPrevImage);
	DDX_Control(pDX, IDC_BTN_SAVE, mBtnSave);
	DDX_Control(pDX, IDC_BTN_CREATE_POLY, mBtnCreatePoly);
	DDX_Control(pDX, IDC_BTN_DELETE_POLY, mBtnDeletePoly);
	DDX_Control(pDX, IDC_BTN_EDIT_POLY, mBtnEditPoly);
}

BEGIN_MESSAGE_MAP(CLabelMeWinDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLabelMeWinDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CLabelMeWinDlg::OnBnClickedBtnOpen)
END_MESSAGE_MAP()


// CLabelMeWinDlg 消息处理程序

BOOL CLabelMeWinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	mBtnOpen.SetIcon(IDI_ICON_FILE);
	mBtnOpen.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnOpen.SetTooltipText(_T("打开一个图片"));

	mBtnOpenDir.SetIcon(IDI_ICON_FILE);
	mBtnOpenDir.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnOpenDir.SetTooltipText(_T("打开一个目录"));

	mBtnNextImage.SetIcon(IDI_ICON_NEXT);
	mBtnNextImage.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnNextImage.SetTooltipText(_T("下一张图片"));

	mBtnPrevImage.SetIcon(IDI_ICON_PREV);
	mBtnPrevImage.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnPrevImage.SetTooltipText(_T("上一张图片"));

	mBtnSave.SetIcon(IDI_ICON_SAVE);
	mBtnSave.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnSave.SetTooltipText(_T("保存标注"));

	mBtnCreatePoly.SetIcon(IDI_ICON_OBJECTS);
	mBtnCreatePoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnCreatePoly.SetTooltipText(_T("创建多边形"));

	mBtnDeletePoly.SetIcon(IDI_ICON_CANCEL);
	mBtnDeletePoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnDeletePoly.SetTooltipText(_T("删除多边形"));

	mBtnEditPoly.SetIcon(IDI_ICON_COLOR_LINE);
	mBtnEditPoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnEditPoly.SetTooltipText(_T("编辑多边形"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLabelMeWinDlg::OnPaint()
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
HCURSOR CLabelMeWinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLabelMeWinDlg::OnBnClickedOk()
{
	// TODO: 这里屏蔽掉enter
	return;
	CDialogEx::OnOK();
}



void CLabelMeWinDlg::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
}
