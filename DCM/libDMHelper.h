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

/*ͼ��ṹ�嶨��*/
typedef struct _tDiImage
{
	void* data;
	int width;
	int height;
	int step;
	int channels;
}TDiImage;

/*������Ϣ�ṹ����*/
typedef struct _tDiInfo
{
	char name[1024];
	char sex[1024];
	char birthDate[1024];
	char studyDate[1024];
	char studyTime[1024];
}TDiInfo;


/*DICOM�ļ���ȡ��*/
class IDiReader
{
public:
	IDiReader();
	virtual ~IDiReader();

	/*����DICOM�ļ����ڴ�*/
	virtual int LoadDCMFile(const char* path) = 0;

	/*��ȡ���ڵĴ���λ*/
	virtual int GetWindow(double* ww, double* wl) = 0;

	/*���ô���λ*/
	virtual int SetWindow(double ww, double wl) = 0;

	/*��ȡ�������ͼ��*/
	virtual int Decode2Img(TDiImage** ppImg, int* len) = 0;
};

/*������ȡ���һ������*/
extern "C" DM_API IDiReader* DM_CALL GetDiReaderObj();

/*����DiReader�����*/
extern "C" DM_API void DM_CALL DestroyDiReaderObj(IDiReader** ppReader);

/*���� TDiImage ����*/
extern "C" DM_API void DM_CALL ReleaseDiImage(TDiImage* ppImg, int len);


/*��ͼ�񱣴��DCM�ļ�*/
extern "C" DM_API int DM_CALL DiImage2File(TDiImage* pimg, TDiInfo* info, const char* path);










