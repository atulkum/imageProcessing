//
//  Matrix.h
//  BitMApFilter
//
//  Created by Guillaume Luccisano on 10/17/15.
//  Copyright (c) 2015 Atul Kumar. All rights reserved.
//

#ifndef __BitMApFilter__Matrix__
#define __BitMApFilter__Matrix__

#include <stdio.h>
#include<string>
#include <iostream>


typedef unsigned char BYTE;

class Mat{
public:
    Mat(std::string filepath);
    ~Mat(){
        /*for(int i = 0; i <biHeight; ++i ){
            for(int j = 0; j <biHeight; ++j ){
                //delete[] data[i][j];
            }
            delete[] data[i];
        }
        delete[] data;*/
    }
    void applyShapenningFilter();
    void outputFile(std::string filepath);
    
private:
    BYTE ***data;
    BYTE ***outdata;
    int imgSize;
    int biHeight;
    int biWidth;
    char bm[2];
    char unused[4];
    unsigned int offset;
    int biSize;  //4 	Header Size - Must be at least 40
    short biPlanes; 	//2 	Must be 1
    short biBitCount; 	//2 	Bits per pixel - 1, //4, 8, 16, 24, or 32
    int biCompression; 	//4 	Compression type (0 = uncompressed)
    int biSizeImage; 	//4 	Image Size - may be zero for uncompressed images
    int biXPelsPerMeter; 	//4 	Preferred resolution in pixels per meter
    int biYPelsPerMeter; 	//4 	Preferred resolution in pixels per meter
    int biClrUsed; 	//4 	Number Color Map entries that are actually used
    int biClrImportant; 	//4 	Number of significant colors
};

#endif /* defined(__BitMApFilter__Matrix__) */
