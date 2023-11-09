#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>


#include "dirFiles.h"

#define PATH_MAXLEN 255 //PATH_MAXLEN

void ERR_OPEN_DIR(char* directory, int errnumb){
	if(directory == NULL){
		fprintf(stderr, "Error: directory is NULL (ERR_OPEN_DIR function)\n");
		_exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Error: opening directory '%s'\n", directory);
	switch(errnumb){
		case 1: perror("EPERM: Operation not permitted\n");
				break;
		case 2: perror("ENOENT: No such file or directory\n");
				break;
		case 3: perror("EIO: Input/output error\n");
				break;
		default: perror("Error occured\n");
				break;
	} 
	exit(EXIT_FAILURE);
}

void ERR_CLOSE_DIR(char* directory, int errnumb){
	if(directory == NULL){
		fprintf(stderr, "Error: directory is NULL (ERR_CLOSE_DIR function)\n");
		_exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Error: closing directory '%s'\n", directory);
	switch(errnumb){
		case 1: perror("EPERM: Operation not permitted\n");
				break;
		case 2: perror("ENOENT: No such file or directory\n");
				break;
		case 3: perror("EIO: Input/output error\n");
				break;
		default: perror("Error occured\n");
				break;
	} 
	exit(EXIT_FAILURE);
}

//true->last char is slash, false->otherwise
bool end_slash(char* str){
	if(str == NULL){
		fprintf(stderr, "Error: str is NULL (end_slash function)\n");
		_exit(EXIT_FAILURE);
	}
	char c;
	size_t i=0;
	while(str[i] != '\0'){
		c = str[i++];
	}
	return (c == '/' ? true : false);
}

//modifies the 
void exploreDir(char* dirName, char** filesArr, unsigned int* filesNum){
	if(dirName == NULL){
		fprintf(stderr, "Error: dirName is NULL (exploreDir function)\n");
		_exit(EXIT_FAILURE);
	}

	int dirFilesNum = 0;

	char* fullpath = (char*) malloc(PATH_MAXLEN*sizeof(char));
	assert(fullpath);
	fullpath = full_path(fullpath);
	fullpath = full_path_stat(fullpath, dirName);
	my_find(fullpath, dir, filename);
	free(fullpath);
	return 0;
}

//return the fullpath from the cwd
char* full_path(char* buf_path){
	//size_t i=1; //REMOVED NOW
	//char* buf_path = (char*) malloc(N*sizeof(char));
	if(buf_path == NULL){
		fprintf(stderr, "Error: buf_path is NULL (full_path function)\n");
		_exit(EXIT_FAILURE);
	}
	if(getcwd(buf_path, PATH_MAXLEN) == NULL){
		if(errno == 34){
			fprintf(stderr, "Error: pathname is bigger than %d (full_path function)\n", PATH_MAXLEN);
		}
		else{
			fprintf(stderr, "Error: error in getcwd function (full_path function)%n");
		}
		_exit(EXIT_FAILURE);
	}
	if(end_slash(buf_path) == false){
		buf_path = strncat(buf_path, "/", strnlen(buf_path, PATH_MAXLEN));
	}
	if(buf_path == NULL){
		fprintf("Error: something went wrong creating fullpath (full_path function)\n");
		exit(EXIT_FAILURE);
	}
	return buf_path;
}

//return the fullpath from the cwd
char* full_path_stat(char* path, char* file){
	size_t i=1;
	size_t dim_tot = strnlen(path, PATH_MAXLEN)+strnlen(file, PATH_MAXLEN)+3;
	if(dim_tot > PATH_MAXLEN){			
		fprintf(stderr, "Error: pathname is bigger than %d (full_path_stat function)\n", PATH_MAXLEN);
		_exit(EXIT_FAILURE);
	}

	char* fullpath = (char*) calloc(dim_tot, sizeof(char));
	fullpath = strncat(fullpath, path, dim_tot);
	if(end_slash(fullpath) == 0)
		fullpath = strncat(fullpath, "/", dim_tot);
	fullpath = strncat(fullpath, file, dim_tot);
	
	if(fullpath == NULL){
		fprintf("Error: something went wrong creating fullpath (full_path_stat function)\n");
		exit(EXIT_FAILURE);
	}
	return fullpath;
}

void my_find(char* fullpath, char* dir, char* filename){
	DIR *dp;
	struct dirent *dirp;

	char* dir_con = (char*) calloc(PATH_MAXLEN, sizeof(char));
	if(end_slash(dir) == 0){
		dir = strncat(dir, "/", PATH_MAXLEN);				
	}
	dir_con = strncpy(dir_con, dir, PATH_MAXLEN);

	#if defined(DEBUG) //DEBUGGING GETCWD
		printf("dir: %s\n", dir);
		printf("cwd: %s\n", buf_path);
	#endif
	
	if((dp = opendir(dir)) == NULL){ //OPEN DIRECTORY
		ERR_OPEN_DIR(dir, errno);
	}
	else{
		//printf("Apertura directory: '%s'\n", dir);  //DEBUG LINE
	}

	struct stat statbuf;
	char* tmp_file = NULL;
	size_t dim_tmp;
	time_t time_tmp;
	char* path_stat = NULL;
	size_t cont = 0;

	while((errno = 0, dirp = readdir(dp)) != NULL){ //CONTINUAREEE!!!
		tmp_file = dirp->d_name;
		//printf("\n%s\n", tmp_file);
		
		dim_tmp = strnlen(tmp_file, 50)+1;
		path_stat = full_path_stat(fullpath, tmp_file); //CONTINUAREEEEE!!!
		printf("full_path_stat : %s\n", path_stat);
		
		//if(stat(path_stat, &statbuf) == -1){
		if(stat(path_stat, &statbuf) == -1){
			cont++;
			//printf("cont: %ld\n", cont);
			perror("error in function stat\n");
			exit(EXIT_FAILURE);
		}
		if(S_ISDIR(statbuf.st_mode)){
			//printf("ciao\n");
			if((strncmp(tmp_file,"..", 100) != 0) && (strncmp(tmp_file,".", 100) != 0)){
				dir_con = strncat(dir_con, tmp_file, 40);
				my_find(path_stat, dir_con, filename);
			}
		}
		else{
			if(strncmp(tmp_file, filename, dim_tmp) == 0){
				time_tmp = statbuf.st_mtime;
				//printf("time_t: %lld\n", (long long) time_tmp); //DEBUG
				my_print(fullpath, tmp_file, time_tmp);
			}
		}
		//free(path_stat);
	}
	if(errno != 0){
		perror("error in function readdir\n");
		exit(EXIT_FAILURE);
	}
	if(closedir(dp) == -1) //CLOSE DIRECTORY
		ERR_CLOSE_DIR(dir, errno);
	free(dir_con);
	//free(fullpath);
	return;
}
