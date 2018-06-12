
// LabelMeWinDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "LabelMeWinDlg.h"
#include "afxdialogex.h"
#include "BrowseDir.h"
#include "DlgAddedLabel.h"
using namespace cv;


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
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILES, &CLabelMeWinDlg::OnNMClickListFiles)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_LOAD_LABEL, &CLabelMeWinDlg::OnBnClickedBtnLoadLabel)
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
	int lw = rect.Width() / 15;
	mListFiles.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListFiles.InsertColumn(1, _T("���"), LVCFMT_LEFT, lw * 3);
	mListFiles.InsertColumn(2, _T("����"), LVCFMT_LEFT, lw * 11);


	

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



float CLabelMeWinDlg::GetPtDistI2(cv::Point& p1, cv::Point& p2)
{
	float dist = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
	dist = sqrt(dist);
	return dist;
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

#pragma region �ļ��б�
void CLabelMeWinDlg::OnNMClickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	// TODO: ��ѯ����˵ڼ��У���������Ӧ��ͼƬ
	int idx = pNMItemActivate->iItem;
	if (mvFiles.size() <= idx)
		return;

	//
	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[idx].c_str());
	mCurrentIndex = idx;
	LoadImageAndShow();
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

void CLabelMeWinDlg::ItemHighLight(int idx_no, int idx_yes, CListCtrl& list)
{
	//List 
	list.SetFocus();
	if(idx_no >= 0)
		list.SetItemState(idx_no, 0, LVIS_SELECTED | LVIS_FOCUSED);
	if (idx_yes >= 0)
	{
		list.SetItemState(idx_yes, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		list.EnsureVisible(idx_yes, FALSE);
	}
}
#pragma endregion

#pragma region ���水ť
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
	mCurrentIndex = 0;
	char dir[4096] = { 0 };
	char drive[4096] = { 0 };
	char filename[4096] = { 0 };
	char ext[4096] = { 0 };
	char* p = cstring_to_char(mCurrentFile);
	_splitpath(p, drive, dir, filename, ext);
	delete[] p;

	//
	mRootDir = CString(drive) + CString(dir);
	mvFiles.push_back(std::string(filename) + std::string(ext));
	mRootDir = mRootDir.Left(mRootDir.GetLength() - 1);

	//��ȡ�ļ����ڴ�
	LoadImageAndShow();
	RefreshFileLists();
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;
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
	LoadImageAndShow();
	//List 
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;
}


void CLabelMeWinDlg::OnBnClickedBtnNextImage()
{
	// TODO: ��һ��
	if (mCurrentIndex == mvFiles.size() - 1)
	{
		MessageBox(_T("�Ѿ������һ����"));
		return;
	}
	mCurrentIndex++;
	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[mCurrentIndex].c_str());
	LoadImageAndShow();
	//List 
	ItemHighLight(mCurrentIndex - 1, mCurrentIndex, mListFiles);


}


void CLabelMeWinDlg::OnBnClickedBtnPrevImage()
{
	// TODO: ǰһ��
	if (mCurrentIndex == 0)
	{
		MessageBox(_T("�Ѿ��ǵ�һ����"));
		return;
	}
	mCurrentIndex--;
	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[mCurrentIndex].c_str());
	LoadImageAndShow();
	//List 
	ItemHighLight(mCurrentIndex + 1, mCurrentIndex, mListFiles);
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
#pragma endregion

#pragma region ͼ����ʾ
void CLabelMeWinDlg::LoadImageAndShow()
{
	auto* p = cstring_to_char(mCurrentFile);
	mSrc = imread(p);
	if (mSrc.empty())
	{
		MessageBox(_T("�޷���ȡ�ļ���\r\n") + mCurrentFile);
		return;
	}

	//��ʾ
	MakeShowingImage(mSrc, mShow, IDC_PIC);
	ConvertMatToCImage(mShow, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
}

void CLabelMeWinDlg::ConvertMatToCImage(cv::Mat & src, CImage & cimg)
{
	void* data = src.data;
	int width = src.cols;
	int height = src.rows;
	int channels = src.channels();
	int step = src.step1();
	if (data == nullptr)
		return;
	if (cimg.IsNull())
		cimg.Create(width, height, 8 * channels);
	else if (cimg.GetWidth() != width ||
		cimg.GetHeight() != height ||
		cimg.GetBPP() / 8 != channels)
	{
		cimg.Destroy();
		cimg.Create(width, height, 8 * channels);
	}
	unsigned char* ps;
	unsigned char* pd = (unsigned char*)data;
	unsigned char* pimg = (unsigned char*)cimg.GetBits(); //��ȡCImage�����ش�������ָ�� 
	int cimg_step = cimg.GetPitch();//ÿ�е��ֽ���,ע���������ֵ�����и�

									// �����1��ͨ����ͼ��(�Ҷ�ͼ��) DIB��ʽ����Ҫ�Ե�ɫ������   
									// CImage�������˵�ɫ�壬����Ҫ�������и�ֵ�� 
	if (1 == channels)
	{
		RGBQUAD* ColorTable;
		int MaxColors = 256;
		//�������ͨ��CI.GetMaxColorTableEntries()�õ���С(�������CI.Load����ͼ��Ļ�)   
		ColorTable = new RGBQUAD[MaxColors];
		cimg.GetColorTable(0, MaxColors, ColorTable);//������ȡ��ָ��   
		for (int i = 0; i< MaxColors; i++)
		{
			ColorTable[i].rgbBlue = (BYTE)i;
			//BYTE��ucharһ���£���MFC�ж�����   
			ColorTable[i].rgbGreen = (BYTE)i;
			ColorTable[i].rgbRed = (BYTE)i;
		}
		cimg.SetColorTable(0, MaxColors, ColorTable);
		delete[]ColorTable;
	}
	for (int y = 0; y < height; y++)
	{
		ps = pd + y * step;
		for (int x = 0; x < width; x++)
		{
			if (1 == channels)
			{
				*(pimg + y * cimg_step + x) = ps[x];
			}
			else if (3 == channels)
			{
				*(pimg + y* cimg_step + x * 3 + 0) = ps[x * 3];
				*(pimg + y* cimg_step + x * 3 + 1) = ps[x * 3 + 1];
				*(pimg + y* cimg_step + x * 3 + 2) = ps[x * 3 + 2];
			}
		}
	}
}

void CLabelMeWinDlg::MakeShowingImage(cv::Mat & src, cv::Mat & dst, UINT id)
{
	// ����ͼƬ
	if (src.empty())
		return;
	CWnd* pwnd = GetDlgItem(id);
	CRect rect;
	pwnd->GetClientRect(rect);

	int h, w;
	float ratio_h = src.rows * 1.0 / rect.Height();
	float ratio_w = src.cols * 1.0 / rect.Width();
	if (ratio_h > ratio_w)
	{
		//��H��ѹ����������
		h = rect.bottom;
		w = src.cols / ratio_h;
	}
	else
	{
		w = rect.right;
		h = src.rows / ratio_w;
	}
	resize(src, dst, Size(w, h), 0.0, 0.0, INTER_CUBIC);
	if (dst.channels() == 1)
		cvtColor(dst, dst, COLOR_GRAY2BGR);
}

void CLabelMeWinDlg::DrawCImageCenter(ATL::CImage & image, CWnd * pwnd, CRect & dstRect, COLORREF bkColor)
{
	if (image.IsNull())
		return;
	CRect rect;
	pwnd->GetClientRect(&rect);
	int img_width = image.GetWidth();
	int img_height = image.GetHeight();
	//����ˮƽ��
	int cx0 = (rect.right - img_width) / 2;
	int cy0 = (rect.bottom - img_height) / 2;
	CClientDC dc(pwnd);
	HDC hdc = dc.GetSafeHdc();
	SetStretchBltMode(hdc, HALFTONE);
	SetBrushOrgEx(hdc, 0, 0, NULL);
	dstRect = rect;
	if (cx0 >= 0 && cy0 >= 0)
	{
		dstRect.left = cx0;
		dstRect.top = cy0;
		dstRect.right = cx0 + img_width;
		dstRect.bottom = cy0 + img_height;
	}
	//��䱳��ɫ
	FillRect(hdc, &rect, CBrush(bkColor));
	image.Draw(hdc, dstRect, CRect(0, 0, image.GetWidth(), image.GetHeight()));
	return;
}

#pragma endregion


#pragma region ����¼�
void CLabelMeWinDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �������
	CRect rect, rectRoi;
	if (mShow.empty() || mSrc.empty())
		goto RETURN;

	//�ж��Ƿ��ڿؼ���
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//�ж��Ƿ��ڿؼ���
	rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		goto RETURN;
	}
	mbLButtonDown = true;
RETURN:
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLabelMeWinDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �������
	if(!mbLButtonDown)
		return CDialogEx::OnLButtonUp(nFlags, point);

	//��¼�µ�
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//�ж��Ƿ��ڿؼ���
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		return CDialogEx::OnLButtonDown(nFlags, point);;
	}
	//
	mptStart.x = point.x - rectRoi.left;
	mptStart.y = point.y - rectRoi.top;

	//����
	mvRoi.push_back(mptStart);
	if (mvRoi.size() > 4)
	{
		//�ж��Ƿ�պ�
		auto dist = GetPtDistI2(mptStart, mvRoi[0]);
		if (dist < MIN_NEIGBOR)
		{
			//˵���Ǳպ���
			//1. �Ի���ѡ��������ǩ
			std::string label;
			CDlgAddedLabel* dlg = new CDlgAddedLabel(msLabels);
			if (dlg->DoModal() == IDOK)
			{
				auto txt = dlg->mEditLabel;
				char* p = cstring_to_char(txt);
				//
				msLabels.insert(std::string(p));
				label = p;
				delete[] p;
			}
			else
			{
				return CDialogEx::OnLButtonUp(nFlags, point);;
			}

			//2. ���������һ��
			auto pa = std::pair<std::string, std::vector<cv::Point>>(std::move(label), std::move(mvRoi));
			mvPolys.emplace_back(std::move(pa));
		}
	}


	

	//��һ��СԲ
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CLabelMeWinDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ����ƶ�

	CDialogEx::OnMouseMove(nFlags, point);
}

void CLabelMeWinDlg::DrawPolys(cv::Mat& canvas)
{
	for (auto& i : mvPolys)
	{
		auto& v = i.second;
		for (int k = 0; k < v.size(); k++)
		{
			auto & pt = v[k];
			circle(canvas, pt, POINT_CIRCLE_R, COLOR_BLUE, -1);
			if (k == v.size() - 1)
			{
				line(canvas, pt, v[0], COLOR_BLUE, POINT_LINE_R);
			}
			else
			{
				line(canvas, pt, v[k + 1], COLOR_BLUE, POINT_LINE_R);
			}
		}
	}
}
void CLabelMeWinDlg::DrawCurrentPoly(cv::Mat& canvas)
{
	for (int i = 0; i < mvRoi.size(); i++)
	{
		circle(canvas, mvRoi[i], POINT_CIRCLE_R, COLOR_GREEN, -1);
		if (i != mvRoi.size() - 1)
			line(canvas, mvRoi[i], mvRoi[i + 1], COLOR_GREEN, POINT_LINE_R);
	}
}

#pragma endregion


void CLabelMeWinDlg::OnBnClickedBtnLoadLabel()
{
	// TODO: ���ر�ǩ�ļ�
}
