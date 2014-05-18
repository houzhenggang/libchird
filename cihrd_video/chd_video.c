

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "chd_video.h"

#define CHIRD_VIDEO_MAGIC 0x11223344


// ��Ƶ��ؽṹ��
typedef struct _chird_video_info_s
{
	int  magic;			// ����
	int  videofd;		// ��Ƶ�豸������
	char devname[12];	// �豸��
	int  framerate;		//����
	int  width;			// ͼ���
	int  height;		// ͼ���
	int  format;

	sem_t start_sync;	// ��ʼ�ɼ�ͬ��
	sem_t end_sync;		// �����ɼ�ͬ��
	
	pthread_t tid;		// ���ݴ����߳�
	int threadsample;	// �ɼ����
	int threadrun;		// �߳����еı�ǣ� 1��ʾ���У� 0��ʾ�˳�

	
	void (*pfun)(const char * , int , void * );  //�ص�����
	void * param;								 //�ص�����

	struct v4l2_buffer videobuf;				//V4L2 ����ṹ��
	int framecount;								//����
	
	struct frame_buffer							//�������
	{
		char * start;
		int    length;
	}*frame;

	int totalframe;		//�ɼ�����������
}CHIRD_VIDEO_INFO_S;


static unsigned int __video_getcapability(int videofd)
{
	struct v4l2_capability cap;
	
	if(ioctl(videofd, VIDIOC_QUERYCAP, &cap) < 0){
		perror("VIDIOC_QUERYCAP failed");
		return -1;
	}

	return cap.capabilities;
}


/* ��ѯ��Ƶ�豸��Ϣ */
static int __video_query_info(int videofd) 
{
	struct v4l2_capability cap;

	if(ioctl(videofd, VIDIOC_QUERYCAP, &cap) < 0){
		perror("VIDIOC_QUERYCAP failed");
		return -1;
	}
	
	printf("Video Informations:\n");
	printf("\tdriver       	: %s\n", 	cap.driver);   
	printf("\tcard         	: %s\n", 	cap.card);
	printf("\tbus_info     	: %s\n", 	cap.bus_info);
	printf("\tversion      	: %08X\n", 	cap.version);
	
	printf("\tSupport operate :\n");
	if(V4L2_CAP_VIDEO_CAPTURE&cap.capabilities){
		printf("\t\tVideo Capture\n");
	}
	if(V4L2_CAP_VIDEO_OUTPUT&cap.capabilities){
		printf("\t\tVideo Output\n");
	}
	if(V4L2_CAP_VIDEO_OVERLAY&cap.capabilities){
		printf("\t\tVideo overlay\n");
	}
	if(V4L2_CAP_VBI_CAPTURE&cap.capabilities){
		printf("\t\tVBI capture\n");
	}
	if(V4L2_CAP_VBI_OUTPUT&cap.capabilities){
		printf("\t\tVBI Output\n");
	}
	if(V4L2_CAP_SLICED_VBI_CAPTURE&cap.capabilities){
		printf("\t\tSliced VBI capture\n");
	}
	if(V4L2_CAP_SLICED_VBI_OUTPUT&cap.capabilities){
		printf("\t\tSliced VBI Output\n");
	}
	if(V4L2_CAP_RDS_CAPTURE&cap.capabilities){
		printf("\t\tRDS capture\n");
	}
	if(V4L2_CAP_TUNER&cap.capabilities){
		printf("\t\tTUNER\n");
	}
	if(V4L2_CAP_AUDIO &cap.capabilities){
		printf("\t\tAUDIO\n");
	}
	if(V4L2_CAP_RADIO&cap.capabilities){
		printf("\t\tRADIO\n");
	}
	if(V4L2_CAP_READWRITE&cap.capabilities){
		printf("\t\tRead Write IO\n");
	}
	if(V4L2_CAP_ASYNCIO&cap.capabilities){
		printf("\t\tAsync IO\n");
	}
	if(V4L2_CAP_STREAMING&cap.capabilities){
		printf("\t\tStreaming MMap\n");
	}	
	printf("------------------------------------------\n");
	printf("\n");

	return 0;
}


/* ��ȡ��Ƶ�豸֧�ֵ���Ƶ��ʽ */
static int __video_scan_support(int videofd)
{
	struct v4l2_fmtdesc fmtdesc;
	unsigned int cap;

	cap = __video_getcapability(videofd);
	
	fmtdesc.index = 0;
	if(V4L2_CAP_VIDEO_CAPTURE&cap){
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		printf("Support format: Capture \n");
	}
	else if(V4L2_CAP_VIDEO_OUTPUT&cap){
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		printf("Support format: Output\n");
	}
	
	while (ioctl(videofd, VIDIOC_ENUM_FMT, &fmtdesc) != -1){	
		printf("\t%d. %s\n",fmtdesc.index+1,fmtdesc.description); 
 		fmtdesc.index++;
	}

	printf("------------------------------------------\n");
	printf("\n");
	
	return 0;
}

//������Ƶ�����ʽ
static int __video_set_param(int videofd, 
							int * width, 
							int * height, 
							int * format, 
							int * framerate)
{
	struct v4l2_format fmt = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};//֡��ʽ�ṹ��

	if(ioctl(videofd, VIDIOC_G_FMT, &fmt) < 0){
		perror("VIDIOC_G_FMT failed");
		return -1;
	}

	fmt.fmt.pix.width 		= * width;			//֡��
	fmt.fmt.pix.height 		= * height;			//֡��
	fmt.fmt.pix.pixelformat = * format;//���ظ�ʽ
	fmt.fmt.pix.field 		= V4L2_FIELD_INTERLACED;
	if(ioctl(videofd, VIDIOC_S_FMT, &fmt) < 0){
		perror("VIDIOC_S_FMT failed");
		return -1;
	}
	
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(videofd, VIDIOC_G_FMT, &fmt) < 0){
		perror("VIDIOC_G_FMT failed");
		return -1;
	}
	
	*width  = fmt.fmt.pix.width;
	*height = fmt.fmt.pix.height;
	*format = fmt.fmt.pix.pixelformat;

	struct v4l2_streamparm parm;
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
	if(ioctl(videofd,   VIDIOC_G_PARM,  &parm)){
		perror("VIDIOC_G_PARM failed\n");
		return -1;
	}

	//��������
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
	parm.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;     
	parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parm.parm.capture.timeperframe.denominator =  * framerate;
	parm.parm.capture.timeperframe.numerator = 1;
	if(ioctl(videofd,   VIDIOC_S_PARM,   &parm)){
		perror("VIDIOC_S_PARM failed\n");
		return -1;
	}


	if(ioctl(videofd,   VIDIOC_G_PARM,  &parm)){
		perror("VIDIOC_G_PARM failed\n");
		return -1;
	}

	if(* framerate != parm.parm.capture.timeperframe.denominator){
		* framerate = parm.parm.capture.timeperframe.denominator;
		printf(" reality framerate is :%d\n", * framerate);

	}


	return 0;
}

//��ϵͳ����֡���塢�����뵽��֡����ӳ�䵽�û��ռ䡢�����뵽��֡����ȫ�������
static int __video_request_memory(int videofd, int count, 
								struct frame_buffer * frame, 
								struct v4l2_buffer * vb)
{
	int i;
	struct v4l2_requestbuffers reqbuf = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};
	
	reqbuf.memory = V4L2_MEMORY_MMAP;	
	reqbuf.count = count;  

	//����������֡����
	if(ioctl(videofd , VIDIOC_REQBUFS, &reqbuf) < 0){
		perror("VIDIOC_REQBUFS failed");
		return -1;
	}

	memset(vb, 0 ,sizeof(struct v4l2_buffer));
	//�������֡����ȫ�������
	for(i = 0; i< count; i++) {	
		vb->index 	= i;	
		vb->type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;  
		vb->memory	= V4L2_MEMORY_MMAP;
		
		//��ȡ֡������Ϣ
		if(ioctl(videofd , VIDIOC_QUERYBUF, vb) < 0){
			perror("VIDIOC_QUERYBUF failed");
			return -1;
		}


		frame[i].length = vb->length;
		//ӳ���û��ռ�
		frame[i].start = mmap(0, vb->length, PROT_READ|PROT_WRITE, MAP_SHARED, videofd, vb->m.offset);
		if(frame[i].start == MAP_FAILED){
			perror("mmap video address failed");
			return -1;
		}
		
		debug("frame buffer %d: address=%#x, length=%#x\n",i,(int)frame[i].start, frame[i].length);
		
	}
	
	return 0;
}

//��������ȡ���Ѳɼ����ݵ�֡���壬ȡ��ԭʼ�ɼ�����
static int __video_request_stream(int videofd, struct v4l2_buffer * vb)
{	
	if(ioctl(videofd, VIDIOC_DQBUF, vb) < 0){
		perror("VIDIOC_DQBUF failed");
		return -1;
	}

	return 0;
}



//��֡�������·��뵽�ɼ������У���ֹ�������ù� 
static int __video_release_stream(int videofd, struct v4l2_buffer * vb)
{	
	if(ioctl(videofd, VIDIOC_QBUF, vb) < 0){
		perror("VIDIOC_QBUF failed");
		return -1;
	}

	return 0;
}

/*
static int chird_video_get_mjpegsize(char * mjpeg, int maxsize)
{
	int i;
	for (i = 0; i < maxsize; i++) {
		if((mjpeg[i] == 0x000000FF) && (mjpeg[i+1] == 0x000000D8))
			break;
	}
	printf("i = %d\n", i);
	if(i >= maxsize) return 0;
	else return maxsize - i;
}
*/

static void * chird_video_threadproc(void * param)
{
	CHIRD_VIDEO_INFO_S * pstvideo = (CHIRD_VIDEO_INFO_S*)param;
	int idx;
	fd_set readfds;
	int ret;
	int datalen;
	char * data;
	//	pthread_detach(pthread_self());

	while(pstvideo->threadrun){

		//��ʼ�ɼ�ͬ��
		if(sem_wait(&pstvideo->start_sync) < 0){
			perror("sem_wait");
			break;
		}
		
		while(pstvideo->threadsample){
			FD_ZERO(&readfds);
			FD_SET(pstvideo->videofd, &readfds);
			
			ret = select(pstvideo->videofd+1, &readfds, NULL, NULL, NULL);
			if(ret < 0) break;
		
			if(FD_ISSET(pstvideo->videofd, &readfds)){			
				//1���ɼ���Ƶ����
				ret = __video_request_stream(pstvideo->videofd, (struct v4l2_buffer *)&pstvideo->videobuf);	
				//�жϲɼ��ɹ���ʧ�ܣ��ɼ�ʧ���п�����video���ر�
				if(ret < 0) continue;

				//��ȡ�ڼ���
				data = pstvideo->frame[pstvideo->videobuf.index].start;
			
				if(pstvideo->format == FORMAT_MJPEG){
					//��ȡMJPEGͼƬ��С, JPEG��ͼƬ��СҪ��̬������ȡ
				//	datalen = chird_video_get_mjpegsize(data, pstvideo->videobuf.bytesused);	
					datalen = pstvideo->videobuf.bytesused;
				}
				else if(pstvideo->format = FORMAT_YUYV){
					datalen = pstvideo->videobuf.length;
				}
				
				//�ص�����
				pstvideo->pfun(data, datalen, pstvideo->param);	

				//4���ͷ���
				ret = __video_release_stream(pstvideo->videofd, (struct v4l2_buffer *)&pstvideo->videobuf);	
				if(ret < 0) continue;

				pstvideo->totalframe++;
			}
		}

		//�����ɼ�ͬ������
		if(sem_post(&pstvideo->end_sync) < 0){
			perror("sem_post failed");
			break;
		}
	}
	
	pthread_exit(0);
	
}

//���ܣ���/dev����Ѱ����û��video�豸������з����豸�������û�з���NULL
//valididx ��ȡ�ڼ�����Ч��Ƶ�豸
char * chird_video_devscan(int valididx)
{
	int i ,fd, idx = 0;
	static char name[20];
	struct v4l2_format fmt = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};//֡��ʽ�ṹ��


	for(i = 0; i < 16; i++){
		sprintf(name, "/dev/video%d", i);
		if(access(name, F_OK) == 0){
			//�򿪲���ȡ���ݣ��ɹ��Ļ�����ʹ��	
			fd = open(name, O_RDWR);
			if(fd < 0){
				continue;
			}

			if(ioctl(fd, VIDIOC_G_FMT, &fmt) >= 0){
				if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){
					close(fd);
					continue;
				}
			}
			close(fd);
			
			if(idx++ == valididx) return name;
		}
	}
	
	return NULL;
}

//����: ��Ƶ��ʼ��
void * chird_video_init(CHIRD_VIDEO_S * pst)
{
	int ret;
	CHIRD_VIDEO_INFO_S * pstvideo;
	if(NULL == pst) return NULL;

	if(pst->devname == NULL 
	|| pst->pfun == NULL
	|| (pst->framecount > 10 || pst->framecount < 0 )
	){
		debug("chird_video_init param failed\n");
		return NULL;
	}

	pstvideo = (CHIRD_VIDEO_INFO_S *)calloc(1, sizeof(CHIRD_VIDEO_INFO_S));
	if(NULL == pstvideo){
		perror("malloc");
		return NULL;
	}

	strcpy(pstvideo->devname,pst->devname);
	pstvideo->framerate = pst->framerate;
	pstvideo->format = pst->format;
	pstvideo->height = pst->height;
	pstvideo->width  = pst->width;
	pstvideo->pfun   = pst->pfun;
	pstvideo->param  = pst->param;
	pstvideo->framecount = pst->framecount;
	pstvideo->magic  = CHIRD_VIDEO_MAGIC;
	
	pstvideo->videofd = open(pstvideo->devname, O_RDWR);
	if(-1 == pstvideo->videofd){
		debug("chird_video_init  failed : %s : %s\n",
			pstvideo->devname, strerror(errno));
		goto ERROR;
	}

	ret = __video_set_param(pstvideo->videofd, 
							&pstvideo->width, 
							&pstvideo->height, 
							&pstvideo->format,
							&pstvideo->framerate);
	if(ret < 0){
		goto ERROR;
	}
	pst->height = pstvideo->height;
	pst->width = pstvideo->width;
	
	// ��ϵͳ����֡���� �ڴ�ӳ�� ����ɼ�����
	pstvideo->frame = calloc(pstvideo->framecount, sizeof(struct frame_buffer));
	if(NULL == pstvideo->frame){
		perror("calloc failed");
		goto ERROR;
	}


	ret = __video_request_memory(pstvideo->videofd, 
							pstvideo->framecount, 
							pstvideo->frame, 
							&pstvideo->videobuf);
	if(ret < 0){
		goto ERROR;
	}

	if(sem_init(&pstvideo->start_sync, 0 , 0) < 0){
		perror("sem_init");
		goto ERROR;
	}


	if(sem_init(&pstvideo->end_sync, 0 , 0) < 0){
		sem_destroy(&pstvideo->start_sync);
		perror("sem_init");
		goto ERROR;
	}

	pstvideo->threadrun = 1;
	ret = pthread_create(&pstvideo->tid, NULL, 
						chird_video_threadproc, 
						(void*)pstvideo);
	debug("\t ----------- chird_video_init successful  -----------\n");
	
	return pstvideo;

ERROR:
	free(pstvideo);
	return NULL;
}


//����: ��Ƶ����ʼ��
int chird_video_uninit(void * handle)
{
	int i;
	CHIRD_VIDEO_INFO_S * pstvideo = (CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	//û����ִ�У�ֱ�ӷ���
	if(pstvideo->threadrun == 0){
		return -1;
	}
	
	pstvideo->threadrun = 0;		//�߳����б����0����ʾ�˳�

	if(pstvideo->threadsample == 1){
		pstvideo->threadsample = 0;
		if(sem_wait(&pstvideo->end_sync) < 0){
			perror("sem_wait failed");
			return -1;
		}
	}

	if(sem_post(&pstvideo->start_sync) < 0){
		perror("sem_post failed");
		return -1;
	}
	
	pthread_join(pstvideo->tid, (void**)NULL);

	for (i = 0; i < pstvideo->framecount; i++) {
		munmap(pstvideo->frame[i].start, pstvideo->frame[i].length);
	}
	
	sem_destroy(&pstvideo->start_sync);
	sem_destroy(&pstvideo->end_sync);
	
	close(pstvideo->videofd);

	free(pstvideo);
	
	debug("\t ----------- chird_video_uninit successful  -----------\n");
	
	return 0;
}

//����: ������Ƶ�ɼ�
int chird_video_start(void * handle)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	CHIRD_VIDEO_INFO_S * pstvideo = (CHIRD_VIDEO_INFO_S*)handle;
	int ret;
	int i;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	//�Ѿ��ڲɼ���������
	if(pstvideo->threadsample == 1){
		return 0;
	}
	
#if 1
	//����Ƶ��Żػ�����
	for(i = 0; i < pstvideo->framecount; i++){
		pstvideo->videobuf.index = i;
		if(ioctl(pstvideo->videofd, VIDIOC_QBUF, &pstvideo->videobuf) < 0){
			perror("VIDIOC_QBUF failed");
			return -1;
		}	
	}
#endif

	if(ioctl(pstvideo->videofd, VIDIOC_STREAMON, &type) < 0){
		perror("VIDIOC_STREAMON failed");
		return -1;
	}
	
	debug("\t ========= Open Video Capture ===========\n");

	//�����ź� 
	pstvideo->threadsample = 1;
	if(sem_post(&pstvideo->start_sync) < 0){
		perror("sem_post failed");
		return -1;
	}
	
	return 0;
}

//����: �ر���Ƶ�ɼ�
int chird_video_end(void * handle)
{
	CHIRD_VIDEO_INFO_S * pstvideo = (CHIRD_VIDEO_INFO_S*)handle;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	//�Ѿ����ڲɼ���������
	if(pstvideo->threadsample == 0){
		return 0;
	}
	
	pstvideo->threadsample = 0;		//�߳����б����0����ʾ�˳�	

	if(sem_wait(&pstvideo->end_sync) < 0){
		perror("sem_wait failed");
		return -1;
	}

	if(ioctl(pstvideo->videofd, VIDIOC_STREAMOFF, &type) < 0){
		perror("VIDIOC_STREAMOFF failed");
		return -1;
	}

	debug("\t ========= Close Video Capture ===========\n");
	
	return 0;	
}

//����: ��ȡ��Ƶ����
int chird_video_showparam(void * handle)
{
   const CHIRD_VIDEO_INFO_S * pstvideo = (const CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	struct v4l2_format fmt = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};
	char fmtstr[8] = {0};
	struct v4l2_streamparm parm = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};

	if ((ioctl(pstvideo->videofd, VIDIOC_G_FMT, &fmt) < 0)
		|| (ioctl(pstvideo->videofd,   VIDIOC_G_PARM,  &parm)) < 0){				
		printf("get format failed\n");
		return -1;
	}
	else{
		memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
		printf("\twidth        : %d\n", fmt.fmt.pix.width);
		printf("\theight       : %d\n", fmt.fmt.pix.height);
		printf("\tpixelformat  : %s\n", fmtstr);
		printf("\tbytesperline : %d\n", fmt.fmt.pix.bytesperline);
		printf("\tsizeimage    : %d\n", fmt.fmt.pix.sizeimage);
		printf("\tframerate    : %d\n", parm.parm.capture.timeperframe.denominator);
		printf("------------------------------------------\n");
		printf("\n");
	}

	
	return 0;
	
}

int chird_video_query_info(void * handle)
{
	const CHIRD_VIDEO_INFO_S * pstvideo = (const CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	__video_query_info(pstvideo->videofd);
	
	return 0;
}


int chird_video_scan_support(void * handle)
{
	const CHIRD_VIDEO_INFO_S * pstvideo = (const CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	__video_scan_support(pstvideo->videofd);
	
	return 0;
}

//����: ��ȡ����
int chird_video_getframecount(void * handle)
{
	const CHIRD_VIDEO_INFO_S * pstvideo = (const CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	return pstvideo->totalframe;
}

//����: �������
int chird_video_clearframecount(void * handle)
{
	CHIRD_VIDEO_INFO_S * pstvideo = (CHIRD_VIDEO_INFO_S*)handle;
	
	if(NULL == pstvideo ||
		pstvideo->magic != CHIRD_VIDEO_MAGIC){
		debug("chird_video : handle wrong!!\n");
		return -1;
	}

	pstvideo->totalframe = 0;

	return 0;
}

#if 1
#include <signal.h>
#include <sys/select.h>
void *  VideoHandle = NULL;
void sig_proc(int arg)
{

	debug("famrecount = %d\n", chird_video_getframecount(VideoHandle));
	chird_video_clearframecount(VideoHandle);
	
	alarm(1);
}
void video_callback(const char * videodata, int length, void * param)
{
	debug("video  : param = %d length = %d \n", param, length);

	//process videodata here ....
}

int main(int argc, char ** argv)
{
	CHIRD_VIDEO_S stVideo;
	struct timeval tv;
	int i;
	
	signal(SIGALRM, sig_proc);

	stVideo.framerate = 30;
	stVideo.framecount = 2;
	stVideo.width  = 640;
	stVideo.height = 480;
	stVideo.pfun  = video_callback;
	stVideo.param = (void*)0;
	stVideo.format =  FORMAT_MJPEG;
	
	debug("Open the first video device..\n");
	stVideo.devname = chird_video_devscan(0);
	VideoHandle = chird_video_init(&stVideo);

	chird_video_query_info(VideoHandle);
	chird_video_scan_support(VideoHandle);
	chird_video_showparam(VideoHandle);

	
	alarm(1);
	if(NULL != VideoHandle)chird_video_start(VideoHandle);
	while(1){
		getchar();
	}
	
	if(NULL != VideoHandle)chird_video_uninit(VideoHandle);	

	//�������ԣ���ʼ��->����->ͣ��->�ɼ�->����ʼ��
	for(i = 0 ; i < 100; i++){
		//�򿪵�һ����Ƶ�豸
		debug("Open thie first video device..\n");
		stVideo.devname = chird_video_devscan(0);
		VideoHandle = chird_video_init(&stVideo);

		tv.tv_sec = 0;
		tv.tv_usec = 500*1000;
		select(0,NULL, NULL, NULL, &tv);
		printf("\t############# start\n");
		if(NULL != VideoHandle)chird_video_start(VideoHandle);
		
		getchar();
		
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		select(0,NULL, NULL, NULL, &tv);
		printf("\t############# end\n");
		if(NULL != VideoHandle)chird_video_end(VideoHandle);	

		tv.tv_sec = 0;
		tv.tv_usec = 500*1000;
		select(0,NULL, NULL, NULL, &tv);
		printf("\t############# start\n");
		if(NULL != VideoHandle)chird_video_start(VideoHandle);


		tv.tv_sec = 1;
		tv.tv_usec = 0;
		select(0,NULL, NULL, NULL, &tv);
		printf("\t############# end\n");
		if(NULL != VideoHandle)chird_video_end(VideoHandle);

		printf("\t############# uninit\n");
		if(NULL != VideoHandle)chird_video_uninit(VideoHandle);

		tv.tv_sec = 0;
		tv.tv_usec = 500*1000;
		select(0,NULL, NULL, NULL, &tv);
	}

	return 0;

}

#endif
