#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>

//
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//
//for debug
#include <errno.h>
int semid1,semid2;
pthread_t p1,p2;
int sum=0;
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
void *subp1(void * arg)
{//printf
	for(int i=1;i<=100;i++)
	{
	P(semid1,0);
	printf("%d:%d\n",i,sum);
	V(semid2,0);
	}
	return NULL;
}
void *subp2(void * arg)
{//calculate
	for(int i=1;i<=100;i++)
	{//let semid1 = read semid2 =write
	P(semid2,0);
	sum=sum+i;
	V(semid1,0);
	}
	return NULL;
}

int main(void)
{
//create 2 semid	
	if(-1==(semid1= semget(4321,1,IPC_CREAT|0666) ))
	{
		return -1;
	}
	if(-1==(semid2= semget(5432,1,IPC_CREAT|0666) ))
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
	arg.val=1;
	if(-1==semctl(semid2,0,SETVAL,arg))
	{	
		return -1;
	}
//create 2 pthread
	if(pthread_create(&p1,NULL,subp1,NULL))
	{// success return 0
		return -1;
	}
	if(pthread_create(&p2,NULL,subp2,NULL))
	{
		return -1;
	}
	void * status;
	if(pthread_join(p1,&status))
	{
		return -1;
	}
	if(pthread_join(p2,&status))
	{
		return -1;
	}
//OK! haha 
return 0;
}

