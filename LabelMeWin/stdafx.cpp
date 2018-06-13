
// stdafx.cpp : 只包括标准包含文件的源文件
// LabelMeWin.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


char* cstring_to_char(CString& str)
{
	int n = str.GetLength();     //
								 //获取宽字节字符的大小，大小是按字节计算的
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char * pFileName = new char[len + 1];   //以字节为单位
											//宽字节编码转换成多字节编码
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), pFileName, len, NULL, NULL);
	pFileName[len] = '\0';   //多字节字符以'\0'结束

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