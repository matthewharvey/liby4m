#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"

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
    int err;
    memset(y4mfile, 0, sizeof(*y4mfile));
    y4mfile->file_ptr = fopen(name, "r");
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
    //This will read all of the frame data into memory
    while (y4mfile->eof == 0)
    {
        y4mNextFrame(y4mfile);
    }
    return write_y4mfile_from_y4mfile_struct(y4mfile, filename);
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
