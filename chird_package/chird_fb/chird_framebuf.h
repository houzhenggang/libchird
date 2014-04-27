/*-----------------------------------------------------------------*
 * Copyright (C),
 * FileName:            frambuffer.c
 * Author:             	meifj
 * Version:             
 * Datetime:           	2011-10-30	create 	by meifj
 						2012-5-8	modification	by meifj
 						2013-5-10	modification	by meifj
 						2013-7-7	modification	by meifj
 * Description:         ����frambuffer for linux��ʾ 
 * Version information: �Ż��������װ�����ӹ���
 *-----------------------------------------------------------------*/
#ifndef _CHIRD_FRAME_BUF_H
#define _CHIRD_FRAME_BUF_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

/* �����Դ� */
int chird_fb_create(int index, int x, int y, int w, int h, int pixel);

/* �����Դ� */
int chird_fb_destroy(int index);

/* �Դ���� */
int chird_fb_clear(int index, int color);

/* ��ӡ��Ļ�̶���Ϣ*/
int chird_fb_showscreenfix(int idx);

/* ��ӡ��Ļ�ɱ���Ϣ */
int chird_fb_showscreenvar(int idx);

#endif

