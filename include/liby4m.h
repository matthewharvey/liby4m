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

#ifndef LIBY4M_H
#define LIBY4M_H

#include "y4mTypes.h"

int y4mOpenFile(y4mFile_t* file, const char* name);
int y4mOpenFp(y4mFile_t* y4mfile, FILE* fp);

char y4mGetY(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);
char y4mGetU(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);
char y4mGetV(y4mFile_t* file, const unsigned int xCoord, const unsigned int yCoord);

int y4mNextFrame(y4mFile_t* file);
int y4mWriteToFile(y4mFile_t* file, char* filename);
int y4mWriteToFp(y4mFile_t* file, FILE* fp);
int y4mWriteToStdout(y4mFile_t* file);

char* y4mGetFrameDataPointer(y4mFile_t* y4mfile);
unsigned int y4mGetWidth(y4mFile_t* y4mfile);
unsigned int y4mGetHeight(y4mFile_t* y4mfile);
int y4mIsEndOfFile(y4mFile_t* y4mfile);

void y4mCloseFile(y4mFile_t* file);

#endif //include guard
