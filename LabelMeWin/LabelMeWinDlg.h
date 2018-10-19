
// LabelMeWinDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "BtnST.h"
#include "afxcmn.h"
#include <vector>
#include <string>
#include <utility>
#include <set>
#include <opencv2/opencv.hpp>
#include <iguana/json.hpp>
#include "spdhelper.hpp"

#define MIN_NEIGBOR 5
#define POINT_CIRCLE_R 2
#define POINT_LINE_R 1

#define COLOR_GREEN (cv::Scalar(0,255,0))
#define COLOR_BLUE (cv::Scalar(255,0,0))
#define COLOR_RED (cv::Scalar(0,0,255))

typedef struct _tPtAnno
{
	int x;
	int y;
}TPtAnno;
REFLECTION(TPtAnno, x, y);

typedef struct _tImgAnno
{
	std::string label;
	std::vector<TPtAnno> pts;
}TImgAnno;
REFLECTION(TImgAnno, label, pts);

typedef struct _ImgAnnoInfo
{
	std::vector<TImgAnno> va;
	float x_scale;	//无用
	float y_scale;	//无用
	//std::string imageName;
}TAnnoInfo;
REFLECTION(TAnnoInfo, va, x_scale, y_scale/*, imageName*/);


// CLabelMeWinDlg 对话框
class CLabelMeWinDlg : public CDialogEx
{
// 构造
public:
	CLabelMeWinDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LABELMEWIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//文件路径
	CString mCurrentFile;
	std::vector<std::string> mvFiles;
	int mCurrentIndex;
	CString mRootDir;
	bool mbAutoSave;

	//标签
	std::set<std::string> msLabels;
	
	//图像显示
	cv::Mat mSrc;
	cv::Mat mShow;
	CImage mCimg;
	CRect mRectShow;

	//
	bool mbLButtonDown;
	cv::Point mptStart;
	cv::Point mptEnd;
	std::vector<cv::Point> mvRoi;
	std::vector<std::pair<std::string, std::vector<cv::Point>>> mvPolys;
	int mCurrentPolyIdx;

	int mnCreateOrEdit; //0 for create ; 1 for edit

	//右键
	bool mbRButtonDown;
	int mnRButtonState;

public:

	CString SelectFolder();
	//查找当前路径下的文件是否有标注文件
	void FindCurrentLabels();
	void RefreshFileLists();
	void RefreshLabelLists();
	void RefreshROILists();
	void ItemHighLight(int idx_no, int idx_yes, CListCtrl& list);

	//图片显示
	void LoadImageAndShow();
	void ConvertMatToCImage(cv::Mat & src, CImage & cimg);
	void MakeShowingImage(cv::Mat & src, cv::Mat & dst, UINT id);
	void DrawCImageCenter(ATL::CImage& image, CWnd* pwnd, CRect& dstRect, COLORREF bkColor = RGB(105, 105, 105));
	void DrawPolys(cv::Mat& canvas);
	void DrawCurrentPoly(cv::Mat& canvas);
	void DrawIdxRedPolys(int idx);

	//
	float GetPtDistI2(cv::Point& p1, cv::Point& p2);

	void SaveLabels();


	cv::Point CanvasPt2SrcPt(cv::Point pt);
	cv::Point SourcePt2CanvasPt(cv::Point pt);
	cv::Point2f GetCurrentScaler();



	/*缩放使能*/
	bool mbZoom;
	float mfScalor;
	void DrawCross(cv::Mat src);
	cv::Point mptButtonDown;
	cv::Point mptCurrentOrigin;
	
	cv::Mat CalcOffsetMat();
	int MakeScaleImage(cv::Mat& src, cv::Mat& dst, UINT id);

	cv::Point mCurrentSrcSize;
	cv::Rect mCurrentSrcRoi;


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnOpen();
	CButtonST mBtnOpen;
	CButtonST mBtnOpenDir;
	CButtonST mBtnNextImage;
	CButtonST mBtnPrevImage;
	CButtonST mBtnSave;
	CButtonST mBtnCreatePoly;
	CButtonST mBtnDeletePoly;
	CButtonST mBtnEditPoly;
	CButtonST mBtnLoadLabel;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CListCtrl mListFiles;
	CListCtrl mListROIs;
	CListCtrl mListLabels;
	afx_msg void OnBnClickedBtnOpenDir();
	afx_msg void OnBnClickedBtnNextImage();
	afx_msg void OnBnClickedBtnPrevImage();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnCreatePoly();
	afx_msg void OnBnClickedBtnDeletePoly();
	afx_msg void OnBnClickedBtnEditPoly();
	afx_msg void OnNMClickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnLoadLabel();
	afx_msg void OnBnClickedCheckAutosave();
	afx_msg void OnNMClickListPolys(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkListPolys(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckZoom();
	afx_msg void OnBnClickedBtnZoomOrigin();
	afx_msg void OnBnClickedBtnZoomUp();
	afx_msg void OnBnClickedBtnZoomDown();
};
