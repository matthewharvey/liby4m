#ifndef PARSE_H
#define PARSE_H
#include "y4mTypes.h"

int parse_y4m_header(FILE* file, y4mFile_t* y4mfile);
int parse_frame_header(char* data, unsigned int data_len, FILE* file, y4mFile_t* y4mfile);
#endif //include guard
