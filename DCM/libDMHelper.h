#pragma once
#ifndef _DM_API
#define DM_API __declspec(dllimport)
#else
#define DM_API __declspec(dllexport)
#endif
#define DM_CALL __stdcall

#define DM_OK 0x00
#define DM_FAILE 0x01
#define DM_WRONGPARAM 0x02

/*图像结构体定义*/
typedef struct _tDiImage
{
	void* data;
	int width;
	int height;
	int step;
	int channels;
}TDiImage;

/*病人信息结构定义*/
typedef struct _tDiInfo
{
	char name[1024];
	char sex[1024];
	char birthDate[1024];
	char studyDate[1024];
	char studyTime[1024];
}TDiInfo;


/*DICOM文件读取类*/
class IDiReader
{
public:
	IDiReader();
	virtual ~IDiReader();

	/*加载DICOM文件到内存*/
	virtual int LoadDCMFile(const char* path) = 0;

	/*获取现在的窗宽窗位*/
	virtual int GetWindow(double* ww, double* wl) = 0;

	/*设置窗宽窗位*/
	virtual int SetWindow(double ww, double wl) = 0;

	/*获取解析后的图像*/
	virtual int Decode2Img(TDiImage** ppImg, int* len) = 0;
};

/*创建读取类的一个对象*/
extern "C" DM_API IDiReader* DM_CALL GetDiReaderObj();

/*销毁DiReader类对象*/
extern "C" DM_API void DM_CALL DestroyDiReaderObj(IDiReader** ppReader);

/*销毁 TDiImage 对象*/
extern "C" DM_API void DM_CALL ReleaseDiImage(TDiImage* ppImg, int len);


/*将图像保存成DCM文件*/
extern "C" DM_API int DM_CALL DiImage2File(TDiImage* pimg, TDiInfo* info, const char* path);










