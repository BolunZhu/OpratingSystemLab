//#pragma GCC diagnostic error "-std=c++11"
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
void parent_func(int sig){
	kill(-getpid(),SIGUSR1);
return;
}

void son1_func(int sig){
	if(sig==SIGUSR1){
	printf("Child Process 1 is Killed by Parent!\n");
	exit(1);
	}
}
void son2_func(int sig){
	if(sig==SIGUSR1){
	printf("Child Process 2 is Killed by Parent!\n");
	exit(2);
	}
}

int main(void)
{
	//create uname_pipe
	pid_t pid;
	pid_t pid1,pid2;
	int pipefd[2];
	int res = pipe(pipefd);
	//catch signal SIGINT
	if(signal(SIGINT,parent_func)==SIG_ERR){
		printf("catch SIGINT error!\n");
	}
	//create pid1
	pid1=fork();
	if(pid1==0)
	{//pid1 func
//	printf("now son_pid1 pid = %d\n",getpid());
	signal(SIGINT,SIG_IGN);
	if(signal(SIGUSR1,son1_func)==SIG_ERR){
		printf("catch SIGUSR1 error!\n");
	}
	int times=1;
		while(1){
		stringstream ss;
		ss<<times;
		string str1 = "I send you "+ss.str()+" times\n";
//		cout<<str1;
		const char *p1=str1.c_str();
		close(pipefd[0]);
//		cout<<"size p1 = "<<str1.size();
		write(pipefd[1],p1,str1.size());
		times++;
		sleep(1);
		}
	}
	else
	{//create pid2
	pid2=fork();
	if(pid2==0)
	{//pid2 func
//	printf("now son_pid2 pid = %d\n",getpid());
	signal(SIGINT,SIG_IGN);
	if(signal(SIGUSR1,son2_func)==SIG_ERR){
		printf("catch SIGUSR1 error!\n");
	}
	char buffer[50];
	close(pipefd[1]);
//	printf("closed pipefd[1]\n");
		while(1)
		{
		read(pipefd[0],buffer,sizeof(buffer));
		printf("%s",buffer);
		}
	}
	else
	{//main pid
	//thro signal SIGINT
	signal(SIGUSR1,SIG_IGN);
	pid=waitpid(pid1,NULL,0);
//	printf("1st pid= %d \n",pid);
	pid=waitpid(pid2,NULL,0);
//	printf("2rd pid= %d \n",pid);
	//printf("pid1 = %d , pid2 = %d \n",pid1,pid2);
	close(pipefd[0]);
	close(pipefd[1]);
	printf("Parent Process is Killed!\n");
	return 0;
	}
	}
return 0;
}
