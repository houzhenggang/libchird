
#ifndef __CHIRD_VIDEO_H__
#define __CHIRD_VIDEO_H__

// c���Բ����Ե���C++�ĺ�����C++�ܷ����C�ĺ���
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <linux/videodev2.h>

#define debug  printf

#define FORMAT_MJPEG		V4L2_PIX_FMT_MJPEG
#define FORMAT_YUYV 		V4L2_PIX_FMT_YUYV

typedef struct chird_video
{
	char * devname;
	int  framerate;			//����, ʵ�ʷ�����ӽ�������
	int  width;				//���, ʵ�ʷ�����ӽ��Ŀ��
	int  height;			//�߶�, ʵ�ʷ�����ӽ��ĸ߶�
	int  format;
	void (*pfun)(const char * videodata, int length, void * param);
	void * param;	
	int framecount;			//��Ҫ�������ռ�
}CHIRD_VIDEO_S;	


//���ܣ���/dev����Ѱ����û��video�豸������з����豸�������û�з���NULL
//valididx ��ȡ�ڼ�����Ч��Ƶ�豸, һ��valididxѡ0����
char * chird_video_devscan(int valididx);

//����: ��Ƶ��ʼ��, ������Ƶ���
void * chird_video_init(CHIRD_VIDEO_S * pst);

//����: ��Ƶ����ʼ��
int chird_video_uninit(void * handle);

//����: ������Ƶ�ɼ�
int chird_video_start(void * handle);

//����: �ر���Ƶ�ɼ�
int chird_video_end(void * handle);

//����: ��ȡ��Ƶ����
int chird_video_showparam(void * handle);

//����: ��ȡ����
int chird_video_getframecount(void * handle);

//����: �������
int chird_video_clearframecount(void * handle);

// ��ѯ��Ƶ��Ϣ
int chird_video_query_info(void * handle);

// ɨ����Ƶ֧�ָ�ʽ
int chird_video_scan_support(void * handle);


	
#ifdef __cplusplus
}
#endif

#endif


