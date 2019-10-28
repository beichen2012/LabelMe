
// LabelMeWinDlg.h : ͷ�ļ�
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
#include <libDMHelper.h>

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
	//for dcm window pos and window width 
	int pos;
	int width;
}TImgAnno;
REFLECTION(TImgAnno, label, pts, pos, width);

typedef struct _ImgAnnoInfo
{
	std::vector<TImgAnno> va;
	int width;
	int height;
}TAnnoInfo;
REFLECTION(TAnnoInfo, va, width, height);


// CLabelMeWinDlg �Ի���
class CLabelMeWinDlg : public CDialogEx
{
// ����
public:
	CLabelMeWinDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LABELMEWIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//�ļ�·��
	CString mCurrentFile;
	std::vector<std::string> mvFiles;
	int mCurrentIndex;
	CString mRootDir;
	bool mbAutoSave;

	//��ǩ
	std::set<std::string> msLabels;
	
	//ͼ����ʾ
	cv::Mat mSrc;
	cv::Mat mShow;
	CImage mCimg;
	CRect mRectShow;

	//
	bool mbLButtonDown;
	cv::Point2f mptStart;
	cv::Point2f mptEnd;
	std::vector<cv::Point2f> mvRoi;
	std::vector<std::pair<std::string, std::vector<cv::Point2f>>> mvPolys;
	std::vector<std::pair<int, int>> mvWindows;
	std::map<std::string, int> mmapColor;

	//��ʶ��ǰ��contours ����
	int mCurrentPolyIdx;

	//��ʶ��ǰ��ģʽ���༭������λ��Ǿ���
	int mnStatusIndicator; 

public:

	CString SelectFolder();
	//���ҵ�ǰ·���µ��ļ��Ƿ��б�ע�ļ�
	void FindCurrentLabels();
	void RefreshFileLists();
	void RefreshLabelLists();
	void RefreshROILists();
	void ItemHighLight(int idx, CListCtrl& list);

	//ͼƬ��ʾ
	int LoadImageAndShow();
	void ConvertMatToCImage(cv::Mat & src, CImage & cimg);
	void MakeShowingImage(cv::Mat & src, cv::Mat & dst, UINT id);
	void DrawCImageCenter(ATL::CImage& image, CWnd* pwnd, CRect& dstRect, COLORREF bkColor = RGB(105, 105, 105));
	void DrawPolys(cv::Mat& canvas);
	void DrawCurrentPoly(cv::Mat& canvas);
	void DrawIdxRedPolys(int idx);

	//
	float GetPtDistI2(cv::Point2f& p1, cv::Point2f& p2);

	void SaveLabels();


	cv::Point2f CanvasPt2SrcPt(cv::Point2f pt);
	cv::Point2f SourcePt2CanvasPt(cv::Point2f pt);
	cv::Point2f GetCurrentScaler();



	/*����ʹ��*/
	bool mbZoom;
	float mfScalor;
	void DrawCross(cv::Mat src);
	cv::Point mptButtonDown;
	cv::Point2f mptCurrentOrigin;
	int MakeScaleImage(cv::Mat& src, cv::Mat& dst, UINT id);

	cv::Point mCurrentSrcSize;
	cv::Rect mCurrentSrcRoi;


	//״̬��
	CStatusBarCtrl mStatusBar;

	//FOR DCM FILES
	std::shared_ptr<IDiReader> mpDCMReader;

	double mDCMWidth;
	double mDCMPos;

	void ShowPosAndWidth();
	void SetVisableWindow(BOOL flag);
	void SetWindowPosControl();
	bool mbDCM;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnOpen();
	CButtonST mBtnOpen;
	CButtonST mBtnOpenDir;
	CButtonST mBtnNextImage;
	CButtonST mBtnPrevImage;
	CButtonST mBtnSave;
	CButtonST mBtnCreatePoly;
	CButtonST mBtnCreateRect;
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
	void ClickedNextImpl(int selfHightlight = 1);
	void ClickedPrevImpl(int selfHightlight = 1);
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkListPolys(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckZoom();
	afx_msg void OnBnClickedBtnZoomOrigin();
	afx_msg void OnBnClickedBtnZoomUp();
	afx_msg void OnBnClickedBtnZoomDown();
	
	afx_msg void OnBnClickedBtnCreateRect();
	CSliderCtrl mSliderWindow;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioWindowPos();
	afx_msg void OnBnClickedRadioWindowWidth();
	afx_msg void OnBnClickedCheckShow();
	afx_msg void OnBnClickedBtnDeleteFile();
	void DeleteFileImpl(std::string DELETE_PATH);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnDeleteFile2();
	afx_msg void OnBnClickedBtnDeleteFile3();
	afx_msg void OnClose();
};
