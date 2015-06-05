/*
*   The MIT License (MIT)
*
*   Copyright (c) 2015 Matthew Harvey
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*/

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
