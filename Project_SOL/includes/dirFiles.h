#ifndef DIRFILES_H_
#define DIRFILES_H_

/**
 * @brief: This function is used for detecting errors opening a directory
 * @requires: directory != NULL
 * @effects: prints informations about the error and terminates the program
 * @throws: if directory == NULL then terminate the program
*/
void ERR_OPEN_DIR(char* directory, int errnumb);

/**
 * @brief: This function is used for detecting errors closing a directory
 * @requires: directory != NULL
 * @effects: prints informations about the error and terminates the program
 * @throws: if directory == NULL then terminate the program
*/
void ERR_CLOSE_DIR(char* directory, int errnumb);

/**
 * @brief: Says if a string terminates with '/' or not
 * @requires: str != NULL
 * @effects: if str[strlen(str)] == '/' return true, otherwise false
 * @throws: if str == NULL then terminate the program
*/
bool end_slash(char* str);

#endif

