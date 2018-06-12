
// LabelMeWinDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "LabelMeWinDlg.h"
#include "afxdialogex.h"
#include "BrowseDir.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabelMeWinDlg �Ի���



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
	DDX_Control(pDX, IDC_BTN_LOAD_LABEL, mBtnLoadLabel);
	DDX_Control(pDX, IDC_LIST_FILES, mListFiles);
	DDX_Control(pDX, IDC_LIST_POLYS, mListROIs);
	DDX_Control(pDX, IDC_LIST_LABELS, mListLabels);
}

BEGIN_MESSAGE_MAP(CLabelMeWinDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLabelMeWinDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CLabelMeWinDlg::OnBnClickedBtnOpen)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_OPEN_DIR, &CLabelMeWinDlg::OnBnClickedBtnOpenDir)
	ON_BN_CLICKED(IDC_BTN_NEXT_IMAGE, &CLabelMeWinDlg::OnBnClickedBtnNextImage)
	ON_BN_CLICKED(IDC_BTN_PREV_IMAGE, &CLabelMeWinDlg::OnBnClickedBtnPrevImage)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CLabelMeWinDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CREATE_POLY, &CLabelMeWinDlg::OnBnClickedBtnCreatePoly)
	ON_BN_CLICKED(IDC_BTN_DELETE_POLY, &CLabelMeWinDlg::OnBnClickedBtnDeletePoly)
	ON_BN_CLICKED(IDC_BTN_EDIT_POLY, &CLabelMeWinDlg::OnBnClickedBtnEditPoly)
END_MESSAGE_MAP()


// CLabelMeWinDlg ��Ϣ�������

BOOL CLabelMeWinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	mBtnOpen.SetIcon(IDI_ICON_FILE);
	mBtnOpen.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnOpen.SetTooltipText(_T("��һ��ͼƬ"));

	mBtnOpenDir.SetIcon(IDI_ICON_FILE);
	mBtnOpenDir.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnOpenDir.SetTooltipText(_T("��һ��Ŀ¼"));

	mBtnNextImage.SetIcon(IDI_ICON_NEXT);
	mBtnNextImage.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnNextImage.SetTooltipText(_T("��һ��ͼƬ"));

	mBtnPrevImage.SetIcon(IDI_ICON_PREV);
	mBtnPrevImage.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnPrevImage.SetTooltipText(_T("��һ��ͼƬ"));

	mBtnSave.SetIcon(IDI_ICON_SAVE);
	mBtnSave.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnSave.SetTooltipText(_T("�����ע"));

	mBtnCreatePoly.SetIcon(IDI_ICON_OBJECTS);
	mBtnCreatePoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnCreatePoly.SetTooltipText(_T("���������"));

	mBtnDeletePoly.SetIcon(IDI_ICON_CANCEL);
	mBtnDeletePoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnDeletePoly.SetTooltipText(_T("ɾ�������"));

	mBtnEditPoly.SetIcon(IDI_ICON_COLOR_LINE);
	mBtnEditPoly.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnEditPoly.SetTooltipText(_T("�༭�����"));

	mBtnLoadLabel.SetIcon(IDI_ICON_LABELS);
	mBtnLoadLabel.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnLoadLabel.SetTooltipText(_T("���ر�ǩ�ļ�"));

	//�ļ��б�
	CRect rect;
	mListFiles.GetClientRect(rect);
	mListFiles.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int lw = rect.Width() / 6;
	mListFiles.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListFiles.InsertColumn(1, _T("���"), LVCFMT_LEFT, lw);
	mListFiles.InsertColumn(2, _T("����"), LVCFMT_LEFT, lw * 5);


	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLabelMeWinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLabelMeWinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLabelMeWinDlg::OnBnClickedOk()
{
	// TODO: �������ε�enter
	return;
	CDialogEx::OnOK();
}


HBRUSH CLabelMeWinDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  List �ؼ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CLabelMeWinDlg::OnBnClickedBtnOpen()
{
	// TODO: ���ļ��Ի���
	//1.
	//���ļ�
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����) 
	CString defaultDir = _T("./");   //Ĭ�ϴ򿪵��ļ�·�� 
	CString fileName = _T("");         //Ĭ�ϴ򿪵��ļ��� 
	CString filter = _T("ͼ�� (*.bmp; *.jpg; *.png; *.tiff)|*.bmp;*.jpg;*.png;*.tiff||");   //�ļ����ǵ����� 
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (openFileDlg.DoModal() == IDOK)
	{
		filePath = openFileDlg.GetPathName();
	}
	//
	if (filePath == _T(""))
	{
		MessageBox(_T("û��ѡ���ļ�"));
		return;
	}

	mCurrentFile = filePath;

	//��ȡ�ļ����ڴ�
}
CString CLabelMeWinDlg::SelectFolder()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = _T("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("��ѡ��һ���ļ��У�");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	return strFolderPath;
}

void CLabelMeWinDlg::RefreshFileLists()
{
	mListFiles.DeleteAllItems();
	for (int i = 0; i < mvFiles.size(); i++)
	{
		mListFiles.InsertItem(i, _T(""));
		mListFiles.SetItemText(i, 1, CString(std::to_string(i).c_str()));
		mListFiles.SetItemText(i, 2, CString(mvFiles[i].c_str()));
	}
}

void CLabelMeWinDlg::OnBnClickedBtnOpenDir()
{
	// TODO: ѡ��Ŀ¼
	mRootDir = SelectFolder();
	if (mRootDir == _T(""))
	{
		MessageBox(_T("û��ѡ���ļ��У�"));
		return;
	}
	//��ѯĿ¼�����е�ͼƬ
	std::string strRoot;
	CBrowseDir br;
	char* pr = cstring_to_char(mRootDir);
	br.SetInitDir(pr);
	strRoot = pr;
	delete[] pr;

	auto r1 = br.BeginBrowseFilenames("*.jpg");
	auto r2 = br.BeginBrowseFilenames("*.bmp");
	auto r3 = br.BeginBrowseFilenames("*.tiff");
	auto r4 = br.BeginBrowseFilenames("*.png");

	mvFiles.clear();
	mvFiles.insert(mvFiles.end(), r1.begin(), r1.end());
	mvFiles.insert(mvFiles.end(), r2.begin(), r2.end());
	mvFiles.insert(mvFiles.end(), r3.begin(), r3.end());
	mvFiles.insert(mvFiles.end(), r4.begin(), r4.end());

	//
	if (mvFiles.size() <= 0)
	{
		MessageBox(_T("ѡ����ļ�����û��ͼƬ�ļ���"));
		return;
	}
	mCurrentFile = CString(mvFiles[0].c_str());
	mCurrentIndex = 0;
	//��mvFiles�����·����ȥ����Ŀ¼
	for (auto& i : mvFiles)
	{
		auto j = i.substr(strRoot.length() + 1);
		i = j;
	}

	//�б���ʾͼƬ
	RefreshFileLists();

	//����ͼƬ
}


void CLabelMeWinDlg::OnBnClickedBtnNextImage()
{
	// TODO: ��һ��
}


void CLabelMeWinDlg::OnBnClickedBtnPrevImage()
{
	// TODO: ǰһ��

}

void CLabelMeWinDlg::OnBnClickedBtnSave()
{
	// TODO: ����
}


void CLabelMeWinDlg::OnBnClickedBtnCreatePoly()
{
	// TODO: ���������
}


void CLabelMeWinDlg::OnBnClickedBtnDeletePoly()
{
	// TODO: ɾ�������
}


void CLabelMeWinDlg::OnBnClickedBtnEditPoly()
{
	// TODO: �༭�����
}
