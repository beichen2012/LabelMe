
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