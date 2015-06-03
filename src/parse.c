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
static int write_header_line(FILE* new_file, y4mFile_t* y4mfile);
static int write_frame_line(FILE* new_file, y4mFile_t* y4mfile);
static int write_frame_data(FILE* new_file, framedata_t* cur, unsigned long datasize);

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
    unsigned int expected_length = strlen(expected_string);
    char* intro = malloc(sizeof(char) * (expected_length + 1)); //+1 for null character
    int retval = fread(intro, 1, expected_length, file);
    if (retval == expected_length)
    {
        intro[expected_length] = '\0';
        retval = strcmp(intro, expected_string);
        if (retval != 0)
            fprintf(stderr, "liby4m: Constant string is %s instead of %s\n", intro, expected_string);
    }
    else if (feof(file))
    {
        //if the end of file has been reached, don't emit an error message
        retval = -1024; //Not a number of significance
    }
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
        fprintf(stderr, "liby4m: unknown separator %c, expected a space\n", space);
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
    if ((retval != 0) && (feof(file)))
    {
        //If we have reached the end of the file, then don't
        //emit an error message
        retval = -1024;//Not a number of significance
    }
    else if (retval != 0)
    {
        fprintf(stderr, "liby4m: frame data is not prefixed by 'FRAME'\n");
    }
    return retval;
}

//TODO
int parse_frame_header(FILE* file, y4mFile_t* y4mfile)
{
    int err = 0;
    err = read_frame_header_string(file);
    //read frame params
    while (err == 0)
    {
        err = read_param(file, y4mfile);
    }
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
    char* new_frame_data = (char*)malloc(size_of_frame*sizeof(char));
    if (new_frame_data == NULL)
    {
        fprintf(stderr, "liby4m: Failed to allocate memory\n");
        return -1;
    }
    read_size = fread(new_frame_data, sizeof(char), size_of_frame, file);
    if (read_size != size_of_frame)
    {
        fprintf(stderr, "liby4m: Failed to read frame data\n");
        return -2;
    }
    if (y4mfile->current_frame_data->data == NULL) //if this is the first insertion
    {
        y4mfile->current_frame_data->data = new_frame_data;
    }
    else
    {
        y4mfile->current_frame_data->next = (framedata_t*)malloc(1*sizeof(framedata_t));
        y4mfile->current_frame_data = y4mfile->current_frame_data->next;
        y4mfile->current_frame_data->data = new_frame_data;
        y4mfile->current_frame_data->next = NULL;
    }
    return 0;
}

int write_y4mfile_from_y4mfile_struct(y4mFile_t* y4mfile, char* filename)
{
    FILE* new_file = fopen(filename, "w");
    int err = write_header_line(new_file, y4mfile);
    framedata_t* cur = y4mfile->first_frame_data;
    while (cur != NULL)
    {
        err |= write_frame_line(new_file, y4mfile);
        err |= write_frame_data(new_file, cur, y4mfile->yplane_size + 2* y4mfile->chromaplanes_size);
        cur = cur->next;
    }
    return err;
}

static int write_header_line(FILE* new_file, y4mFile_t* y4mfile)
{
    char message[] = "YUV4MPEG2";
    int retval = fwrite(message, 1, strlen(message), new_file);
    if (retval == strlen(message))
    {
        const int buf_size = 20;
        char param[buf_size]; //20 is way more than necessary
        char* param_ptr = &param[0];
        char* colourspace_string;
        snprintf(param_ptr, buf_size, " %c%d", PARAM_WIDTH, y4mfile->width);
        fwrite(param_ptr, 1, strlen(param), new_file);
        snprintf(param_ptr, buf_size, " %c%d", PARAM_HEIGHT, y4mfile->height);
        fwrite(param_ptr, 1, strlen(param), new_file);
        snprintf(param_ptr, buf_size, " %c%d:%d", PARAM_FRAMERATE, y4mfile->framerate_num, y4mfile->framerate_den);
        fwrite(param_ptr, 1, strlen(param), new_file);
        snprintf(param_ptr, buf_size, " %c%c", PARAM_INTERLACING, y4mfile->interlacing_mode);
        fwrite(param_ptr, 1, strlen(param), new_file);
        snprintf(param_ptr, buf_size, " %c%d:%d", PARAM_ASPECTRATIO, y4mfile->aspectratio_num, y4mfile->aspectratio_den);
        fwrite(param_ptr, 1, strlen(param), new_file);
        switch (y4mfile->colourspace)
        {
        case COLOUR_C420JPEG:
            colourspace_string = "420jpeg";
            break;
        case COLOUR_C420PALDV:
            colourspace_string = "420paldv";
            break;
        case COLOUR_C420:
            colourspace_string = "420";
            break;
        case COLOUR_C422:
            colourspace_string = "422";
            break;
        case COLOUR_C444:
            colourspace_string = "444";
            break;
        }
        snprintf(param_ptr, buf_size, " %c%s", PARAM_COLOURSPACE, colourspace_string);
        fwrite(param_ptr, 1, strlen(param), new_file);
        if (y4mfile->comment != NULL)
        {
            snprintf(param_ptr, buf_size, " %c%s", PARAM_COMMENT, y4mfile->comment);
            fwrite(param_ptr, 1, strlen(param), new_file);
        }
        snprintf(param_ptr, buf_size, "\n");
        fwrite(param_ptr, 1, strlen(param), new_file);
    }
    else
        return retval;
    return 0;
}

/*
*   For now this just writes "FRAME\n" into the file
*   In the future this might need to support options
*   that change per frame, but now is not that time.
*/
static int write_frame_line(FILE* new_file, y4mFile_t* y4mfile)
{
    char message[] = "FRAME\n";
    int retval = fwrite(message, 1, strlen(message), new_file);
    return (retval != strlen(message));
}

static int write_frame_data(FILE* new_file, framedata_t* cur, unsigned long datasize)
{
    int retval = fwrite(cur->data, 1, datasize, new_file);
    return (retval != datasize);
}

void free_frame_data(y4mFile_t* y4mfile)
{
    framedata_t* cur = y4mfile->first_frame_data;
    while(cur != NULL)
    {
        framedata_t* to_delete = cur;
        free(cur->data);
        cur = cur->next;
        free(to_delete);
    }
}
