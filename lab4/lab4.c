#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
void printdir(char * dir, int depth);
void printrights(const unsigned short rights);

int main(int argc ,char const *argv[])
{
	char dir[50]=".";
	if(argc<=1)
	{
		printdir(dir,1);
		return 0;
	}
	strcpy(dir,argv[1]);
	printdir(dir,1);
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
		printf("opendir %s error\n",dir);
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
			printf("%10ld\t",statbuf.st_size);
			//char * tmpp= strtok(ctime(&statbuf.st_mtime),"\n");
			//printf("%s\t%s\n",tmpp,entry->d_name);
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
			printf("%10ld\t",statbuf.st_size);
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
    if(rights & S_IRUSR)  
        printf("r");
    else
        printf("-");
    if(rights & S_IWUSR)  
        printf("w");
    else
        printf("-");
    if(rights & S_IXUSR) 
		printf("x");
    else
		printf("-");
    if(rights & S_IRGRP)  
		printf("r");
    else
		printf("-");
    if(rights & S_IWGRP)  
		printf("w");
    else
		printf("-");
    if(rights & S_IXGRP) 
		printf("x");
    else
		printf("-");
    if(rights & S_IROTH) 
		printf("r");
    else
		printf("-");
    if(rights & S_IWOTH)
		printf("w");
    else
		printf("-");
    if(rights & S_IXOTH)
		printf("x\t");
    else
		printf("-\t");
    
    return ;
}
