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
    PARAM_COMMENT = 'X'
};

enum interlacing_modes
{
    INTERLACING_PROGRESSIVE,
    INTERLACING_TOP_FIELD_FIRST,
    INTERLACING_BOTTOM_FIELD_FIRST,
    INTERLACING_MIXED_MODES
};

enum colour_spaces
{
    COLOUR_C420JPEG,
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
