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
    parse_frame_header(y4mfile->file_ptr, y4mfile);
    //file will now be pointing at some frame data
    free(y4mfile->current_frame_data);
    load_frame_data(y4mfile->file_ptr, y4mfile);
    return 0;
}

/*!
* \brief Deinit the y4mFile_t structure
*
* \param y4mfile The struct to be de-initialized. Must have been previously initialized.
*/
void y4mCloseFile(y4mFile_t* y4mfile)
{
    fclose(y4mfile->file_ptr);
    if (y4mfile->comment != NULL)
        free(y4mfile->comment);
}
