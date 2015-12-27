//
//  Matrix.cpp
//  BitMApFilter
//
//  Created by Guillaume Luccisano on 10/17/15.
//  Copyright (c) 2015 Atul Kumar. All rights reserved.
//

#include "Matrix.h"
/*
 
 
 The File Header (14 bytes)
 
 Confirms that the file is (at least probably) a BMP file.
 
 Tells exactly how large the file is.
 
 Tells where the actual image data is located within the file.
 
 The Image Header (40 bytes in the versions of interest)
 
 Tells how large the image is (rows and columns).
 
 Tells what format option is used (bits per pixel).
 
 Tells which type of compression, if any, is used.
 
 Provides other details, all of which are seldom used.
 
 The Color Table (length varies and is not always present)
 
 Provides the color palette for bit depths of 8 or less.
 
 Provides the (optional) bit masks for bit depths of 16 and 32.
 
 Not used for 24-bit images.
 
 The Pixel Data
 
 Pixel by pixel color information
 
 Row-by-row, bottom to top.
 
 Rows start on double word (4-byte) boundaries and are null padded if necessary.
 
 Each row is column-by-column, left to right.
 
 In 24-bit images, color order is Red, Green, Blue.
 
 In images less than 8-bits, the higher order bits are the left-most pixels.

 */
#include <fstream>
#include <string>


Mat::Mat(std::string filepath){
    
    std::ifstream image;
    image.open(filepath, std::ios::binary | std::ios::in);
    
    
    
    image.read(bm, 2);
    
    std::cout << bm[0] << bm[1] << std::endl;
    
    
    image.read(reinterpret_cast<char *> (&imgSize), 4);
    
    std::cout << imgSize << std::endl;
   
    image.read(unused, 4);
    
    
    image.read(reinterpret_cast<char *> (&offset), 4);
    std::cout << offset << std::endl;
    
    image.read(reinterpret_cast<char *> (&biSize), 4);
    std::cout << biSize << std::endl;
    
    image.read(reinterpret_cast<char *> (&biWidth), 4);
    std::cout << biWidth << std::endl;
    
    image.read(reinterpret_cast<char *> (&biHeight), 4);
    std::cout << biHeight << std::endl;
    
    image.read(reinterpret_cast<char *> (&biPlanes), 2);
    std::cout << biPlanes << std::endl;
    
    image.read(reinterpret_cast<char *> (&biBitCount), 2);
    std::cout << biBitCount << std::endl;
    
    
    image.read(reinterpret_cast<char *> (&biCompression), 4);
    std::cout << biCompression << std::endl;
    
    image.read(reinterpret_cast<char *> (&biSizeImage), 4);
    std::cout << biSizeImage << std::endl;
    
    image.read(reinterpret_cast<char *> (&biXPelsPerMeter), 4);
    std::cout << biXPelsPerMeter << std::endl;
    
    image.read(reinterpret_cast<char *> (&biYPelsPerMeter), 4);
    std::cout << biYPelsPerMeter << std::endl;
    
    image.read(reinterpret_cast<char *> (&biClrUsed), 4);
    std::cout << biClrUsed << std::endl;
    
    
    image.read(reinterpret_cast<char *> (&biClrImportant), 4);
    std::cout << biClrImportant << std::endl;
    
    data = new BYTE**[biHeight];
    for(int x = 0; x <biHeight; ++x ){
        data[x] = new BYTE*[biWidth];
        for(int y = 0; y <biWidth; ++y ){
            data[x][y] = new BYTE[3];
            image.read(reinterpret_cast<char *> (&data[x][y][0]), 1);
            image.read(reinterpret_cast<char *> (&data[x][y][1]), 1);
            image.read(reinterpret_cast<char *> (&data[x][y][2]), 1);
            //std::cout << 0 + data[x][y][0] << " " << 0 + data[x][y][1] << " " << 0 + data[x][y][2]  << std::endl;
        }
    }
}


void Mat::outputFile(std::string filepath){
    
    std::ofstream image;
    image.open(filepath, std::ios::binary | std::ios::out);
    
    
    
    image.write(bm, 2);
    
    
    
    image.write(reinterpret_cast<char *> (&imgSize), 4);
    
    image.write(unused, 4);
    
    
    image.write(reinterpret_cast<char *> (&offset), 4);
    
    
    image.write(reinterpret_cast<char *> (&biSize), 4);
    image.write(reinterpret_cast<char *> (&biWidth), 4);
    
    image.write(reinterpret_cast<char *> (&biHeight), 4);
    
    image.write(reinterpret_cast<char *> (&biPlanes), 2);
    
    image.write(reinterpret_cast<char *> (&biBitCount), 2);
    
    
    image.write(reinterpret_cast<char *> (&biCompression), 4);
    
    image.write(reinterpret_cast<char *> (&biSizeImage), 4);
    
    image.write(reinterpret_cast<char *> (&biXPelsPerMeter), 4);
    
    image.write(reinterpret_cast<char *> (&biYPelsPerMeter), 4);
    
    image.write(reinterpret_cast<char *> (&biClrUsed), 4);
    
    
    image.write(reinterpret_cast<char *> (&biClrImportant), 4);
    
    
    for(int x = 0; x <biHeight; ++x ){
        for(int y = 0; y <biWidth; ++y ){
            image.write(reinterpret_cast<char *> (&outdata[x][y][0]), 1);
            image.write(reinterpret_cast<char *> (&outdata[x][y][1]), 1);
            image.write(reinterpret_cast<char *> (&outdata[x][y][2]), 1);
        }
    }
}

void Mat::applyShapenningFilter(){
    int filter[3][3] = {
        {0,-1,0},
        {-1,5,-1},
        {0,-1,0}
    };
  
    outdata = new BYTE**[biHeight];
    for(int x = 0; x <biHeight; ++x ){
        outdata[x] = new BYTE*[biWidth];
        for(int y = 0; y <biWidth; ++y ){
            outdata[x][y] = new BYTE[3];
            int R = 0;
            int G = 0;
            int B = 0;
            
            for(int fx = -1; fx <= 1; ++fx){
                for(int fy = -1; fy <= 1; ++fy){
                    if(x + fx >= 0 && x + fx <biHeight && y + fy >= 0 && y + fy <biWidth){
                        R += data[x+fx][y+fy][0]*filter[1+fx][1+fy];
                        G += data[x+fx][y+fy][1]*filter[1+fx][1+fy];
                        B += data[x+fx][y+fy][2]*filter[1+fx][1+fy];
                    }
                }
            }
            if(R < 0) R = 0;
            if(G < 0) G = 0;
            if(B < 0) B = 0;
            
            if(R > 255) R = 255;
            if(G > 255) G = 255;
            if(B > 255) B = 255;
            
            outdata[x][y][0] = static_cast<BYTE>(R);
            outdata[x][y][1] = static_cast<BYTE>(G);
            outdata[x][y][2] = static_cast<BYTE>(B);
            
        }
    }
}




