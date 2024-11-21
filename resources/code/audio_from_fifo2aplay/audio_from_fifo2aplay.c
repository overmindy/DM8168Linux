#include "comm.h"

int main(int argc,char* argv[])
{
	char buffer[128];
	FILE *fp;
	int rc;

  printf("start...\n");
  umask(0);   
//  if(mkfifo(MY_FIFO,0666) <0){
//      perror("mkfifo");
//      return 1;
//  }
/*  first using mkfifo commnand to create FIFO in Linux shell ,so program only open !*/

  /* ֻ��Ҫ�ļ���������  */
  int fd = open(MY_FIFO,O_RDONLY);   /* open FIFO to get audio data from ALSA02  */
  if(fd < 0){
     perror("open");
     return 1;
  }

	printf("Start record data from FIFO to stdout for aplay | Only ctrl_c to break !\n");
	fp =fopen("alsa_wav.bin","wb");
	if(NULL ==fp){
		printf("open file failed ! \n");
		return -1;
		}
	fflush(stdout);
	while(1){
//	rc=read(0,buffer,128);
	rc = read(fd,buffer,128);    /* read from FIFO,32 samples,2 bytes/sample, 2 channels ,each time*/
	if(rc==128){
	//	fwrite(buffer,1,128,fp);
		write(1,buffer,128);      /* write to STDOUT for aplay  */
	}
	else{
		if(rc>0)
		 fwrite(buffer,1,rc,fp);
	//	 write(1,buffer,rc);      /* write to STDOUT for aplay  */   
	 }
	}
	
	fclose(fp);
//	close(fd);    /* close fifo  */
  return 0;
}


#include "comm.h"

int main(int argc,char* argv[])
{
	char buffer[128];
	FILE *fp;
	int rc;

  printf("start...\n");
  umask(0);   
//  if(mkfifo(MY_FIFO,0666) <0){
//      perror("mkfifo");
//      return 1;
//  }
/*  first using mkfifo commnand to create FIFO in Linux shell ,so program only open !*/

  /* ֻ��Ҫ�ļ���������  */
  int fd = open(MY_FIFO,O_RDONLY);   /* open FIFO to get audio data from ALSA02  */
  if(fd < 0){
     perror("open");
     return 1;
  }

	printf("Start record data from FIFO to stdout for aplay | Only ctrl_c to break !\n");
	fp =fopen("alsa_wav.bin","wb");
	if(NULL ==fp){
		printf("open file failed ! \n");
		return -1;
		}
	fflush(stdout);
	while(1){
//	rc=read(0,buffer,128);
	rc = read(fd,buffer,128);    /* read from FIFO,32 samples,2 bytes/sample, 2 channels ,each time*/
	if(rc==128){
	//	fwrite(buffer,1,128,fp);
		write(1,buffer,128);      /* write to STDOUT for aplay  */
	}
	else{
		if(rc>0)
		 fwrite(buffer,1,rc,fp);
	//	 write(1,buffer,rc);      /* write to STDOUT for aplay  */   
	 }
	}
	
	fclose(fp);
//	close(fd);    /* close fifo  */
  return 0;
}

