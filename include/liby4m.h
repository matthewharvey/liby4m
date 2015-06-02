#ifndef LIBY4M_H
#define LIBY4M_H

#include "y4mTypes.h"

int y4mOpenFile(y4mFile_t* file, const char* name);

char y4mGetY(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);
char y4mGetU(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);
char y4mGetV(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);

int y4mNextFrame(y4mFile_t* file);

char* y4mGetFrameDataPointer(y4mFile_t* y4mfile);

void y4mCloseFile(y4mFile_t* file);

#endif //include guard
