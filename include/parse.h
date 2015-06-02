#ifndef PARSE_H
#define PARSE_H
#include "y4mTypes.h"

int parse_y4m_header(FILE* file, y4mFile_t* y4mfile);
int parse_frame_header(FILE* file, y4mFile_t* y4mfile);
void set_y4m_params_by_colourspace(y4mFile_t* y4mfile);
int load_frame_data(FILE* file, y4mFile_t* y4mfile);
#endif //include guard
