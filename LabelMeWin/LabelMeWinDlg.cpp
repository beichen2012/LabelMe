
// LabelMeWinDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "LabelMeWinDlg.h"
#include "afxdialogex.h"
#include "BrowseDir.h"
#include "DlgAddedLabel.h"
#include <fstream>
#include <rapidjson\document.h>
#include <regex>
using namespace cv;

//���Ŵ���
#define MAX_SCALE_RATIO 4
#define SCALE_STEP 0.5

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabelMeWinDlg �Ի���



CLabelMeWinDlg::CLabelMeWinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LABELMEWIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_FRAME);
	mCurrentPolyIdx = -1;
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
	ON_BN_CLICKED(IDC_CHECK_AUTOSAVE, &CLabelMeWinDlg::OnBnClickedCheckAutosave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_POLYS, &CLabelMeWinDlg::OnNMClickListPolys)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_POLYS, &CLabelMeWinDlg::OnNMDblclkListPolys)
	ON_BN_CLICKED(IDC_CHECK_ZOOM, &CLabelMeWinDlg::OnBnClickedCheckZoom)
	ON_BN_CLICKED(IDC_BTN_ZOOM_ORIGIN, &CLabelMeWinDlg::OnBnClickedBtnZoomOrigin)
	ON_BN_CLICKED(IDC_BTN_ZOOM_UP, &CLabelMeWinDlg::OnBnClickedBtnZoomUp)
	ON_BN_CLICKED(IDC_BTN_ZOOM_DOWN, &CLabelMeWinDlg::OnBnClickedBtnZoomDown)
END_MESSAGE_MAP()

double PolygonTest(std::vector<cv::Point>& c, Point2f pt, bool measureDist)
{


	double result = 0;
	int i, total = c.size(), counter = 0;
	//CV_Assert(total >= 0 && (depth == CV_32S || depth == CV_32F));

	bool is_float = false;
	double min_dist_num = FLT_MAX, min_dist_denom = 1;
	Point ip(cvRound(pt.x), cvRound(pt.y));

	if (total == 0)
		return measureDist ? -DBL_MAX : -1;

	const Point* cnt = &c[0];
	const Point2f* cntf = (const Point2f*)cnt;

	if (!is_float && !measureDist && ip.x == pt.x && ip.y == pt.y)
	{
		// the fastest "purely integer" branch
		Point v0, v = cnt[total - 1];

		for (i = 0; i < total; i++)
		{
			int dist;
			v0 = v;
			v = cnt[i];

			if ((v0.y <= ip.y && v.y <= ip.y) ||
				(v0.y > ip.y && v.y > ip.y) ||
				(v0.x < ip.x && v.x < ip.x))
			{
				if (ip.y == v.y && (ip.x == v.x || (ip.y == v0.y &&
					((v0.x <= ip.x && ip.x <= v.x) || (v.x <= ip.x && ip.x <= v0.x)))))
					return 0;
				continue;
			}

			dist = (ip.y - v0.y)*(v.x - v0.x) - (ip.x - v0.x)*(v.y - v0.y);
			if (dist == 0)
				return 0;
			if (v.y < v0.y)
				dist = -dist;
			counter += dist > 0;
		}

		result = counter % 2 == 0 ? -1 : 1;
	}
	else
	{
		Point2f v0, v;
		Point iv;

		if (is_float)
		{
			v = cntf[total - 1];
		}
		else
		{
			v = cnt[total - 1];
		}

		if (!measureDist)
		{
			for (i = 0; i < total; i++)
			{
				double dist;
				v0 = v;
				if (is_float)
					v = cntf[i];
				else
					v = cnt[i];

				if ((v0.y <= pt.y && v.y <= pt.y) ||
					(v0.y > pt.y && v.y > pt.y) ||
					(v0.x < pt.x && v.x < pt.x))
				{
					if (pt.y == v.y && (pt.x == v.x || (pt.y == v0.y &&
						((v0.x <= pt.x && pt.x <= v.x) || (v.x <= pt.x && pt.x <= v0.x)))))
						return 0;
					continue;
				}

				dist = (double)(pt.y - v0.y)*(v.x - v0.x) - (double)(pt.x - v0.x)*(v.y - v0.y);
				if (dist == 0)
					return 0;
				if (v.y < v0.y)
					dist = -dist;
				counter += dist > 0;
			}

			result = counter % 2 == 0 ? -1 : 1;
		}
		else
		{
			for (i = 0; i < total; i++)
			{
				double dx, dy, dx1, dy1, dx2, dy2, dist_num, dist_denom = 1;

				v0 = v;
				if (is_float)
					v = cntf[i];
				else
					v = cnt[i];

				dx = v.x - v0.x; dy = v.y - v0.y;
				dx1 = pt.x - v0.x; dy1 = pt.y - v0.y;
				dx2 = pt.x - v.x; dy2 = pt.y - v.y;

				if (dx1*dx + dy1*dy <= 0)
					dist_num = dx1*dx1 + dy1*dy1;
				else if (dx2*dx + dy2*dy >= 0)
					dist_num = dx2*dx2 + dy2*dy2;
				else
				{
					dist_num = (dy1*dx - dx1*dy);
					dist_num *= dist_num;
					dist_denom = dx*dx + dy*dy;
				}

				if (dist_num*min_dist_denom < min_dist_num*dist_denom)
				{
					min_dist_num = dist_num;
					min_dist_denom = dist_denom;
					if (min_dist_num == 0)
						break;
				}

				if ((v0.y <= pt.y && v.y <= pt.y) ||
					(v0.y > pt.y && v.y > pt.y) ||
					(v0.x < pt.x && v.x < pt.x))
					continue;

				dist_num = dy1*dx - dx1*dy;
				if (dy < 0)
					dist_num = -dist_num;
				counter += dist_num > 0;
			}

			result = std::sqrt(min_dist_num / min_dist_denom);
			if (counter % 2 == 0)
				result = -result;
		}
	}

	return result;
}


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

	//��ǩ�б�
	mListLabels.GetClientRect(rect);
	mListLabels.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lw = rect.Width() / 15;
	mListLabels.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListLabels.InsertColumn(1, _T("���"), LVCFMT_LEFT, lw * 3);
	mListLabels.InsertColumn(2, _T("����"), LVCFMT_LEFT, lw * 11);

	//��ǰ��ǩ�б�
	mListROIs.GetClientRect(rect);
	mListROIs.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	lw = rect.Width() / 15;
	mListROIs.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListROIs.InsertColumn(1, _T("���"), LVCFMT_LEFT, lw * 3);
	mListROIs.InsertColumn(2, _T("��ǩ"), LVCFMT_LEFT, lw * 11);

	//�Զ�����
	mbAutoSave = true;
	((CButton *)GetDlgItem(IDC_CHECK_AUTOSAVE))->SetCheck(1);

	mnCreateOrEdit = 0;
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(TRUE);

	mbRButtonDown = false;
	mnRButtonState = 0;

	// ����ʹ��
	((CButton*)GetDlgItem(IDC_CHECK_ZOOM))->SetCheck(0);
	GetDlgItem(IDC_BTN_ZOOM_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ZOOM_DOWN)->EnableWindow(FALSE);
	mfScalor = 1.0f;
	mbZoom = false;

	//״̬��
	mStatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);
	mStatusBar.GetClientRect(rect);
	
	int strPartDim[4] = { rect.Width() / 4,rect.Width() / 4 * 2,rect.Width() / 4 * 3 , -1 };
	mStatusBar.SetParts(4, strPartDim);
	
	//����״̬���ı�
	mStatusBar.SetText(_T(""), 0, 0);
	mStatusBar.SetText(_T(""), 1, 0);
	mStatusBar.SetText(_T(""), 2, 0);
	mStatusBar.SetText(_T(""), 3, 0);


	LOGD("Init dialog...");
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

void CLabelMeWinDlg::SaveLabels()
{
	//TODO: �����ע
	std::string strRoot;
	char* p = cstring_to_char(mRootDir);
	strRoot = p;
	delete[] p;

	// ��ǩ�ļ�
	std::string pathLabel = strRoot + "\\" + "label.txt";
	std::ofstream ofs;
	ofs.open(pathLabel, std::ios::out);
	if (!ofs.is_open())
	{
		MessageBox(_T("��ע�⣺��ǩ�ļ���δ���棬���Ⲣ��Ӱ���ע�ļ��ı���"));
	}
	else
	{
		for (auto& i : msLabels)
		{
			ofs << i << std::endl;
		}
		ofs.close();
	}
	//��ע�ļ�
	std::vector<TImgAnno> va;
	for (auto& i : mvPolys)
	{
		TImgAnno ta;
		ta.label = i.first;
		for (auto& j : i.second)
		{
			TPtAnno pt;
			pt.x = j.x;
			pt.y = j.y;
			ta.pts.push_back(pt);
		}
		va.push_back(ta);
	}
	TAnnoInfo outInfo;
	outInfo.va = std::move(va);
	outInfo.x_scale = float(mSrc.cols) / float(mShow.cols);
	outInfo.y_scale = float(mSrc.rows) / float(mShow.rows);
	//
	char drive[4096] = { 0 };
	char dir[4096] = { 0 };
	char filename[4096] = { 0 };
	p = cstring_to_char(mCurrentFile);
	_splitpath(p, drive, dir, filename, NULL);
	delete[] p;
	std::string pathJson = std::string(drive) + dir + filename + ".json";
	iguana::string_stream ss;
	iguana::json::to_json(ss, outInfo);
	
	ofs.open(pathJson, std::ios::out);
	if (!ofs.is_open())
	{
		MessageBox(_T("�޷������ͼƬ�ı�ע�ļ���"));
	}
	else
	{
		ofs << ss.str() << std::endl;
		ofs.close();
	}
}

cv::Point CLabelMeWinDlg::CanvasPt2SrcPt(cv::Point pt)
{
	//��������ƽ��
	if (mSrc.empty())
	{
		return{ -1,-1 };
	}

	Point2f rt;
	int dx = mptCurrentOrigin.x - mSrc.cols / 2;
	int dy = mptCurrentOrigin.y - mSrc.rows / 2;
	rt.x = float(mCurrentSrcSize.x) / float(mShow.cols);
	rt.y = float(mCurrentSrcSize.y) / float(mShow.rows);

	//����
	pt.x *= rt.x;
	pt.y *= rt.y;

	//ƽ�Ƶ�ROI��ʼ��
	pt.x += mCurrentSrcRoi.x;
	pt.y += mCurrentSrcRoi.y;

	return pt;
}

cv::Point CLabelMeWinDlg::SourcePt2CanvasPt(cv::Point pt)
{
	//��ƽ��������
	Point2f rt;
	int dx = mptCurrentOrigin.x - mSrc.cols / 2;
	int dy = mptCurrentOrigin.y - mSrc.rows / 2;

	//ƽ�Ƶ�ROI����
	pt.x -= mCurrentSrcRoi.x;
	pt.y -= mCurrentSrcRoi.y;

	//���ţ�
	rt.x = float(mCurrentSrcSize.x) / float(mShow.cols);
	rt.y = float(mCurrentSrcSize.y) / float(mShow.rows);

	pt.x /= rt.x;
	pt.y /= rt.y;
	return pt;
}

cv::Point2f CLabelMeWinDlg::GetCurrentScaler()
{
	Point2f rt;
	//if (mScaleRatio == 0)
	//{
		rt.x = float(mSrc.cols) / float(mShow.cols);
		rt.y = float(mSrc.rows) / float(mShow.rows);
	/*}
	else
	{
		rt.x = float(mroiScale.width) / float(mShow.cols);
		rt.y = float(mroiScale.height) / float(mShow.rows);
	}*/
	return rt;
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



	//����
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("�Ƿ񱣴浱ǰͼƬ�ı�ע��"), _T("��ʾ"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//����
				bSave = true;
			}
		}
		else
		{
			//�Զ�����
			bSave = true;
		}
	}

	if (bSave)
	{
		// ִ�б���
		SaveLabels();
	}
	//���
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();


	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[idx].c_str());
	mCurrentIndex = idx;
	LoadImageAndShow();
	FindCurrentLabels();
	//

}

void CLabelMeWinDlg::FindCurrentLabels()
{
	if (mCurrentFile.IsEmpty())
		return;
	char drive[4096] = { 0 };
	char dir[4096] = { 0 };
	char filename[4096] = { 0 };
	char* p = cstring_to_char(mCurrentFile);
	_splitpath(p, drive, dir, filename, NULL);
	delete[] p;

	std::string fn = std::string(drive) + dir + filename + ".json";

	//load json
	mvPolys.clear();
	using namespace rapidjson;
	Document d;
	char* pd = ReadWholeFile(fn.c_str());
	if (pd == NULL)
		return;
	if (d.Parse(pd).HasParseError())
	{
		delete[] pd;
		return;
	}
	delete[] pd;
	//parse
	Value& v = d["va"];
	for (SizeType i = 0; i < v.Size(); i++)
	{
		//std::pair<std::string, std::vector<cv::Point>> roi;
		std::string label;
		std::vector<cv::Point> pts;
		auto& json_ia = v[i];
		label = json_ia["label"].GetString();
		auto& cell = json_ia["pts"].GetArray();
		for (SizeType j = 0; j < cell.Size(); j++)
		{
			int x = cell[j]["x"].GetInt();
			int y = cell[j]["y"].GetInt();
			pts.push_back({ x,y });
		}
		mvPolys.emplace_back(std::pair<std::string, std::vector<cv::Point>>(std::move(label), std::move(pts)));
	}

	// ��ǩ
	for (auto& i : mvPolys)
	{
		msLabels.insert(i.first);
	}

	//refresh list
	RefreshLabelLists();
	RefreshROILists();

	//��ʾ 
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
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

void CLabelMeWinDlg::RefreshLabelLists()
{
	mListLabels.DeleteAllItems();
	int i = 0;
	for (auto& ele : msLabels)
	{
		mListLabels.InsertItem(i, _T(""));
		mListLabels.SetItemText(i, 1, CString(std::to_string(i).c_str()));
		mListLabels.SetItemText(i, 2, CString(ele.c_str()));
		i++;
	}
}

void CLabelMeWinDlg::RefreshROILists()
{
	mListROIs.DeleteAllItems();
	int i = 0;
	for (auto& ele : mvPolys)
	{
		mListROIs.InsertItem(i, _T(""));
		mListROIs.SetItemText(i, 1, CString(std::to_string(i).c_str()));
		mListROIs.SetItemText(i, 2, CString(ele.first.c_str()));
		i++;
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
	ENTER_FUNC;
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
	LOGD("file path is: {}", p);
	_splitpath(p, drive, dir, filename, ext);
	delete[] p;

	//
	mRootDir = CString(drive) + CString(dir);
	mvFiles.push_back(std::string(filename) + std::string(ext));
	mRootDir = mRootDir.Left(mRootDir.GetLength() - 1);

	//��ȡ�ļ����ڴ�
	LoadImageAndShow();
	RefreshFileLists();
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;

	//�����Ƿ��б��ļ��ı�ע�ļ������벢��ʾ
	FindCurrentLabels();
	LEAVE_FUNC;
}

void CLabelMeWinDlg::OnBnClickedBtnOpenDir()
{
	// TODO: ѡ��Ŀ¼
	ENTER_FUNC;
	auto&& root_dir = SelectFolder();
	if (root_dir == _T(""))
	{
		MessageBox(_T("û��ѡ���ļ��У�"));
		return;
	}
	mRootDir = root_dir;
	//��ѯĿ¼�����е�ͼƬ
	std::string strRoot;
	CBrowseDir br;
	char* pr = cstring_to_char(mRootDir);
	LOGD("root folder is: {}", pr);
	br.SetInitDir(pr);
	strRoot = pr;
	delete[] pr;

	auto r1 = br.BeginBrowseFilenames("*.jpg");
	auto r2 = br.BeginBrowseFilenames("*.bmp");
	auto r3 = br.BeginBrowseFilenames("*.tiff");
	auto r4 = br.BeginBrowseFilenames("*.png");

	LOGD("find jpg in root folder, num is: -> {}", r1.size());
	LOGD("find bmp in root folder, num is: -> {}", r2.size());
	LOGD("find tiff in root folder, num is: -> {}", r3.size());
	LOGD("find png in root folder, num is: -> {}", r4.size());

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
	//����, value-index
	std::vector<std::pair<int, int>> sortIndex;
	std::regex re(R"((\d)+)");
	std::cmatch match;
	char filename[4096];
	for (int i = 0; i < mvFiles.size(); i++)
	{
		auto ele = std::pair<int, int>(-1, i);
		memset(filename, 0, sizeof(filename));
		_splitpath(mvFiles[i].c_str(), NULL, NULL, filename, NULL);

		if (std::regex_search(filename, match, re))
		{
			//ele.first = std::atoi(match[0].str());
			ele.first = std::stoi(match[0].str());
		}
		sortIndex.emplace_back(ele);
	}
	std::sort(sortIndex.begin(), sortIndex.end(), [](const std::pair<int, int>& left, const std::pair<int, int>& right) {
		return left.first < right.first;
	});
	//
	auto tmpFiles = std::move(mvFiles);
	for (int i = 0; i < sortIndex.size(); i++)
	{
		mvFiles.emplace_back(tmpFiles[sortIndex[i].second]);
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
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();

	//����ͼƬ
	LoadImageAndShow();
	FindCurrentLabels();
	//List 
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;

	LEAVE_FUNC;
}


void CLabelMeWinDlg::OnBnClickedBtnNextImage()
{
	// TODO: ��һ��
	if (mCurrentIndex == mvFiles.size() - 1)
	{
		MessageBox(_T("�Ѿ������һ����"));
		return;
	}
	//����
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("�Ƿ񱣴浱ǰͼƬ�ı�ע��"), _T("��ʾ"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//����
				bSave = true;
			}
		}
		else
		{
			//�Զ�����
			bSave = true;
		}
	}

	if (bSave)
	{
		// ִ�б���
		SaveLabels();
	}
	//���
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();

	mCurrentIndex++;
	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[mCurrentIndex].c_str());
	LoadImageAndShow();
	FindCurrentLabels();
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
	//����
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("�Ƿ񱣴浱ǰͼƬ�ı�ע��"), _T("��ʾ"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//����
				bSave = true;
			}
		}
		else
		{
			//�Զ�����
			bSave = true;
		}
	}

	if (bSave)
	{
		// ִ�б���
		SaveLabels();
	}
	//���
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();

	mCurrentIndex--;
	mCurrentFile = mRootDir + _T("\\") + CString(mvFiles[mCurrentIndex].c_str());
	LoadImageAndShow();
	FindCurrentLabels();
	//List 
	ItemHighLight(mCurrentIndex + 1, mCurrentIndex, mListFiles);
}

void CLabelMeWinDlg::OnBnClickedBtnSave()
{
	// TODO: ����
	if (mvPolys.size() > 0)
	{
		// ִ�б���
		SaveLabels();
	}
}


void CLabelMeWinDlg::OnBnClickedBtnCreatePoly()
{
	// TODO: ���������
	mnCreateOrEdit = 0;
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(TRUE);
}


void CLabelMeWinDlg::OnBnClickedBtnDeletePoly()
{
	// TODO: ɾ�������
	if (mCurrentPolyIdx < 0)
		return;
	if (mCurrentPolyIdx >= mvPolys.size())
		return;

	//
	mvPolys.erase(mvPolys.begin() + mCurrentPolyIdx);
	RefreshROILists();

	Mat tmp = mShow.clone();
	DrawCurrentPoly(tmp);
	DrawPolys(tmp);

	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
}


void CLabelMeWinDlg::OnBnClickedBtnEditPoly()
{
	// TODO: �༭�����
	mnCreateOrEdit = 1;
	mvRoi.clear();
	mbLButtonDown = false;
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(FALSE);
}
#pragma endregion

#pragma region ͼ����ʾ
void CLabelMeWinDlg::LoadImageAndShow()
{
	auto* p = cstring_to_char(mCurrentFile);
	mSrc = imread(p);
	delete[] p;
	if (mSrc.empty())
	{
		MessageBox(_T("�޷���ȡ�ļ���\r\n") + mCurrentFile);
		return;
	}

	//��ʾ
	MakeShowingImage(mSrc, mShow, IDC_PIC);
	ConvertMatToCImage(mShow, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

	//
	mptCurrentOrigin.x = mSrc.cols / 2;
	mptCurrentOrigin.y = mSrc.rows / 2;

	//
	mbZoom = false;
	GetDlgItem(IDC_BTN_ZOOM_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ZOOM_DOWN)->EnableWindow(FALSE);

	//
	CString msg;
	msg.Format(_T("�� %d ��/�� %d ��"), mCurrentIndex + 1, mvFiles.size());
	mStatusBar.SetText(msg, 0, 0);

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
	mCurrentSrcSize.x = src.cols;
	mCurrentSrcSize.y = src.rows;
	mCurrentSrcRoi = { 0,0,src.cols, src.rows };
	resize(src, dst, Size(w, h), 0.0, 0.0, INTER_CUBIC);
	if (dst.channels() == 1)
		cvtColor(dst, dst, COLOR_GRAY2BGR);
	mStatusBar.SetText(_T("100%"), 3, 0);
	CString msg;
	msg.Format(_T("��ǰ: (%d, %d, %d, %d)"), mCurrentSrcRoi.x, mCurrentSrcRoi.y, mCurrentSrcRoi.width, mCurrentSrcRoi.height);
	mStatusBar.SetText(msg, 2, 0);
	msg.Format(_T("ͼ��: %d x %d"), src.cols, src.rows);
	mStatusBar.SetText(msg, 1, 0);
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
	mptButtonDown.x = point.x;
	mptButtonDown.y = point.y;
	//
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ClipCursor(rect);
RETURN:
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLabelMeWinDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �������
	ClipCursor(NULL);
	if(!mbLButtonDown)
		return CDialogEx::OnLButtonUp(nFlags, point);
	mbLButtonDown = false;

	//�����������ģʽ�£�ʲôҲ����
	if (mbZoom)
	{
		//��¼��ƫ������
		int dx = point.x - mptButtonDown.x;
		int dy = point.y - mptButtonDown.y;

		//ת��Ϊԭͼ��ƫ����
		float scale_x = mCurrentSrcSize.x * 1.0f / mShow.cols;
		float scale_y = mCurrentSrcSize.y * 1.0f / mShow.rows;
		dx *= scale_x;
		dy *= scale_y;

		//
		mptCurrentOrigin.x -= dx;
		mptCurrentOrigin.y -= dy;

		if (MakeScaleImage(mSrc, mShow, IDC_PIC) < 0)
		{
			mptCurrentOrigin.x += dx;
			mptCurrentOrigin.y += dy;
			return CDialogEx::OnLButtonUp(nFlags, point);
		}

		cv::Mat tmp = mShow.clone();

		DrawCross(tmp);
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
		return CDialogEx::OnLButtonUp(nFlags, point);
	}
		
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

	mptStart = CanvasPt2SrcPt(mptStart);

	//�������ͼ��߽������ˣ��Ͳ�Ҫ��
	if(mptStart.x < -1 || mptStart.y < -1)
		return CDialogEx::OnLButtonDown(nFlags, point);;


	if (mnCreateOrEdit == 1)
	{ //## �༭ģʽ

		//�鿴��ǰ�㣬���ĸ�Poly����
		int idx = -1;
		for (int i = 0; i < mvPolys.size(); i++)
		{
			auto& v = mvPolys[i].second;
			//std::vector<cv::Point2f> c;
			//for (auto& j : v)
			//	c.push_back(Point2f(j.x, j.y));
			auto ret = PolygonTest(v, Point2f(mptStart.x, mptStart.y), false);
			if (ret >= 0)
			{
				//��¼��i
				idx = i;
				break;
			}
		}

		if(idx < 0)
			return CDialogEx::OnLButtonUp(nFlags, point);

		//��
		ItemHighLight(mCurrentPolyIdx, idx, mListROIs);
		mCurrentPolyIdx = idx;
		DrawIdxRedPolys(idx);

		// ӳ��ROI�б�
		

		return CDialogEx::OnLButtonUp(nFlags, point);
	}
	
	//## ����ģʽ
	//����
	mvRoi.push_back(mptStart);
	if (mvRoi.size() > 4)
	{
		//�ж��Ƿ�պ�
		auto dist = GetPtDistI2(mptStart, mvRoi[0]);
		auto scaler = GetCurrentScaler();
		if (dist < MIN_NEIGBOR * scaler.x)
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
				mvRoi.clear();
				MakeScaleImage(mSrc, mShow, IDC_PIC);
				Mat tmp = mShow.clone();
				DrawPolys(tmp);
				DrawCurrentPoly(tmp);
				ConvertMatToCImage(tmp, mCimg);
				DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
				return CDialogEx::OnLButtonUp(nFlags, point);;
			}

			//2. ���������һ��
			auto pa = std::pair<std::string, std::vector<cv::Point>>(std::move(label), std::move(mvRoi));
			mvPolys.emplace_back(std::move(pa));
			//3. ˢ�±�ǩ�б�
			RefreshLabelLists();
			RefreshROILists();
		}
	}

	//��һ��СԲ
	MakeScaleImage(mSrc, mShow, IDC_PIC);
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
	if (mSrc.empty())
		return CDialogEx::OnMouseMove(nFlags, point);
	if(!mbLButtonDown)
		return CDialogEx::OnMouseMove(nFlags, point);

	//�������������϶�ͼƬ
	//��¼�µ�
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//�ж��Ƿ��ڿؼ���
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		return CDialogEx::OnMouseMove(nFlags, point);
	}


	//1.���ж��Ƿ�������ģʽ��
	if (mbZoom)
	{
		static int drag_interval = 0;
		drag_interval++;
		if (drag_interval % 12 != 0)
			return CDialogEx::OnMouseMove(nFlags, point);
		drag_interval = 0;
		//��¼��ƫ������
		int dx = point.x - mptButtonDown.x;
		int dy = point.y - mptButtonDown.y;
		mptButtonDown.x = point.x;
		mptButtonDown.y = point.y;

		//ת��Ϊԭͼ��ƫ����
		float scale_x = mCurrentSrcSize.x * 1.0f / mShow.cols;
		float scale_y = mCurrentSrcSize.y * 1.0f / mShow.rows;
		dx *= scale_x;
		dy *= scale_y;

		//
		mptCurrentOrigin.x -= dx;
		mptCurrentOrigin.y -= dy;

		if (MakeScaleImage(mSrc, mShow, IDC_PIC) < 0)
		{
			mptCurrentOrigin.x += dx;
			mptCurrentOrigin.y += dy;
			return CDialogEx::OnLButtonUp(nFlags, point);
		}

		cv::Mat tmp = mShow.clone();

		DrawCross(tmp);
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
		return CDialogEx::OnMouseMove(nFlags, point);
	}


	//2, ��������ģʽ������������ע
	mptStart.x = point.x - rectRoi.left;
	mptStart.y = point.y - rectRoi.top;

	mptStart = CanvasPt2SrcPt(mptStart);

	//�������ͼ��߽������ˣ��Ͳ�Ҫ��
	if (mptStart.x < -1 || mptStart.y < -1)
		return CDialogEx::OnLButtonDown(nFlags, point);;

	mvRoi.push_back(mptStart);
	//��һ��СԲ
	static int draw_intervel = 0;
	if (draw_intervel == 8)
	{
		draw_intervel = 0;
		MakeScaleImage(mSrc, mShow, IDC_PIC);
		Mat tmp = mShow.clone();
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
	}
	draw_intervel++;
	
	return CDialogEx::OnMouseMove(nFlags, point);
}

void CLabelMeWinDlg::DrawPolys(cv::Mat& canvas)
{
	Point pt1, pt2;
	for (auto& i : mvPolys)
	{
		auto& v = i.second;
		for (int k = 0; k < v.size(); k++)
		{
			pt1 = v[k];
			pt1 = SourcePt2CanvasPt(pt1);
			circle(canvas, pt1, POINT_CIRCLE_R, COLOR_BLUE, -1);
			if (k == v.size() - 1)
			{
				pt2 = SourcePt2CanvasPt(v[0]);
				line(canvas, pt1, pt2, COLOR_BLUE, POINT_LINE_R);
			}
			else
			{
				pt2 = SourcePt2CanvasPt(v[k + 1]);
				line(canvas, pt1, pt2, COLOR_BLUE, POINT_LINE_R);
			}
		}
	}
}
void CLabelMeWinDlg::DrawCurrentPoly(cv::Mat& canvas)
{
	Point pt1, pt2;
	//����ƫ����
	int dx = mptCurrentOrigin.x - canvas.cols / 2;
	int dy = mptCurrentOrigin.y - canvas.rows / 2;
	for (int i = 0; i < mvRoi.size(); i++)
	{
		pt1 = SourcePt2CanvasPt(mvRoi[i]);
		circle(canvas, pt1, POINT_CIRCLE_R, COLOR_GREEN, -1);
		if (i != mvRoi.size() - 1)
		{
			pt2 = SourcePt2CanvasPt(mvRoi[i + 1]);
			line(canvas, pt1, pt2 , COLOR_GREEN, POINT_LINE_R);
		}	
	}
}

#pragma endregion


void CLabelMeWinDlg::OnBnClickedBtnLoadLabel()
{
	// TODO: ���ر�ǩ�ļ�
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����) 
	CString defaultDir = _T("./");   //Ĭ�ϴ򿪵��ļ�·�� 
	CString fileName = _T("");         //Ĭ�ϴ򿪵��ļ��� 
	CString filter = _T("��ͨ�ļ� (*.txt)|*.txt||");   //�ļ����ǵ����� 
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
	//���ж���
	std::ifstream ifs;
	char* p = cstring_to_char(filePath);
	ifs.open(p, std::ios::in);
	delete[] p;
	if (!ifs.is_open())
	{
		MessageBox(_T("�򿪱�ǩ�ļ�ʧ�ܣ�"));
		return;
	}
	std::string line;
	while (getline(ifs, line))
	{
		msLabels.insert(line);
	}
	ifs.close();

	//ˢ�±�ǩ�б�
	RefreshLabelLists();
}

 
void CLabelMeWinDlg::OnBnClickedCheckAutosave()
{
	// TODO: �Զ�����?
	if (((CButton *)GetDlgItem(IDC_CHECK_AUTOSAVE))->GetCheck() == 1)
		mbAutoSave = true;
	else
		mbAutoSave = false;
}

void CLabelMeWinDlg::OnNMDblclkListPolys(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	int index = pNMItemActivate->iItem;
	if (index > mvPolys.size() - 1 || index < 0)
		return;

	// �޸ĵ�ǰѡ���� ����εı�ǩ
	std::string newLabel;
	CDlgAddedLabel* dlg = new CDlgAddedLabel(msLabels);
	if (dlg->DoModal() == IDOK)
	{
		auto txt = dlg->mEditLabel;
		char* p = cstring_to_char(txt);
		//
		msLabels.insert(std::string(p));
		newLabel = p;
		delete[] p;

		//
		
		mCurrentPolyIdx = index;
		mvPolys[index].first = newLabel;
		//
		DrawIdxRedPolys(index);

		//��ע�б��ǩ�б�Ҫˢ�� 
		RefreshLabelLists();
		RefreshROILists();
	}
	else
	{
		MessageBox(_T("��û��ѡ���µı�ǩ����ǩ�������޸ģ�"));
	}
	delete dlg;

}

void CLabelMeWinDlg::OnNMClickListPolys(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ��ɫ��ʾ��ǰѡ�е���
	*pResult = 0;

	//
	int index = pNMItemActivate->iItem;
	if (index > mvPolys.size() - 1 || index < 0)
		return;
	mCurrentPolyIdx = index;
	//
	DrawIdxRedPolys(index);
}

void CLabelMeWinDlg::DrawIdxRedPolys(int idx)
{
	Mat tmp = mShow.clone();
	DrawCurrentPoly(tmp);
	DrawPolys(tmp);
	auto& v = mvPolys[idx].second;

	for (int k = 0; k < v.size(); k++)
	{
		auto & pt = v[k];
		circle(tmp, SourcePt2CanvasPt(pt), POINT_CIRCLE_R, COLOR_RED, -1);
		if (k == v.size() - 1)
		{
			line(tmp, SourcePt2CanvasPt(pt), SourcePt2CanvasPt(v[0]), COLOR_RED, POINT_LINE_R);
		}
		else
		{
			line(tmp, SourcePt2CanvasPt(pt), SourcePt2CanvasPt(v[k + 1]), COLOR_RED, POINT_LINE_R);
		}
	}

	//
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
}

BOOL CLabelMeWinDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//����ESC�¼�

			//
			if (!mbLButtonDown && !mbRButtonDown && !mSrc.empty())
			{
				//�����겻���ڰ���״̬�������Ѿ�������ͼƬ
				//��ô��ȡ�����һ����ı༭
				//��һ��СԲ
				if (mvRoi.size() > 0)
				{
					mvRoi.pop_back();
					Mat tmp = mShow.clone();
					DrawPolys(tmp);
					DrawCurrentPoly(tmp);
					ConvertMatToCImage(tmp, mCimg);
					DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
				}
			}

			//ȡ��Ĭ�Ϲرմ����¼�
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

#pragma region ���Ź���

/*����ʹ��*/
void CLabelMeWinDlg::OnBnClickedCheckZoom()
{
	// TODO: ����ʹ��
	if (mSrc.empty())
	{
		MessageBox(_T("û�ж�ȡͼƬ��"));
		((CButton*)GetDlgItem(IDC_CHECK_ZOOM))->SetCheck(0);
		return;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_ZOOM))->GetCheck() == 1)
	{
		mbZoom = true;
		GetDlgItem(IDC_BTN_ZOOM_UP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_ZOOM_DOWN)->EnableWindow(TRUE);

		//��ʮ���ߣ���������϶�ͼƬ
		MakeScaleImage(mSrc, mShow, IDC_PIC);
		Mat tmp = mShow.clone();
		DrawCurrentPoly(tmp);
		DrawCross(tmp);
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

	}
	else
	{
		mbZoom = false;
		GetDlgItem(IDC_BTN_ZOOM_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ZOOM_DOWN)->EnableWindow(FALSE);

		//ȡ����ʮ����
		MakeScaleImage(mSrc, mShow, IDC_PIC);
		Mat tmp = mShow.clone();
		DrawCurrentPoly(tmp);
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
	}

}


void CLabelMeWinDlg::OnBnClickedBtnZoomOrigin()
{
	// TODO: ��ԭ
	mfScalor = 1.0f;
	((CButton*)GetDlgItem(IDC_CHECK_ZOOM))->SetCheck(0);
	GetDlgItem(IDC_BTN_ZOOM_DOWN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ZOOM_UP)->EnableWindow(FALSE);
	mbZoom = false;
	mCurrentSrcSize.x = mSrc.cols;
	mCurrentSrcSize.y = mSrc.rows;
	mCurrentSrcRoi = { 0, 0, mSrc.cols, mSrc.rows };
	mptCurrentOrigin.x = mSrc.cols / 2;
	mptCurrentOrigin.y = mSrc.rows / 2;
	MakeShowingImage(mSrc, mShow, IDC_PIC);
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
}


void CLabelMeWinDlg::OnBnClickedBtnZoomUp()
{
	// TODO: �Ŵ�
	mfScalor += SCALE_STEP;
	if (mfScalor > MAX_SCALE_RATIO + 0.00001)
	{
		mfScalor = MAX_SCALE_RATIO;
		return;
	}

	//
	MakeScaleImage(mSrc, mShow, IDC_PIC);
	Mat tmp = mShow.clone();
	DrawCross(tmp);
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

}


void CLabelMeWinDlg::OnBnClickedBtnZoomDown()
{
	// TODO: ��С
	mfScalor -= SCALE_STEP;
	if (mfScalor < 1.0f)
	{
		mfScalor = 1.0f;
		return;
	}
	if (std::abs(mfScalor - 1.0f) < 0.0001)
	{
		mfScalor = 1.0f;
		mCurrentSrcSize.x = mSrc.cols;
		mCurrentSrcSize.y = mSrc.rows;
		mCurrentSrcRoi = { 0, 0, mSrc.cols, mSrc.rows };
		mptCurrentOrigin.x = mSrc.cols / 2;
		mptCurrentOrigin.y = mSrc.rows / 2;
		MakeShowingImage(mSrc, mShow, IDC_PIC);
		Mat tmp = mShow.clone();
		DrawCross(tmp);
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
		return;
	}
	//
	MakeScaleImage(mSrc, mShow, IDC_PIC);
	Mat tmp = mShow.clone();
	DrawCross(tmp);
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
}

void CLabelMeWinDlg::DrawCross(cv::Mat src)
{
	auto color = cv::Scalar{ 255,255,255 };
	//�Ź���
	int h3 = src.rows / 3;
	int w3 = src.cols / 3;
	line(src, { 0,h3 }, { src.cols - 1, h3 }, color, 1);
	line(src, { 0,h3 * 2 }, { src.cols - 1, h3 * 2 }, color, 1);
	line(src, { w3, 0 }, { w3, src.rows - 1 }, color, 1);
	line(src, { w3 * 2, 0 }, { w3 * 2, src.rows - 1 }, color, 1);

}

int CLabelMeWinDlg::MakeScaleImage(cv::Mat& src, cv::Mat& dst, UINT id)
{
	// ����ͼƬ
	if (src.empty())
		return -1;
	CWnd* pwnd = GetDlgItem(id);
	CRect rect;
	pwnd->GetClientRect(rect);


	//���Ŷ�Ӧ��ԭͼ��λ��
	int sh = src.rows * 1.0f / mfScalor;
	int sw = src.cols * 1.0f / mfScalor;

	float ratio_h = sh * 1.0 / rect.Height();
	float ratio_w = sw * 1.0 / rect.Width();

	int h, w;
	if (ratio_h > ratio_w)
	{
		//��H��ѹ����������
		h = rect.bottom;
		w = sw / ratio_h;
	}
	else
	{
		w = rect.right;
		h = sh / ratio_w;
	}

	//w,h�������յ�Ҫ���ŵĳߴ�
	//�ȼ���src�е�roi
	int x0 = (src.cols - sw) / 2 - 1;
	int y0 = (src.rows - sh) / 2 - 1;
	int dx = src.cols / 2 - mptCurrentOrigin.x;
	int dy = src.rows / 2 - mptCurrentOrigin.y;
	x0 -= dx;
	y0 -= dy;
	cv::Mat mroi = cv::Mat::zeros({ sw, sh }, CV_8UC3);

	cv::Rect srcRoi{ x0,y0,sw, sh };
	cv::Rect dstRoi{ 0,0,sw, sh };

	mCurrentSrcSize.x = sw;
	mCurrentSrcSize.y = sh;
	mCurrentSrcRoi = srcRoi;

	if (srcRoi.x < 0)
	{
		dstRoi.x = -srcRoi.x;
		srcRoi.x = 0;
	}
	if (srcRoi.y < 0)
	{
		dstRoi.y = -srcRoi.y;
		srcRoi.y = 0;
	}
	if (srcRoi.x + srcRoi.width > mSrc.cols)
	{
		srcRoi.width = mSrc.cols - 1 - srcRoi.x;
		dstRoi.width = srcRoi.width;
	}
	if (srcRoi.y + srcRoi.height > mSrc.rows)
	{
		srcRoi.height = mSrc.rows - 1 - srcRoi.y;
		dstRoi.height = srcRoi.height;
	}

	if (dstRoi.x + dstRoi.width > sw)
	{
		dstRoi.width = sw - dstRoi.x - 1;
		srcRoi.width = dstRoi.width;
	}

	if (dstRoi.y + dstRoi.height > sh)
	{
		dstRoi.height = sh - dstRoi.y - 1;
		srcRoi.height = dstRoi.height;
	}


	if (srcRoi.width <= 0 || srcRoi.height <= 0 || srcRoi.x < 0 || srcRoi.y < 0 ||
		dstRoi.width <= 0 || dstRoi.height <= 0 || dstRoi.x < 0 || dstRoi.y < 0)
	{
		return -1;
	}

	src(srcRoi).copyTo(mroi(dstRoi));
	

	resize(mroi, dst, Size(w, h), 0.0, 0.0, INTER_CUBIC);
	if (dst.channels() == 1)
		cvtColor(dst, dst, COLOR_GRAY2BGR);

	CString msg;
	msg.Format(_T("%d"), int(mfScalor * 100));
	msg += _T("%");
	mStatusBar.SetText(msg, 3, 0);
	msg.Format(_T("��ǰ: (%d, %d, %d, %d)"), mCurrentSrcRoi.x, mCurrentSrcRoi.y, mCurrentSrcRoi.width, mCurrentSrcRoi.height);
	mStatusBar.SetText(msg, 2, 0);

	return 0;
}

#pragma endregion
