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
    int i;
    int yplane_bytes = y4mGetWidth(y4mfile) * y4mGetHeight(y4mfile);
    for (i = 0; i < yplane_bytes; ++i)
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
    fprintf(stderr,
            "name: %s\n"
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
    //y4mWriteToStdout(&file);
    y4mCloseFile(&file);
}
