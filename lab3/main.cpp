#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>//for execl
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
const int sh_key=1234;//for shmget
const int sem_key=2345;
const int array_len=4096;
const int cache_size = 16;
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
    //create share memory :cache array
    if(argc!=3){
        printf("#parameter !=3 exit\n");
        return -1;
    }
    int p1,p2;
    int shmid1=shmget(sh_key,  array_len, IPC_CREAT|0666);
    if(shmid1==-1)
    {
        return -1;
    }
    char * S = (char *)shmat(shmid1,NULL,SHM_R|SHM_W);
    //create 2 sem: 0-read 1-write
	if(-1==(semid1= semget(sem_key,2,IPC_CREAT|0666) ))
	{
		return -1;
	}
    //set value for sem
	union semun arg;
	arg.val=0;
	if(-1==semctl(semid1,0,SETVAL,arg))
	{
		return -1;
	}
	arg.val=cache_size;
	if(-1==semctl(semid1,1,SETVAL,arg))
	{	
		return -1;
	}
    //get the length of file
    FILE * fp = fopen(argv[1],"rb");
    fseek(fp, 0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(fp); //文件长度
    fclose(fp);
    //设置参数
    char cdatasize[20];
    sprintf(cdatasize,"%d",nFileLen);
    printf("now nFilelen=%d cdatasize =%d\n",nFileLen,atoi(cdatasize));
    //create 2 thread: readbuf writebuf
    p1=fork();
    if(p1==0)
    {//p1 begin
        printf("Process 1 readbuf created\n");
	execl("./readbuf","readbuf",argv[2],cdatasize,NULL);//output filepath
        fprintf(stderr, "here p1 would never run\n");
        exit(0);
    }
    else{
        p2=fork();
        if(p2==0)
        {//p2 begin
        printf("Process 2 writebuf created\n");
	execl("./writebuf","writebuf",argv[1],cdatasize,NULL);//input filepath
        fprintf(stderr, "here p2 would never run\n");
        exit(0);
        }
        else
        {
            //main begin
            //wait for 2 thread end
            waitpid(p1,NULL,0);
            waitpid(p2,NULL,0);
            //delete sem
            if (semctl(semid1, 0, IPC_RMID, arg) == -1)
            {
                fprintf(stderr, "Failed to delete sem\n");
                return -1;
            }
            //delete shm
            if (shmctl(shmid1, IPC_RMID, 0) == -1)
            {
                fprintf(stderr, "shmctl(IPC_RMID) failed\n");
                return -1;
            }
            printf("main process has been killed\n");
        }
    }
   
    return 0;
}
