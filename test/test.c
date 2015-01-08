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
            file.colour_space,
            file.comment);
    /*char** buffer = malloc(sizeof(char*) * file.width);
    for (int j = 0; j < file.width; ++j)
    {
        buffer[j] = malloc(sizeof(char) * file.height * BYTES_PER_PIXEL);
    }
    for (int i = 0; i < file.frames; ++i)
    {
        //char** buffer = malloc(sizeof(char) * file.height * file.width * BYTES_PER_PIXEL);
        for (int y = 0; y < file.height; ++y)
        {
            for (int x = 0; x < file.width; ++x)
            {
                //should this really be an int?
                buffer[x][y*BYTES_PER_PIXEL + 0] = y4mGetRed(&file, x, y);
                buffer[x][y*BYTES_PER_PIXEL + 1] = y4mGetGreen(&file, x, y);
                buffer[x][y*BYTES_PER_PIXEL + 2] = y4mGetBlue(&file, x, y);
                printf("%d %d %d\n", buffer[x][y*BYTES_PER_PIXEL + 0], buffer[x][y*BYTES_PER_PIXEL + 1], buffer[x][y*BYTES_PER_PIXEL + 2]);
            }
        }
        assert (0 == y4mNextFrame(&file));

    }
    for (int j = 0; j < file.width; ++j)
    {
        free(buffer[j]);
    }
    free(buffer);
    */
    y4mCloseFile(&file);
}
