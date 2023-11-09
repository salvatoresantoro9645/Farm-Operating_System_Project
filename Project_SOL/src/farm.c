#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#include "master.h"
#include "workerPool.h"

//#define DEBUG_CONF
//#define DEBUG

//usage message
#define USAGE_MESSAGE \
	"usage: ./farm [OPTION]... [FILE]...\n"

//message to help user use options correctly
#define HELP_MESSAGE \
	"command that optionally can be passed as arguments of farm are:\n" \
	"-n <nthread> : specifies the number of Workers threads\n" \
			"\t \t of Master process (default value : 4)\n" \
	"-q <qlen> : specifies the length of cuncurrent queue between\n" \
			"\t \t Master thread and Workers threads (default value : 8)\n" \
	"-d <directory-name> : specifies a directory containing binary files\n" \
			"\t \t and eventually other directories containing binary files;\n" \
			"\t \t binary files must be used as input files for calculus\n" \
	"-t <delay> : specifies a time in milliseconds that elapses between\n" \
			"\t \t the sending of two subsequent request to the Worker threads\n" \
			"\t \t by the Master thread (default value : 0)\n"

//used to chech commands validity
#define VALID_COMMAND(charachet) \
	(charachet == 'n' || charachet == 'q' || \
	 charachet == 'd' || charachet == 't')

#define INVALID_OPTARG(optarg) \
	fprintf(stderr, "Error: Invalid argument for '-%c' option\n", optarg); \
	fprintf(stdout, USAGE_MESSAGE); \
	fprintf(stdout, HELP_MESSAGE); \
	_exit(EXIT_FAILURE);

#define INVALID_OPTARG_STRING(optargString) \
	fprintf(stderr, "Error: Invalid argument for '%s' option\n", optargString); \
	fprintf(stdout, USAGE_MESSAGE); \
	fprintf(stdout, HELP_MESSAGE); \
	_exit(EXIT_FAILURE);

#define DUPLICATE_OPT(optarg) \
	fprintf(stderr,"Error: Duplicate argument for '-%c' option\n", optarg); \
	fprintf(stdout, USAGE_MESSAGE); \
	fprintf(stdout, HELP_MESSAGE); \
	_exit(EXIT_FAILURE)

#define INVALID_OPT \
	fprintf(stderr,"Error: Invalid option argument\n"); \
	_exit(EXIT_FAILURE)

#define PATH_MAXLEN 255
#define OPT "n:q:d:t:"

/**
 * @brief: Configures the system parameters with value passed as argument of main 
 *			or use the defaul value
 * @requires: argc > 0 && argv != NULL
 * @effects: modifies threadsNum, queueLen, timeDelay, PATH_MAXLEN or
 *			 maintain default values
 * @throws: if there's a invalid command, prints the USAGE_MESSAGE and
 * 			HELP_MESSAGE and terminates the program 
*/
void systemConfiguration(int argc, char** argv);

/**
 * @brief: Says if the argument (a string) is a number or not
 * @requires: str != NULL
 * @effects: return true if "str is a number", false otherwise
 * throws: if str == NULL terminates the program
*/
bool isNumber(char* str);

struct threadWorker_args{
	char* filename;
	
}

unsigned int threadsNum = 4; //number of thread Worker given as argument of the option 'n'
unsigned int queueLen = 8; //length of concurrent queue given as argument of the option 'q'
unsigned int timeDelay = 0; //time in milliseconds give as argument of the option 't'
char* dirName = NULL;  //name of the directory given as argument of the option 'd'
unsigned int filesNumber = 0;
char** filenamesArr = NULL;  //array containing names of the files given as input arguments

int main(int argc, char* argv[]){
	//IMPLEMENT SIGNAL HANDLER!!!!!!!!!!!!!!!!!!
	systemConfiguration(argc, argv);

	master(threadsNum, queueLen, dirName, timeDelay, filesNumber, filenamesArr);
	if(dirName != NULL)
		free(dirName);
	if(filenamesArr != NULL){
		for(int i=0; i<filesNumber; i++){
			if(filenamesArr[i] != NULL)
				free(filenamesArr[i]);
		}
		free(filenamesArr);
	}
	return 0;
}


void systemConfiguration(int argc, char** argv){
	opterr = 0; //error message disabled
	unsigned int opt_n=0, opt_q=0, opt_d=0, opt_t=0, opt_file=0;
	unsigned int arg_n, arg_q, arg_t;
	char* arg_d = NULL;
	unsigned int c, len_arg;
	unsigned int i=0;

	while((c=getopt(argc, argv, OPT)) != -1){
		i++;
		#if defined(DEBUG_CONF) 
			printf("option char : %c\n", c);
		#endif
		switch(c){
			case 'n':
				if(opt_n == 1){
					DUPLICATE_OPT(c);
				}
				if(optarg == NULL || !isNumber(optarg)){
					INVALID_OPTARG(c);
				}
				opt_n = 1;
				arg_n = atoi(optarg);
				break;
			case 'q':
				if(opt_q == 1){
					DUPLICATE_OPT(c);
				}
				if(optarg == NULL || !isNumber(optarg)){
					INVALID_OPTARG(c);
				}
				opt_q = 1;
				arg_q = atoi(optarg);
				break;
			case 'd':
				if(optarg == NULL){
					INVALID_OPTARG(c);
				}
				opt_d = 1;
				len_arg = strnlen(optarg, PATH_MAXLEN); //give a bound on strlen
				#if defined(DEBUG_CONF)
				if(len_arg == PATH_MAXLEN){
					fprintf(stderr, "Error: Length of dir path is over");
				}
				#endif
				arg_d = (char*) calloc(len_arg+1, sizeof(char));
				assert(arg_d);
				arg_d = strncpy(arg_d, optarg, len_arg);
				break;
			case 't':
				if(opt_t == 1){
					DUPLICATE_OPT(c);
				}
				if(optarg == NULL || !isNumber(optarg)){
					INVALID_OPTARG(c);
				}
				opt_t = 1;
				arg_t = atoi(optarg);
				break;
			case '?':  //if option isn't correct or argument is absent
				if(optopt != '-' && optind > 2){
					INVALID_OPTARG_STRING(argv[optind-1]);
				}
				else{
					INVALID_OPTARG_STRING(argv[optind]);
				}
				break;
			#if defined(DEBUG_CONF)	
			case ':': //debug: modify the OPT string with ':' at the beginning
				INVALID_OPTARG_STRING(argv[optind-1]);
				break;
			case 'h':
				fprintf(stdout, HELP_MESSAGE);
				return;	
			#endif
			default:
				break;
		}
	}

	#if defined(DEBUG_CONF) //Checking argv's items
	for(unsigned int j=0; j<argc; j++){
		fprintf(stdout, "%d° : %s\n", j, argv[j]);
	}
	#endif

	//if some file has been passed as argument
	int tmp_optind = optind;
	unsigned int filenamesArrSize = 0;
	unsigned int filenameLength;

	while(optind < argc) {
		optind++;
		filenamesArrSize++;
	}
	if(filenamesArrSize != 0){
		filenamesArr = (char**) malloc(filenamesArrSize*sizeof(char*));
		assert(filenamesArr);
		opt_file = 1;
	}
	for(unsigned int j=0; j<filenamesArrSize; j++){
		filenamesArr[j] = NULL;
	}

	filesNumber = filenamesArrSize;
	for(unsigned int j=0; j<filenamesArrSize; j++){
		filenameLength = strnlen(argv[tmp_optind], PATH_MAXLEN);
		filenamesArr[j] = (char*) calloc(filenameLength+1, sizeof(char));
		assert(filenamesArr[j]);
		filenamesArr[j] = strncpy(filenamesArr[j], argv[tmp_optind], filenameLength);
		tmp_optind++;
 	}

 	//if files or directories have not been given as input of program
	if(!opt_d && filenamesArrSize<1){
		fprintf(stderr, "Error : Not files to compute the result...\n");
		fprintf(stdout, USAGE_MESSAGE);
		fprintf(stdout, HELP_MESSAGE);
		free(arg_d);
		_exit(EXIT_FAILURE);
	}
	#if defined(DEBUG_CONF)
		if(opt_n)
			printf("n: %d\n", arg_n);
		if(opt_q)
			printf("q: %d\n", arg_q);
		if(opt_d)
			printf("d: %s\n", arg_d);
		if(opt_t)
			printf("m: %d\n", arg_t);
		if(opt_file){
			fprintf(stdout, "Files list : \n");
			for(unsigned int j=0; j<filenamesArrSize; j++){
				fprintf(stdout, "%d° : %s\n", j+1, filenamesArr[j]);
			}
		}
	#endif

	if(opt_n)
		threadsNum = arg_n;
	if(opt_q)
		queueLen = arg_q;
	if(opt_d){
		unsigned int dirNameLen = strnlen(arg_d, PATH_MAXLEN);
		dirName = (char*) calloc(dirNameLen+1, sizeof(char));
		assert(dirName);
		dirName = strncpy(dirName, arg_d, dirNameLen);
	}
	if(opt_t)
		timeDelay = arg_t;

	if(arg_d != NULL){
		free(arg_d);
	}
	return;
}

bool isNumber(char* str){ //0->NaN, 1->strIsaNumber
	int i=0;
	while(str[i]!='\0'){
		if(str[i]<48 || str[i]>57){
			return false;
		}
		i++;
	}
	return true;
}


