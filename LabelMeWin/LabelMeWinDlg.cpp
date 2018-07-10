
// LabelMeWinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LabelMeWin.h"
#include "LabelMeWinDlg.h"
#include "afxdialogex.h"
#include "BrowseDir.h"
#include "DlgAddedLabel.h"
#include <fstream>
#include <rapidjson\document.h>
using namespace cv;

//最多放大4倍速
#define MAX_SCALE_RATIO 4

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabelMeWinDlg 对话框



CLabelMeWinDlg::CLabelMeWinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LABELMEWIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_FRAME);
	mCurrentPolyIdx = -1;

	mWheelDelta = 0;
	mScaleRatio = 0;
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
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
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

	mBtnLoadLabel.SetIcon(IDI_ICON_LABELS);
	mBtnLoadLabel.SetAlign(CButtonST::ST_ALIGN_VERT);
	mBtnLoadLabel.SetTooltipText(_T("加载标签文件"));

	//文件列表
	CRect rect;
	mListFiles.GetClientRect(rect);
	mListFiles.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int lw = rect.Width() / 15;
	mListFiles.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListFiles.InsertColumn(1, _T("序号"), LVCFMT_LEFT, lw * 3);
	mListFiles.InsertColumn(2, _T("名称"), LVCFMT_LEFT, lw * 11);

	//标签列表
	mListLabels.GetClientRect(rect);
	mListLabels.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lw = rect.Width() / 15;
	mListLabels.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListLabels.InsertColumn(1, _T("序号"), LVCFMT_LEFT, lw * 3);
	mListLabels.InsertColumn(2, _T("名称"), LVCFMT_LEFT, lw * 11);

	//当前标签列表
	mListROIs.GetClientRect(rect);
	mListROIs.SetExtendedStyle(mListFiles.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lw = rect.Width() / 15;
	mListROIs.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	mListROIs.InsertColumn(1, _T("序号"), LVCFMT_LEFT, lw * 3);
	mListROIs.InsertColumn(2, _T("标签"), LVCFMT_LEFT, lw * 11);

	//自动保存
	mbAutoSave = true;
	((CButton *)GetDlgItem(IDC_CHECK_AUTOSAVE))->SetCheck(1);

	mnCreateOrEdit = 0;
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(TRUE);

	mbRButtonDown = false;
	mnRButtonState = 0;

	LOGD("Init dialog...");
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



float CLabelMeWinDlg::GetPtDistI2(cv::Point& p1, cv::Point& p2)
{
	float dist = (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
	dist = sqrt(dist);
	return dist;
}

void CLabelMeWinDlg::SaveLabels()
{
	//TODO: 保存标注
	std::string strRoot;
	char* p = cstring_to_char(mRootDir);
	strRoot = p;
	delete[] p;

	// 标签文件
	std::string pathLabel = strRoot + "\\" + "label.txt";
	std::ofstream ofs;
	ofs.open(pathLabel, std::ios::out);
	if (!ofs.is_open())
	{
		MessageBox(_T("请注意：标签文件并未保存，但这并不影响标注文件的保存"));
	}
	else
	{
		for (auto& i : msLabels)
		{
			ofs << i << std::endl;
		}
		ofs.close();
	}
	//标注文件
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
		MessageBox(_T("无法保存该图片的标注文件！"));
	}
	else
	{
		ofs << ss.str() << std::endl;
		ofs.close();
	}
}

cv::Point CLabelMeWinDlg::CanvasPt2SrcPt(cv::Point pt)
{
	if (mSrc.empty())
	{
		return{ -1,-1 };
	}

	//
	Point2f rt;
	Point out;
	if (mScaleRatio == 0)
	{
		rt.x = float(mSrc.cols) / float(mShow.cols);
		rt.y = float(mSrc.rows) / float(mShow.rows);
		out.x = rt.x * pt.x;
		out.y = rt.y * pt.y;
	}
	else
	{
		rt.x = float(mroiScale.width) / float(mShow.cols);
		rt.y = float(mroiScale.height) / float(mShow.rows);
		out.x = mroiScale.x + pt.x * rt.x;
		out.y = mroiScale.y + pt.y * rt.y;
	}

	return out;
}

cv::Point CLabelMeWinDlg::SourcePt2CanvasPt(cv::Point pt)
{
	Point2f rt;
	Point out;
	if (mScaleRatio == 0)
	{
		rt.x = float(mSrc.cols) / float(mShow.cols);
		rt.y = float(mSrc.rows) / float(mShow.rows);
		out.x = pt.x / rt.x;
		out.y = pt.y / rt.y;
	}
	else
	{
		rt.x = float(mroiScale.width) / float(mShow.cols);
		rt.y = float(mroiScale.height) / float(mShow.rows);
		out.x = (pt.x - mroiScale.x) / rt.x;
		out.y = (pt.y - mroiScale.y) / rt.y;
	}
	return out;
}

cv::Point2f CLabelMeWinDlg::GetCurrentScaler()
{
	Point2f rt;
	if (mScaleRatio == 0)
	{
		rt.x = float(mSrc.cols) / float(mShow.cols);
		rt.y = float(mSrc.rows) / float(mShow.rows);
	}
	else
	{
		rt.x = float(mroiScale.width) / float(mShow.cols);
		rt.y = float(mroiScale.height) / float(mShow.rows);
	}
	return rt;
}

void CLabelMeWinDlg::OnBnClickedOk()
{
	// TODO: 这里屏蔽掉enter
	return;
	CDialogEx::OnOK();
}


HBRUSH CLabelMeWinDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  List 控件背景

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


CString CLabelMeWinDlg::SelectFolder()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = _T("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择一个文件夹：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
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

#pragma region 文件列表
void CLabelMeWinDlg::OnNMClickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	// TODO: 查询点击了第几行，并加载相应的图片
	int idx = pNMItemActivate->iItem;
	if (mvFiles.size() <= idx)
		return;

	//



	//保存
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("是否保存当前图片的标注？"), _T("提示"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//保存
				bSave = true;
			}
		}
		else
		{
			//自动保存
			bSave = true;
		}
	}

	if (bSave)
	{
		// 执行保存
		SaveLabels();
	}
	//清空
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

	// 标签
	for (auto& i : mvPolys)
	{
		msLabels.insert(i.first);
	}

	//refresh list
	RefreshLabelLists();
	RefreshROILists();

	//显示 
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

#pragma region 界面按钮
void CLabelMeWinDlg::OnBnClickedBtnOpen()
{
	// TODO: 打开文件对话框
	ENTER_FUNC;
	//1.
	//打开文件
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //是否打开(否则为保存) 
	CString defaultDir = _T("./");   //默认打开的文件路径 
	CString fileName = _T("");         //默认打开的文件名 
	CString filter = _T("图像 (*.bmp; *.jpg; *.png; *.tiff)|*.bmp;*.jpg;*.png;*.tiff||");   //文件过虑的类型 
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (openFileDlg.DoModal() == IDOK)
	{
		filePath = openFileDlg.GetPathName();
	}
	//
	if (filePath == _T(""))
	{
		MessageBox(_T("没有选择文件"));
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

	//读取文件到内存
	LoadImageAndShow();
	RefreshFileLists();
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;

	//查找是否有本文件的标注文件，读入并显示
	FindCurrentLabels();
	LEAVE_FUNC;
}

void CLabelMeWinDlg::OnBnClickedBtnOpenDir()
{
	// TODO: 选择目录
	ENTER_FUNC;
	mRootDir = SelectFolder();
	if (mRootDir == _T(""))
	{
		MessageBox(_T("没有选择文件夹！"));
		return;
	}
	//查询目录下所有的图片
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
		MessageBox(_T("选择的文件夹中没有图片文件！"));
		return;
	}
	mCurrentFile = CString(mvFiles[0].c_str());
	mCurrentIndex = 0;
	//把mvFiles里面的路径，去掉根目录
	for (auto& i : mvFiles)
	{
		auto j = i.substr(strRoot.length() + 1);
		i = j;
	}

	//列表显示图片
	RefreshFileLists();
	mvPolys.clear();
	mvRoi.clear();
	RefreshROILists();

	//加载图片
	LoadImageAndShow();
	FindCurrentLabels();
	//List 
	ItemHighLight(-1, 0, mListFiles);
	mbLButtonDown = false;

	LEAVE_FUNC;
}


void CLabelMeWinDlg::OnBnClickedBtnNextImage()
{
	// TODO: 下一张
	if (mCurrentIndex == mvFiles.size() - 1)
	{
		MessageBox(_T("已经是最后一张了"));
		return;
	}
	//保存
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("是否保存当前图片的标注？"), _T("提示"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//保存
				bSave = true;
			}
		}
		else
		{
			//自动保存
			bSave = true;
		}
	}

	if (bSave)
	{
		// 执行保存
		SaveLabels();
	}
	//清空
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
	// TODO: 前一张
	if (mCurrentIndex == 0)
	{
		MessageBox(_T("已经是第一张了"));
		return;
	}
	//保存
	bool bSave = false;
	if (mvPolys.size() != 0)
	{
		if (!mbAutoSave)
		{
			int ret = MessageBox(_T("是否保存当前图片的标注？"), _T("提示"), MB_OKCANCEL);
			if (ret == IDOK)
			{
				//保存
				bSave = true;
			}
		}
		else
		{
			//自动保存
			bSave = true;
		}
	}

	if (bSave)
	{
		// 执行保存
		SaveLabels();
	}
	//清空
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
	// TODO: 保存
	if (mvPolys.size() > 0)
	{
		// 执行保存
		SaveLabels();
	}
}


void CLabelMeWinDlg::OnBnClickedBtnCreatePoly()
{
	// TODO: 创建多边形
	mnCreateOrEdit = 0;
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(TRUE);
}


void CLabelMeWinDlg::OnBnClickedBtnDeletePoly()
{
	// TODO: 删除多边形
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
	// TODO: 编辑多边形
	mnCreateOrEdit = 1;
	mvRoi.clear();
	mbLButtonDown = false;
	mbDraged = false;
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
	GetDlgItem(IDC_BTN_CREATE_POLY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DELETE_POLY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_EDIT_POLY)->EnableWindow(FALSE);
}
#pragma endregion

#pragma region 图像显示
void CLabelMeWinDlg::LoadImageAndShow()
{
	auto* p = cstring_to_char(mCurrentFile);
	mSrc = imread(p);
	if (mSrc.empty())
	{
		MessageBox(_T("无法读取文件：\r\n") + mCurrentFile);
		return;
	}

	//显示
	MakeShowingImage(mSrc, mShow, IDC_PIC);
	//MakeShowingImage(mSrc, mShowScale, IDC_PIC);
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
	unsigned char* pimg = (unsigned char*)cimg.GetBits(); //获取CImage的像素存贮区的指针 
	int cimg_step = cimg.GetPitch();//每行的字节数,注意这个返回值有正有负

									// 如果是1个通道的图像(灰度图像) DIB格式才需要对调色板设置   
									// CImage中内置了调色板，我们要对他进行赋值： 
	if (1 == channels)
	{
		RGBQUAD* ColorTable;
		int MaxColors = 256;
		//这里可以通过CI.GetMaxColorTableEntries()得到大小(如果你是CI.Load读入图像的话)   
		ColorTable = new RGBQUAD[MaxColors];
		cimg.GetColorTable(0, MaxColors, ColorTable);//这里是取得指针   
		for (int i = 0; i< MaxColors; i++)
		{
			ColorTable[i].rgbBlue = (BYTE)i;
			//BYTE和uchar一回事，但MFC中都用它   
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
	// 缩放图片
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
		//按H的压缩比例计算
		h = rect.bottom;
		w = src.cols / ratio_h;
	}
	else
	{
		w = rect.right;
		h = src.rows / ratio_w;
	}
	mroiScale = { 0,0, mSrc.cols, mSrc.rows };
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
	//计算水平点
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
	//填充背景色
	FillRect(hdc, &rect, CBrush(bkColor));
	image.Draw(hdc, dstRect, CRect(0, 0, image.GetWidth(), image.GetHeight()));
	return;
}

#pragma endregion


#pragma region 鼠标事件
void CLabelMeWinDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 左键按下
	CRect rect, rectRoi;
	if (mShow.empty() || mSrc.empty())
		goto RETURN;

	//判断是否在控件内
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//判断是否在控件内
	rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		goto RETURN;
	}
	mbLButtonDown = true;
	mbDraged = false;
	mptDragOrigin = { -1,-1 };
	//
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ClipCursor(rect);
RETURN:
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLabelMeWinDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 左键弹起
	ClipCursor(NULL);
	if(!mbLButtonDown)
		return CDialogEx::OnLButtonUp(nFlags, point);
	mbLButtonDown = false;
	mptDragOrigin = { -1, -1 };
	//记录下点
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//判断是否在控件内
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		return CDialogEx::OnLButtonDown(nFlags, point);;
	}
	//
	
	if (mbDraged)
	{
		return CDialogEx::OnLButtonDown(nFlags, point);;
	}
		
	mptStart.x = point.x - rectRoi.left;
	mptStart.y = point.y - rectRoi.top;

	mptStart = CanvasPt2SrcPt(mptStart);

	if (mnCreateOrEdit == 1)
	{ //## 编辑模式

		//查看当前点，在哪个Poly里面
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
				//记录下i
				idx = i;
				break;
			}
		}

		if(idx < 0)
			return CDialogEx::OnLButtonUp(nFlags, point);

		//画
		ItemHighLight(mCurrentPolyIdx, idx, mListROIs);
		mCurrentPolyIdx = idx;
		DrawIdxRedPolys(idx);

		// 映射ROI列表
		

		return CDialogEx::OnLButtonUp(nFlags, point);
	}
	
	//## 创建模式
	//保存
	mvRoi.push_back(mptStart);
	if (mvRoi.size() > 4)
	{
		//判断是否闭合
		auto dist = GetPtDistI2(mptStart, mvRoi[0]);
		auto scaler = GetCurrentScaler();
		if (dist < MIN_NEIGBOR * scaler.x)
		{
			//说明是闭合了
			//1. 对话框，选择或输入标签
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
				Mat tmp = mShow.clone();
				DrawPolys(tmp);
				DrawCurrentPoly(tmp);
				ConvertMatToCImage(tmp, mCimg);
				DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
				return CDialogEx::OnLButtonUp(nFlags, point);;
			}

			//2. 保存进行下一个
			auto pa = std::pair<std::string, std::vector<cv::Point>>(std::move(label), std::move(mvRoi));
			mvPolys.emplace_back(std::move(pa));
			//3. 刷新标签列表
			RefreshLabelLists();
			RefreshROILists();
		}
	}


	

	//画一个小圆
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CLabelMeWinDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 鼠标移动
	if (mSrc.empty())
		return CDialogEx::OnMouseMove(nFlags, point);
	if(!mbLButtonDown)
		return CDialogEx::OnMouseMove(nFlags, point);

	//按下鼠标左键，拖动图片
	//记录下点
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//判断是否在控件内
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		return CDialogEx::OnMouseMove(nFlags, point);
	}
	mbDraged = true;
	// 没有缩放，不允许手动
	if (mScaleRatio == 0)
		return CDialogEx::OnMouseMove(nFlags, point);


	Point center;
	center.x = point.x - rectRoi.left;
	center.y = point.y - rectRoi.top;
	center = CanvasPt2SrcPt(center);
	if (mptDragOrigin.x == -1 || mptDragOrigin.y == -1)
	{
		mptDragOrigin = center;
	}
	if (mptDragOrigin == center)
		return CDialogEx::OnMouseMove(nFlags, point);
	//移动点
	int delta_x = center.x - mptDragOrigin.x;
	int delta_y = center.y - mptDragOrigin.y;
	
	if(!(std::abs(delta_x) > 10 || std::abs(delta_y) > 10))
		return CDialogEx::OnMouseMove(nFlags, point);

	mptDragOrigin = center;
	center.x = mroiScale.x + mroiScale.width / 2 - delta_x;
	center.y = mroiScale.y + mroiScale.height / 2 - delta_y;
	
	MakeScaleImage(mSrc, center, mShow, IDC_PIC, mScaleRatio);
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
	
	return CDialogEx::OnMouseMove(nFlags, point);
}

void CLabelMeWinDlg::DrawPolys(cv::Mat& canvas)
{

	for (auto& i : mvPolys)
	{
		auto& v = i.second;
		for (int k = 0; k < v.size(); k++)
		{
			Point  pt = v[k];
			pt = SourcePt2CanvasPt(pt);
			circle(canvas, pt, POINT_CIRCLE_R, COLOR_BLUE, -1);
			if (k == v.size() - 1)
			{
				line(canvas, pt, SourcePt2CanvasPt(v[0]), COLOR_BLUE, POINT_LINE_R);
			}
			else
			{
				line(canvas, pt, SourcePt2CanvasPt(v[k + 1]), COLOR_BLUE, POINT_LINE_R);
			}
		}
	}
}
void CLabelMeWinDlg::DrawCurrentPoly(cv::Mat& canvas)
{
	for (int i = 0; i < mvRoi.size(); i++)
	{
		circle(canvas, SourcePt2CanvasPt(mvRoi[i]), POINT_CIRCLE_R, COLOR_GREEN, -1);
		if (i != mvRoi.size() - 1)
			line(canvas, SourcePt2CanvasPt(mvRoi[i]), SourcePt2CanvasPt(mvRoi[i + 1]), COLOR_GREEN, POINT_LINE_R);
	}
}

#pragma endregion


void CLabelMeWinDlg::OnBnClickedBtnLoadLabel()
{
	// TODO: 加载标签文件
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //是否打开(否则为保存) 
	CString defaultDir = _T("./");   //默认打开的文件路径 
	CString fileName = _T("");         //默认打开的文件名 
	CString filter = _T("普通文件 (*.txt)|*.txt||");   //文件过虑的类型 
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (openFileDlg.DoModal() == IDOK)
	{
		filePath = openFileDlg.GetPathName();
	}
	//
	if (filePath == _T(""))
	{
		MessageBox(_T("没有选择文件"));
		return;
	}
	//按行读入
	std::ifstream ifs;
	char* p = cstring_to_char(filePath);
	ifs.open(p, std::ios::in);
	delete[] p;
	if (!ifs.is_open())
	{
		MessageBox(_T("打开标签文件失败！"));
		return;
	}
	std::string line;
	while (getline(ifs, line))
	{
		msLabels.insert(line);
	}
	ifs.close();

	//刷新标签列表
	RefreshLabelLists();
}


void CLabelMeWinDlg::OnBnClickedCheckAutosave()
{
	// TODO: 自动保存?
	if (((CButton *)GetDlgItem(IDC_CHECK_AUTOSAVE))->GetCheck() == 1)
		mbAutoSave = true;
	else
		mbAutoSave = false;
}


void CLabelMeWinDlg::OnNMClickListPolys(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 红色显示当前选中的行
	*pResult = 0;

	//
	int index = pNMItemActivate->iItem;
	if (index > mvPolys.size() - 1)
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

#pragma region 滚动及右键放大

BOOL CLabelMeWinDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(mSrc.empty())
		return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
	

	//记录下点
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	//ScreenToClient(rect);
	//判断是否在控件内
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (pt.x < rectRoi.left || pt.x > rectRoi.right || pt.y < rectRoi.top || pt.y > rectRoi.bottom)
	{
		return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
	}


	mWheelDelta += zDelta;
	if (mWheelDelta >= 120 || mWheelDelta <= -120)
	{
		//1. 记录点，转换点
		cv::Point cvpt;
		cvpt.x = pt.x - rectRoi.left;
		cvpt.y = pt.y - rectRoi.top;

		int delta = mWheelDelta > 0 ? 2 : -2;
		mWheelDelta = 0;
		if (mScaleRatio + delta > MAX_SCALE_RATIO ||
			mScaleRatio + delta < 0)
		{
			return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
		}

		// 放大或是缩小
		mScaleRatio += delta;
		if (mScaleRatio == 0)
		{
			MakeShowingImage(mSrc, mShow, IDC_PIC);
		}
		else
		{
			//float x_scale = float(mSrc.cols) / float(mShow.cols);
			//float y_scale = float(mSrc.rows) / float(mShow.rows);
			//cvpt.x *= x_scale;
			//cvpt.y *= y_scale;
			cvpt = CanvasPt2SrcPt(cvpt);
			MakeScaleImage(mSrc, cvpt, mShow, IDC_PIC, mScaleRatio);
		}

		//画标记
		Mat tmp = mShow.clone();
		DrawPolys(tmp);
		DrawCurrentPoly(tmp);
		ConvertMatToCImage(tmp, mCimg);
		DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);
		
		
	}
	
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CLabelMeWinDlg::MakeScaleImage(cv::Mat& src, cv::Point& center, cv::Mat& dst, UINT id, int scale_factor)
{
	// 缩放图片
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
		//按H的压缩比例计算
		h = rect.bottom;
		w = src.cols / ratio_h;
	}
	else
	{
		w = rect.right;
		h = src.rows / ratio_w;
	}
	//w,h就是最终的比例
	Rect r;
	if (scale_factor == 2)
	{
		r.x = center.x - w;
		r.y = center.y - h;
		r.width = w * 2;
		r.height = h * 2;
	}
	else if (scale_factor == 4)
	{
		r.x = center.x - w / 2;
		r.y = center.y - h / 2;
		r.width = w;
		r.height = h;
	}
	
	if (r.x < 0)
		r.x = 0;
	if (r.y < 0)
		r.y = 0;
	if (r.x + r.width > src.cols - 1)
		r.width = src.cols - r.x - 1;
	if (r.y + r.height > src.rows - 1)
		r.height = src.rows - r.y - 1;
	mroiScale = r;
	Mat roi = src(r);
	resize(roi, dst, Size(w, h), 0.0, 0.0, INTER_CUBIC);
	if (dst.channels() == 1)
		cvtColor(dst, dst, COLOR_GRAY2BGR);
}

void CLabelMeWinDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// TODO: 左键按下
	CRect rect, rectRoi;
	if (mShow.empty() || mSrc.empty())
		goto RETURN;

	//判断是否在控件内
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//判断是否在控件内
	rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		goto RETURN;
	}

	mbRButtonDown = true;
	//
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ClipCursor(rect);
RETURN:
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CLabelMeWinDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ClipCursor(NULL);
	if (!mbRButtonDown)
		return CDialogEx::OnRButtonUp(nFlags, point);
	mbRButtonDown = false;
	//记录下点
	CRect rect;
	GetDlgItem(IDC_PIC)->GetWindowRect(rect);
	ScreenToClient(rect);
	//判断是否在控件内
	CRect rectRoi = mRectShow + POINT{ rect.left, rect.top };
	if (point.x < rectRoi.left || point.x > rectRoi.right || point.y < rectRoi.top || point.y > rectRoi.bottom)
	{
		return CDialogEx::OnLButtonDown(nFlags, point);;
	}
	//
	cv::Point cvpt;
	cvpt.x = point.x - rectRoi.left;
	cvpt.y = point.y - rectRoi.top;

	mnRButtonState++;
	if (mnRButtonState == 3)
		mnRButtonState = 0;
	if (mnRButtonState == 0)
	{
		mScaleRatio = 0;
		MakeShowingImage(mSrc, mShow, IDC_PIC);
	}
	else
	{
		cvpt = CanvasPt2SrcPt(cvpt);
		mScaleRatio += 2;
		MakeScaleImage(mSrc, cvpt, mShow, IDC_PIC, mScaleRatio);
	}

	//画标记
	Mat tmp = mShow.clone();
	DrawPolys(tmp);
	DrawCurrentPoly(tmp);
	ConvertMatToCImage(tmp, mCimg);
	DrawCImageCenter(mCimg, GetDlgItem(IDC_PIC), mRectShow);

	CDialogEx::OnRButtonUp(nFlags, point);
}

#pragma endregion

BOOL CLabelMeWinDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//捕获ESC事件

			//
			if (!mbLButtonDown && !mbRButtonDown && !mSrc.empty())
			{
				//如果鼠标不是在按下状态，并且已经加载了图片
				//那么，取消最后一个点的编辑
				//画一个小圆
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

			//取消默认关闭窗体事件
			return TRUE;
		}
	}



	return CDialogEx::PreTranslateMessage(pMsg);
}
