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

#ifndef Y4MTYPES_H
#define Y4MTYPES_H

enum param_types
{
    PARAM_WIDTH = 'W',
    PARAM_HEIGHT = 'H',
    PARAM_FRAMERATE = 'F',
    PARAM_INTERLACING = 'I',
    PARAM_ASPECTRATIO = 'A',
    PARAM_COLOURSPACE = 'C',
    PARAM_COMMENT = 'X',
    NUM_PARAMS
};

enum interlacing_modes
{
    INTERLACING_PROGRESSIVE = 'p',
    INTERLACING_TOP_FIELD_FIRST = 't',
    INTERLACING_BOTTOM_FIELD_FIRST = 'b',
    INTERLACING_MIXED_MODES = 'm'
};

enum colour_spaces
{
    COLOUR_C420JPEG,
    COLOUR_C420MPEG2,
    COLOUR_C420PALDV,
    COLOUR_C420,
    COLOUR_C422,
    COLOUR_C444
};

typedef struct framedata framedata_t;

struct framedata
{
    char* data;
    framedata_t* next;
};

typedef struct
{
    FILE* file_ptr;
    unsigned int width;
    unsigned int height;
    unsigned int framerate_num;
    unsigned int framerate_den;
    enum interlacing_modes interlacing_mode;
    unsigned int aspectratio_num;
    unsigned int aspectratio_den;
    enum colour_spaces colourspace;
    unsigned int yplane_size;
    unsigned int chromaplanes_size;
    unsigned int eof; //indicates that there are no more frames to read
    framedata_t* first_frame_data; //functionally the 'head' pointer to the linked list
    framedata_t* current_frame_data; //functionally the 'tail' pointer to the linked list
    char* comment; //must be free'd upon deletion
} y4mFile_t;

#endif //include guard
