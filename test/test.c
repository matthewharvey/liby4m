#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liby4m.h"

#define BYTES_PER_PIXEL (3)

enum operations
{
    OPERATION_DARKEN,
    OPERATION_LIGHTEN,
    OPERATION_INVERT
};

void change_data(char* frame_data, y4mFile_t* y4mfile, int operation)
{
    int yplane_bytes = y4mGetWidth(y4mfile) * y4mGetHeight(y4mfile);
    for (int i = 0; i < yplane_bytes; ++i)
    {
        switch (operation)
        {
        case OPERATION_DARKEN:
            frame_data[i] -= 10;
            break;
        case OPERATION_LIGHTEN:
            frame_data[i] += 10;
            break;
        case OPERATION_INVERT:
            frame_data[i] = 255 - frame_data[i];
            break;
        }
    }
}

int main(int argc, char** argv)
{
    assert(argc == 2);
    char* filename = argv[1];
    y4mFile_t file;
    assert(0 == y4mOpenFile(&file, filename));
    printf( "name: %s\n"
            "width:  %u\n"
            "height: %u\n"
            "framerate: %u:%u\n"
            "interlacing mode: %d\n"
            "aspect ratio: %u:%u\n"
            "colour space: %d\n"
            "comment: %s\n",
            filename,
            y4mGetWidth(&file),
            y4mGetHeight(&file),
            //WARNING: Directly accessing the struct like this
            //isn't a good idea for future proofing. If you
            //want to access some of these members from a real program then
            //write an accessor function in src/liby4m.c.
            file.framerate_num, file.framerate_den,
            file.interlacing_mode,
            file.aspectratio_num, file.aspectratio_den,
            file.colourspace,
            file.comment);
    //int op = 0;
    while(y4mIsEndOfFile(&file) == 0)
    {
        char* data_ptr = y4mGetFrameDataPointer(&file);
        /*if (op)
            change_data(data_ptr, &file, OPERATION_DARKEN);
        else
            change_data(data_ptr, &file, OPERATION_LIGHTEN);
        op = !op;
        */
        change_data(data_ptr, &file, OPERATION_INVERT);
        y4mNextFrame(&file);
    }
    y4mWriteToFile(&file, "new.y4m");
    y4mCloseFile(&file);
}
