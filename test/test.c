#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liby4m.h"

#define BYTES_PER_PIXEL (3)

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
            file.width,
            file.height,
            file.framerate_num, file.framerate_den,
            file.interlacing_mode,
            file.aspectratio_num, file.aspectratio_den,
            file.colourspace,
            file.comment);
    y4mCloseFile(&file);
}
