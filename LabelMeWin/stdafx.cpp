
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// LabelMeWin.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


char* cstring_to_char(CString& str)
{
	int n = str.GetLength();     //
								 //��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
	char * pFileName = new char[len + 1];   //���ֽ�Ϊ��λ
											//���ֽڱ���ת���ɶ��ֽڱ���
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), pFileName, len, NULL, NULL);
	pFileName[len] = '\0';   //���ֽ��ַ���'\0'����

	return pFileName;
}

char* ReadWholeFile(const char* path)
{
	FILE *pFile = fopen(path, "r");
	if (pFile == NULL)
		return nullptr;
	char *pBuf;
	fseek(pFile, 0, SEEK_END);
	int len = ftell(pFile);
	pBuf = new char[len + 1];
	memset(pBuf, 0, len + 1);
	rewind(pFile);
	fread(pBuf, 1, len, pFile);
	pBuf[len] = 0;
	fclose(pFile);
	return pBuf;
}


#ifdef _DEBUG
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_imgproc320.lib")
#pragma comment(lib, "opencv_highgui320.lib")
#else
#pragma comment(lib, "opencv_core341.lib")
#pragma comment(lib, "opencv_imgcodecs341.lib")
#pragma comment(lib, "opencv_imgproc341.lib")
#pragma comment(lib, "opencv_highgui341.lib")
#endif