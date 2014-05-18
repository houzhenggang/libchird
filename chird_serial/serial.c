#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
#include     <unistd.h>     /*Unix��׼��������*/
#include     <sys/types.h> /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termio.h>    /*PPSIX�ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
#include     <string.h>
#include     "serial.h"

int chird_serial_open(const char * Dev)
{
	int fd = open(Dev, O_RDWR);        
	if (-1 == fd){ 						 /*��������λ��*/
		perror("Can't Open Serial Port");
		return -1;
	}

	return fd;
}



void chird_serial_setspeed(int fd, int speed)
{
	int   i;
	int   status;
	struct termios   Opt;
    int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
     B38400, B19200, B9600, B4800, B2400, B1200, B300, };
    int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300,
     38400, 19200, 9600, 4800, 2400, 1200, 300, };

	tcgetattr(fd, &Opt);
	for ( i= 0; i < sizeof(speed_arr) / sizeof(int); i++){
	   if (speed == name_arr[i])
	   {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
            Opt.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|NOFLSH);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0)
				perror("tcsetattr fd1");
		 return;
		}
       
   tcflush(fd,TCIOFLUSH);
   }
}

int chird_serial_setParity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if ( tcgetattr( fd,&options) != 0)					/*tcgetattr�������ڻ�ȡ���ն���صĲ���������fdΪ�ն˵��ļ���������
														���صĽ��������termios�ṹ����,�ɹ�����0,ʧ�ܷ���-1*/
	{
		perror("SetupSerial 1");
		return(0);
	}

	options.c_cflag &= ~CSIZE;
	switch (databits) /*��������λ��*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");	
		return(0);/*return (FALSE);*/
	}
	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/ 
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/ 
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's': /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
		break;
		default:
			fprintf(stderr,"Unsupported parity\n");
		return(0);/*return (FALSE);*/
    }
/* ����ֹͣλ*/   
	switch (stopbits)
	{
		case 1:
		   options.c_cflag &= ~CSTOPB;
		   break;
		case 2:
		   options.c_cflag |= CSTOPB;
		   break;
		default:
		   fprintf(stderr,"Unsupported stop bits\n");
		   return(0);/*return (FALSE);*/
	}
/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
	   perror("SetupSerial 3");
	   return (0);/*return (FALSE);*/
	}
	return(1)/*return (TRUE)*/;
}
//////////////////////////////////////////////////////////////////////////////
int chird_serial_open(char *dev, int speed){

    int fd;

    fd = OpenDev(dev);				/*openDev��com1���豸�ļ�,�򿪷���1��û�д򿪷���0*/

    if (fd>0)						/*�����com1*/
		set_speed(fd,speed);		/*���ò�����Ϊ19200*/
	else							/*�������������Ϣ"û�д򿪴���"*/
	{
	   printf("Can't Open Serial Port!\n");
	   exit(0);						/*�˳�����*/
	}
    
	if (set_Parity(fd,8,1,'N')== 0)	/*���ô򿪵��ļ�fd,����λ8λ,ֹͣλ1λ,��żУ��λ,ȡֵΪN,E,O,S*/
	{
		printf("Set Parity Error\n");
		exit(1);
	}

	chird_serial_setparam(fd, &speed, 8, 1, 'N');

    return fd;

}     

int chird_serial_setparam(int fd, const int * speed, 
									const int * databits, 
									const int * stopbits, 
									const int * parity)
{
	if(NULL != speed){
		if(chird_serial_setspeed(fd, speed) < 0)
			return -1;
	}

	if(NULL != databits){
	}

	if(NULL != stopbits){
	}

	if(NULL != parity){
		
	}
	
}

int serial_read(int fd, char *buf){

    int cnt;
	memset(buf, 0, 2);	
    cnt = read(fd, buf, 1);
   // printf("read(%d):%c\n", cnt, *buf);
	
    return cnt;
}

int serial_write(int fd, char *buf){

    int cnt;

    cnt = write(fd, buf, 1);
  //  printf("write(%d):%c\n", cnt,*buf);
    
    return cnt;
}

int serial_close(int fd){
    
    close(fd);
}


