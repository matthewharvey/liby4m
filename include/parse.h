#ifndef PARSE_H
#define PARSE_H
#include "y4mTypes.h"

int parse_y4m_header(FILE* file, y4mFile_t* y4mfile);
int parse_frame_header(FILE* file, y4mFile_t* y4mfile);
void set_y4m_params_by_colourspace(y4mFile_t* y4mfile);
int load_frame_data(FILE* file, y4mFile_t* y4mfile);
void free_frame_data(y4mFile_t* y4mfile);
int write_y4mfile_from_y4mfile_struct(y4mFile_t* y4mfile, FILE* filename);

#endif //include guard
