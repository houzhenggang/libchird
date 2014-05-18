//2012��11��25��, PM 10:46:01 by mfj
#ifndef _CHIRD_ALSA1_H_
#define _CHIRD_ALSA1_H_

#include <alsa/asoundlib.h>

//��������
typedef enum{
	ALSA_STREAM_TYPE_PLAY = 0,			//����
	ALSA_STREAM_TYPE_SAMPLE = 1,		//¼��
}CHIRD_ALSA_STREAM_TYPE_E;

//�ɼ�Ƶ��
typedef enum{
	CHIRD_ALSA_RATE_8K = 8000,
	CHIRD_ALSA_RATE_16K = 16000,
	CHIRD_ALSA_RATE_44_1K = 44100,
	CHIRD_ALSA_RATE_48K = 48000,
}CHIRD_ALSA_RATE_E;

//ͨ����
typedef enum{
	CHIRD_ALSA_CHN_SINGLE = 1,		//��ͨ��
	CHIRD_ALSA_CHN_STEREO = 2,		//������
}CHIRD_ALSA_CHN_E;

typedef struct chird_alsa_s
{
	int card;						//������� 	ex:pcmC0D1p  card=0, device=1
	int device;						//�豸��
	CHIRD_ALSA_STREAM_TYPE_E stream;
	CHIRD_ALSA_RATE_E rate;
	CHIRD_ALSA_CHN_E channel;
}CHIRD_ALSA_S;

/*
	alsa��Ƶ��ʼ��	Ĭ��Ϊ16λС������
	����ֵ:   ��Ƶ���
	dev	  :   �豸���� Ĭ��Ϊdefault		plugin:0:0
	stream:   ���š�¼��  0Ϊ���ţ�1Ϊ¼��
	rate  :	  ����Ƶ�ʣ� ����ѡ�������Ч��Ƶ��
	channel:  ��Ƶͨ��
*/
void * chird_alsa_init(const CHIRD_ALSA_S *pstalsa);

int chird_alsa_uninit(void * handle);

int chird_alsa_sample(void * handle, void * buffer, unsigned long length);

int chird_alsa_playback(void * handle, void * buffer, unsigned long length);


#endif
