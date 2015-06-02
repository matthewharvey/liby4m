#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "parse.h"

static int read_intro(FILE* file);
static int read_constant_string(FILE* file, char* expected_string);
static int read_param(FILE* file, y4mFile_t* y4mfile);
static int read_params(FILE* file, y4mFile_t* y4mfile);
static int read_frame_header_string(FILE* file);

/*
* function to populate all information about the 'file'
* and into the 'y4mfile'.
*/
int parse_y4m_header(FILE* file, y4mFile_t* y4mfile)
{
    assert(y4mfile != NULL);

    if (0 == read_intro(file))
    {
        if (0 == read_params(file, y4mfile))
        {
            return 0;
        }
        else
        {
            fprintf(stderr, "liby4m: Unable to read file's parameter list\n");
            return -2;
        }
    }
    else
    {
        fprintf(stderr, "liby4m: Malformed y4m file\n");
        return -1;
    }
}

static int read_intro(FILE* file)
{
    int retval = read_constant_string(file, "YUV4MPEG2");
    if (retval != 0)
        fprintf(stderr, "liby4m: First ten bytes of file are not YUV4MPEG2\n");
    return retval;
}

/*
* Generic function for reading the next strlen(expected_string) characters
* from 'file' and make sure that they are the same as 'expected_string'.
*/
static int read_constant_string(FILE* file, char* expected_string)
{
    char* intro = malloc(sizeof(char) * (strlen(expected_string) + 1));
    fread(intro, 1, strlen(expected_string), file);
    intro[(strlen(expected_string))] = '\0';
    int retval = strcmp(intro, expected_string);
    if (retval != 0)
        fprintf(stderr, "liby4m: Constant string is %s instead of %s\n", intro, expected_string);
    free(intro);
    return retval;
}

static int read_params(FILE* file, y4mFile_t* y4mfile)
{
    int err = 0;
    do
    {
        err = read_param(file, y4mfile);
    } while (err == 0);
    //10 is a special number that means the parameter
    //list is complete, and the rest of the file is
    //ready to start reading from the first frame.
    if (err == 10)
        return 0;
    else
        return err;
}

static int read_param(FILE* file, y4mFile_t* y4mfile)
{
    int space = fgetc(file); //strip off preceding space
    int initial;
    if (space == ' ')
        initial = fgetc(file);
    //10 means the parameters are finished
    else if (space == '\n')
        return 10;
    else
    {
        fprintf(stderr, "liby4m: unkown separator %c, expected a space\n", space);
        return -1;
    }

    //check which parameter is next based on the first character,
    //and then act accordingly.
    switch (initial)
    {
    case PARAM_WIDTH:
    {
        //will return 0 if the scan is successful
        return (1 != fscanf(file, "%u", &y4mfile->width));
        break;
    }
    case PARAM_HEIGHT:
    {
        return (1 != fscanf(file, "%u", &y4mfile->height));
        break;
    }
    case PARAM_FRAMERATE:
    {
        return (2 != fscanf(file, "%u:%u", &y4mfile->framerate_num, &y4mfile->framerate_den));
        break;
    }
    case PARAM_INTERLACING:
    {
        int mode = fgetc(file);
        int retval = 0;
        switch (mode)
        {
        case 'p':
            y4mfile->interlacing_mode = INTERLACING_PROGRESSIVE;
            break;
        case 't':
            y4mfile->interlacing_mode = INTERLACING_TOP_FIELD_FIRST;
            break;
        case 'b':
            y4mfile->interlacing_mode = INTERLACING_BOTTOM_FIELD_FIRST;
            break;
        case 'm':
            y4mfile->interlacing_mode = INTERLACING_MIXED_MODES;
            break;
        default:
            fprintf(stderr, "liby4m: Unrecognized interlacing mode %c\n", mode);
            retval = -2;
            break;
        }
        return retval;
        break;
    }
    case PARAM_ASPECTRATIO:
    {
        return (2 != fscanf(file, "%u:%u", &y4mfile->aspectratio_num, &y4mfile->aspectratio_den));
        break;
    }
    case PARAM_COLOURSPACE:
    {
        //10 is the largest possible string 'C420paldv' + null terminator
        char* colourspace_string = malloc(sizeof(char) * 10);
        int retval = 0;
        if (1 != fscanf(file, "%s", colourspace_string))
        {
            fprintf(stderr, "liby4m: Could not get word for colour space\n");
            retval = -3;
        }
        if (0 == strcmp(colourspace_string, "420jpeg"))
        {
            y4mfile->colourspace = COLOUR_C420JPEG;
        }
        else if (0 == strcmp(colourspace_string, "420jpeg"))
        {
            y4mfile->colourspace = COLOUR_C420PALDV;
        }
        else if (0 == strcmp(colourspace_string, "420jpeg"))
        {
            y4mfile->colourspace = COLOUR_C420;
        }
        else if (0 == strcmp(colourspace_string, "420jpeg"))
        {
            y4mfile->colourspace = COLOUR_C422;
        }
        else if (0 == strcmp(colourspace_string, "420jpeg"))
        {
            y4mfile->colourspace = COLOUR_C444;
        }
        else
        {
            fprintf(stderr, "liby4m: Unrecognized colourspace %s\n", colourspace_string);
            retval = -3;
        }
        free(colourspace_string);
        return retval;
        break;
    }
    case PARAM_COMMENT:
    {
        //This is a silly limitation. It should be removed in the future.
        char tmp_string[1024];
        int retval = 0;
        if (1 != fscanf(file, "%1023s", tmp_string))
        {
            fprintf(stderr, "liby4m: Could not get word for colour space\n");
            retval = -3;
        }
        else
        {
            char* comment_string = malloc(sizeof(char) * (strlen(tmp_string) + 1));
            strcpy(comment_string, tmp_string);
            y4mfile->comment = comment_string;
        }
        return retval;
        break;
    }
    default:
    {
        fprintf(stderr, "liby4m: Unknown y4m parameter %c\n", initial);
        return -8;
        break;
    }
    }
}

static int read_frame_header_string(FILE* file)
{
    int retval = read_constant_string(file, "FRAME");
    if (retval != 0)
        fprintf(stderr, "liby4m: frame data is not prefixed by 'FRAME'\n");
    return retval;
}

//TODO
int parse_frame_header(FILE* file, y4mFile_t* y4mfile)
{
    read_frame_header_string(file);
    //read frame params
    int err = 0;
    do
    {
        err = read_param(file, y4mfile);
    } while (err == 0);
    set_y4m_params_by_colourspace(y4mfile);
    //10 is a special number that means the parameter
    //list is complete, and the rest of the file is
    //ready to start reading from the first frame.
    if (err == 10)
        return 0;
    else
        return err;
}

void set_y4m_params_by_colourspace(y4mFile_t* y4mfile)
{
    //This is true for all colourspaces
    y4mfile->yplane_size = y4mfile->width * y4mfile->height;
    switch (y4mfile->colourspace)
    {
    case COLOUR_C420JPEG:
    case COLOUR_C420PALDV:
    case COLOUR_C420:
        y4mfile->chromaplanes_size = (y4mfile->width * y4mfile->height) / 4;
        break;
    case COLOUR_C422:
        y4mfile->chromaplanes_size = (y4mfile->width * y4mfile->height) / 2;
        break;
    case COLOUR_C444:
        y4mfile->chromaplanes_size = (y4mfile->width * y4mfile->height);
        break;
    default:
        fprintf(stderr, "liby4m: Invalid colourspace in struct\n");
        //instead of killing the program here, we are just going to
        //select a default and continue. The default was so chosen
        //because that's the colour space of my test files.
        y4mfile->colourspace = COLOUR_C420JPEG;
        break;
    }
}

int load_frame_data(FILE* file, y4mFile_t* y4mfile)
{
    size_t read_size;
    unsigned int size_of_frame = y4mfile->yplane_size + 2*y4mfile->chromaplanes_size;
    y4mfile->current_frame_data = (char*)malloc(size_of_frame*sizeof(char));
    if (y4mfile->current_frame_data == NULL)
    {
        fprintf(stderr, "liby4m: Failed to allocate memory\n");
        return -1;
    }
    read_size = fread(y4mfile->current_frame_data, sizeof(char), size_of_frame, file);
    if (read_size != size_of_frame)
    {
        fprintf(stderr, "liby4m: Failed to read frame data\n");
        return -2;
    }
    return 0;
}
