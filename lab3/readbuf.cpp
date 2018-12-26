#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>//for execl
#include <stdio.h>
#include <stdlib.h>
#include <string>
const int sh_key=1234;//for shmget
const int sem_key=2345;
const int array_len=4096;
const int cache_size=16;
int semid1;
union semun
{
	int val;
	struct semid_ds * buf;
	unsigned short * array;
};
void P(int semid,int index)
{
	struct sembuf sem;
	sem.sem_num=index;
	sem.sem_op=-1;
	sem.sem_flg=0;
	semop(semid,&sem,1);
	return ;
}
void V(int semid,int index)
{
	struct sembuf sem;
	sem.sem_num=index;
	sem.sem_op=1;
	sem.sem_flg=0;
	semop(semid,&sem,1);
	return ;
}

int main(int argc, char const *argv[])
{
    if(argc!=3)
    {
        printf("readbuf:argc!=3 exit\n");
        return -1;
    }
    // get shm
    int shmid1=shmget(sh_key, 0 ,0);
    if(shmid1==-1)
    {
        return -1;
    }
    
    char * S = (char *)shmat(shmid1,NULL,SHM_R|SHM_W);
    // get sem
    //create 2 sem: 0-read 1-write
	if(-1==(semid1= semget(sem_key,0,0) ))
	{
		return -1;
	}
    // begin read
    FILE * fp =fopen(argv[1],"wb");
    int index =0;
    int size = atoi(argv[2]);
    int got = 0;
    int bufsize = array_len/cache_size;
    while(size>0)
    {
	if(size<bufsize)
	{
	   bufsize=size;
	}
        P(semid1,0);
	got=fwrite(S+index*cache_size,1,bufsize,fp);
        V(semid1,1);
        printf("readbuf read %d bytes from sharememory and write them into ouput file\n",got);
	index = (index+1)%cache_size;
        size-=got;
    }
    fclose(fp);
    printf("readbuf successfully copy the file from share memory into ouput file\n");
    printf("readbuf was killed\n");
    return 0;
    
}
