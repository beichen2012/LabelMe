
// LabelMeWin.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLabelMeWinApp: 
// �йش����ʵ�֣������ LabelMeWin.cpp
//

class CLabelMeWinApp : public CWinApp
{
public:
	CLabelMeWinApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLabelMeWinApp theApp;