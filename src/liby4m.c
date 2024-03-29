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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"

static void fast_forward_to_end(y4mFile_t* y4mfile);

/*!
* \brief Initialize a y4mFile_t struct by reading the FILE* 'fp'
*
* \param y4mfile: structure to be initialized.
* \param fp: file pointer to the file to be read.
*
* Reads the header information from `fp` to extract information which is
* available in the struct's members
*
* \return 0 if successful. Otherwise failure.
*/
int y4mOpenFp(y4mFile_t* y4mfile, FILE* fp)
{
    int err;
    memset(y4mfile, 0, sizeof(*y4mfile));
    y4mfile->file_ptr = fp;
    y4mfile->current_frame_data = (framedata_t*)malloc(sizeof(framedata_t)*1);
    y4mfile->current_frame_data->next = NULL;
    y4mfile->current_frame_data->data = NULL;
    y4mfile->first_frame_data = y4mfile->current_frame_data;
    err = parse_y4m_header(y4mfile->file_ptr, y4mfile);
    if (0 == err)
    {
        err = parse_frame_header(y4mfile->file_ptr, y4mfile);
        if (0 == err)
        {
            err = load_frame_data(y4mfile->file_ptr, y4mfile);
            if (0 != err)
            {
                fprintf(stderr, "liby4m: Failed to load frame data\n");
            }
        }
        else
        {
            fprintf(stderr, "liby4m: Failed to parse frame header\n");
        }
    }
    else
    {
        fprintf(stderr, "liby4m: Failed to parse file header\n");
    }
    return err;
}

/*!
* \brief Initialize a y4mFile_t struct by opening the file 'name'
*
* \param y4mfile: structure to be initialized.
* \param name: name of the file to be opened.
*
* Opens the file in question and reads the header information
* to extract information which is available in the struct's members
*
* \return 0 if successful. Otherwise failure.
*/
int y4mOpenFile(y4mFile_t* y4mfile, const char* name)
{
    FILE* fp = fopen(name, "r");
    return y4mOpenFp(y4mfile, fp);
}

char y4mGetY(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord) { return 0; }
char y4mGetU(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord) { return 0; }
char y4mGetV(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord) { return 0; }

int y4mNextFrame(y4mFile_t* y4mfile)
{
    int err;
    err = parse_frame_header(y4mfile->file_ptr, y4mfile);
    if (0 == err)
    {
        //file will now be pointing at some frame data
        err = load_frame_data(y4mfile->file_ptr, y4mfile);
        return err;
    }
    else
    {
        y4mfile->eof = 1;
        return 0;
    }
}

/*!
* \brief Get a pointer to the data for the current frame
*
* \param y4mfile The struct for which to get the frame data
*
* Returns a pointer to the raw frame data that was read directly
* from the Y4M file. It is the responsibility of the application
* to interpret the format of this data and act upon it accordingly.
* Modifications to this data will be reflected in the struct,
* and will be seen in the file on disk if y4mWriteToDisk is called.
*/
char* y4mGetFrameDataPointer(y4mFile_t* y4mfile)
{
    return y4mfile->current_frame_data->data;
}

/*!
* \brief Take the changes currently made to the data and
* write them to disk
*
* \param y4mfile The struct to write to a file
* \param filename The name of the new file to write
*
* Warning: This function will not preserve the position
* of the current frame.
*/
int y4mWriteToFile(y4mFile_t* y4mfile, char* filename)
{
    fast_forward_to_end(y4mfile);
    FILE* new_file = fopen(filename, "w");
    return write_y4mfile_from_y4mfile_struct(y4mfile, new_file);
}

int y4mWriteToFp(y4mFile_t* y4mfile, FILE* fp)
{
    fast_forward_to_end(y4mfile);
    return write_y4mfile_from_y4mfile_struct(y4mfile, fp);
}

int y4mWriteToStdout(y4mFile_t* y4mfile)
{
    return y4mWriteToFp(y4mfile, stdout);
}

static void fast_forward_to_end(y4mFile_t* y4mfile)
{
    //This will read all of the frame data into memory
    while (y4mfile->eof == 0)
    {
        y4mNextFrame(y4mfile);
    }
}

/*!
* \brief Return the width of the file
*/
unsigned int y4mGetWidth(y4mFile_t* y4mfile)
{
    return y4mfile->width;
}

/*!
* \brief Return the height of the file
*/
unsigned int y4mGetHeight(y4mFile_t* y4mfile)
{
    return y4mfile->height;
}
/*!
* \brief Return the color space of the file
*/
enum colour_spaces y4mGetColourSpace(y4mFile_t* y4mfile)
{
    return y4mfile->colourspace;
}
/*!
*   \brief Return whether the final frame has been read
*
*   This probably only returns true once the final frame
*   has been read, and y4mNextFrame has been called once more.
*/
int y4mIsEndOfFile(y4mFile_t* y4mfile)
{
    return (y4mfile->eof != 0);
}

/*!
* \brief Deinit the y4mFile_t structure
*
* \param y4mfile The struct to be de-initialized. Must have been previously initialized.
*/
void y4mCloseFile(y4mFile_t* y4mfile)
{
    fclose(y4mfile->file_ptr);
    free_frame_data(y4mfile);
    if (y4mfile->comment != NULL)
        free(y4mfile->comment);
}
