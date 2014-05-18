/*---------------------------------------------------------*
 * Copyright (C),
 * FileName:    jpeg.h
 * Author:      meifj
 * Version:             
* Datetime:     2011-10-30	create 	by meifj
				2012-5-8	modification	by meifj
				2013-5-10	modification	by meifj
				2013-7-7	modification	by meifj
				2014-03-10  modification	by meifj 
							ʹ��libjpeg8�����ӱ��빦��
 * Description:         ����libjpeg 
 * Version information: �Ż��������װ�����ӹ���
 *-----------------------------------------------------------*/
#ifndef _CHIRD_JPEG_H_
#define _CHIRD_JPEG_H_

#include <stdio.h>
#include <jpeglib.h>

typedef struct jpeg_info{
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int pixel;
	unsigned int pixel_num;
	unsigned char  * buf;
	unsigned long size;
}CHIRD_JPEG_ENC_S;

typedef struct jpeg_
{
	unsigned int   w;
	unsigned int   h;
	unsigned char * buf;
}CHIRD_JPEG_DEC_S;


	
/*	
	jpegͼƬ���룬��ѹ���������jp->buf��
	ͼƬ��Ϣ���Զ���䵽pj��
	���������Ҫ����jpeg_release()����
*/
#if 0
int jpeg_decodebyfile(struct jpeg_info * pj, const char * filename);
int jpeg_decodebybuffer(struct jpeg_info * pj, unsigned char * buffer, unsigned int size);
int jpeg_showinfo(struct jpeg_info *pj);
/*
	jpegͼƬ���룬��YUV420(YV12)�ɱ�
	����ǰ��Ҫ��pj��width��height�������
	������������jp->buf�У���С��jp->size��
	���������Ҫ����jpeg_release()����
*/
int jpeg_encodetofile(struct jpeg_info * pj, int quality, const char * filename);
int jpeg_encodetobuffer(struct jpeg_info * pj, int quality, char * yuv420);
#endif

int chird_jpeg_decbybuffer(const void * jpegbuf, void * rgb24, int size);
int chird_jpeg_decbyfile(const char * filename, void * rgb24);

int chird_jpeg_showbybuffer(const void * jpegbuf, int size);
int chird_jpeg_showbyfile(const char * filename);



//int chird_jpeg_encotobuffer(const char * yv12, 
//							void * jpegbuf, 
//							int w, int h,  unsigned long * psize,
//							int quality);

int chird_jpeg_encotofile(const char * yv12, 
							const char * filename, 
							int w, int h,
							int quality);



/*
	��ɫת����������
*/
int jpeg_yuyv_to_yv12(unsigned char * dest,const unsigned char * src, int w, int h);

#endif
