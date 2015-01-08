#ifndef PARSE_H
#define PARSE_H
#include "y4mTypes.h"

int parse(FILE* file, y4mFile_t* y4mfile);
int nextFrame(char* data, unsigned int data_len, FILE* file, y4mFile_t* y4mfile);
#endif //include guard
