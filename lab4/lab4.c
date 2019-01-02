#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>

void printdir(char * dir, int depth);
void printrights(const unsigned short rights);

int main(int argc ,char const *argv[])
{
	char dir[50];
	printf("Input dir path\n");
	scanf("%s",dir);
	printdir(dir,1);
	printf("main process over\n");
	return 0;
}

void printdir(char * dir , int depth)
{
	DIR * dp;
	struct dirent * entry;
	struct stat statbuf;
	//open dir
	if(NULL==(dp = opendir(dir)))
	{
		printf("opendir error\n");
		return ;
	}
	//change dir
	chdir(dir);
	//search dir
	while(NULL!= (entry=readdir(dp)) ){
		//call lstat by filename
		lstat(entry->d_name,&statbuf);
		//if is a dir
		if(S_ISDIR(statbuf.st_mode))
		{
			//if name is . or ..
			if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
			{
				continue;//skip
			}
			//now print dir's info (e.g. depth & name )
			printrights(statbuf.st_mode);
			printf("%d\t",depth);
			//printf user name
			struct passwd * upwd;
			upwd = getpwuid(statbuf.st_uid);
			printf("%s\t",upwd->pw_name);
			//printf group name
			struct passwd * gpwd;
			if(NULL != (gpwd = getpwuid(statbuf.st_uid)))
			{
				printf("%s\t",gpwd->pw_name);
			}
			printf("%10lld\t",statbuf.st_size);
			printf("%s\t%s\n",strtok(ctime(&statbuf.st_mtime),"\n"),entry->d_name);
			printdir(entry->d_name,depth+4);
		}
		else{
			//print info
			printrights(statbuf.st_mode);
			printf("%d\t",depth);
			//printf user name
			struct passwd * upwd;
			upwd = getpwuid(statbuf.st_uid);
			printf("%s\t",upwd->pw_name);
			//printf group name
			struct passwd * gpwd;
			if(NULL != (gpwd = getpwuid(statbuf.st_uid)))
			{
				printf("%s\t",gpwd->pw_name);
			}
			printf("%10lld\t",statbuf.st_size);
			printf("%s\t%s\n",strtok(ctime(&statbuf.st_mtime),"\n"),entry->d_name);
		}
	}
	//return father dir
	chdir("..");
	//close dir
	closedir(dp);
	return ;
}

void printrights(const unsigned short rights)
{
    printf("-");
    if(rights & S_IRUSR)    //文件所有者具有可读权�?
        printf("r");
    else
        printf("-");
    if(rights & S_IWUSR)    //文件所有者具有可写权�?
        printf("w");
    else
        printf("-");
    if(rights & S_IXUSR)    //文件所有者具有可执行权限
		printf("x");
    else
		printf("-");
    if(rights & S_IRGRP)    //用户组具有可读权�?	
		printf("r");
    else
		printf("-");
    if(rights & S_IWGRP)    //用户组具有可写权�?
		printf("w");
    else
		printf("-");
    if(rights & S_IXGRP)    //用户组具有可执行权限
		printf("x");
    else
		printf("-");
    if(rights & S_IROTH)    //其他用户具有可读权限
		printf("r");
    else
		printf("-");
    if(rights & S_IWOTH)    //其他用户具有可写权限
		printf("w");
    else
		printf("-");
    if(rights & S_IXOTH)    //其他用户具有可执行权�?
		printf("x\t");
    else
		printf("-\t");
    
    return ;
}