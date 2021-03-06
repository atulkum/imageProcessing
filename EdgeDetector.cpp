#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

// Here we assume the image is of size 256*256 and is of raw format
int main(int argc, char *argv[]){
	// file pointer
	FILE *file;
	int type = 0;
	if(argc < 4){
		cout << "usage: program_name input_image.raw output_image.raw type" << endl;
		return 0;
	}
	type = atoi(argv[3]);	
	//read image	
	unsigned char inImg[256][256];	
	if (!(file=fopen(argv[1],"rb"))){
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(inImg, sizeof(unsigned char), 256*256, file);
	fclose(file);	

	
	//Sobel filter
	int Gx[3][3] = {
		{-1,0,1},
        {-2,0,2},
		{-1,0,1}
	};
	int Gy[3][3] = {
		{1,2,1},
        {0,0,0},
		{-1,-2,-1}
	};
	int PrewittLaplasian[3][3] = {
		{-1,-1,-1},
		{-1,8,-1},
		{-1,-1,-1}
	};
	int gradientMap[256][256];	
	unsigned char outImg[256][256];	
	
	for(int i=1; i<255; i++){
		for(int j=1; j<255; j++){
			if(type == 1){
				long sumx = 0, sumy = 0;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						sumx = sumx + inImg[i+a][j+b]*Gx[a+1][b+1];
					}
				}
				sumx /= 4;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						sumy = sumy + inImg[i+a][j+b]*Gy[a+1][b+1];
					}
				}
				sumy /= 4;
//				if(sumx < 0) sumx = -sumx;
//				if(sumy < 0) sumy = -sumy;
				gradientMap[i][j] = (int)sqrt(sumx*sumx + sumy*sumy);
				if(gradientMap[i][j] > 255) gradientMap[i][j] = 255;
				if(gradientMap[i][j] < 0) gradientMap[i][j] = 0;
			}
			else if(type == 2){
				long sum = 0;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						sum = sum + inImg[i+a][j+b]*PrewittLaplasian[a+1][b+1];
					}
				}
				sum /= 8;
				gradientMap[i][j] = sum;
			}
		}         
	}
///////////////////////
	int max = -1;
	for(int i=1; i<255; i++){
		for(int j=1; j<255; j++){
			int g = gradientMap[i][j];
			if(g < 0) g = -g;
			if(max < g) max = g;
		}
	}

	unsigned long histogram[max];
	for(int i = 0 ; i  < max; ++i){
		histogram[i] = 0;
	}
	for(int i = 0 ; i  < 256; ++i){
		for(int j = 0 ; j  < 256; ++j){
			int g  = gradientMap[i][j];
			if(g < 0) g = -g;
			histogram[g] += 1;
		}
	}
	int threshold = 0;
	for(int i = 0; i  < max; ++i){
		histogram[i + 1] += histogram[i];
		if((histogram[i]*100.0)/(256*256) > 79){
			threshold = i;
			break;
		}
	}

////////////////////////
	if(type == 1){
		for(int i = 0 ; i  < 256; ++i){
			for(int j = 0 ; j  < 256; ++j){
				if(gradientMap[i][j] > threshold) outImg[i][j] = 255;
				else outImg[i][j] = 0;
			}
		}
	}
	else if(type == 2){
		//zero crossing check
		for(int i=1; i<255; i++){
			for(int j=1; j<255; j++){
				int min = 1024, max = -1024;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						int g = gradientMap[i+a][j+b]; 
						if(g < min) min = g;
						if(g > max) max = g;
					}
				}
				if( min < -threshold && max > threshold) outImg[i][j] = 255;
				else outImg[i][j] = 0;
			}
		}
	}
	 	
	if (!(file=fopen(argv[2],"wb")))
	{
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(outImg, sizeof(unsigned char), 256*256, file);
	fclose(file);

	return 0;
}
