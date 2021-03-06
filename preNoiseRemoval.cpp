// The programe reads out the image data from an image file and write into another 
// last updated on 5/2/2007 by May-chen Kuo                                                               


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

// Here we assume the image is of size 256*256 and is of raw format
// You will need to make corresponding changes to accommodate images of different sizes and types
	void LPF(unsigned char inImg[256][256], unsigned char outImg[256][256], int type){
		short filterh1[3][3] = {{1,1,1},
								{1,1,1},
								{1,1,1}};
		short filterh2[3][3] = {{1,1,1},
								{1,2,1},
								{1,1,1}};
		short filterh3[3][3] = {{1,2,1},
								{2,4,2},
								{1,2,1}};

		short **filter;
		int d = 0;
		if(type == 1){
			filter = (short**)filterh1;	
		}
		else if(type == 2){
			filter = (short**)filterh2;	
		}
		else if(type == 3){
			filter = (short**)filterh3;	
		}

		for(int i=0; i<3; i++){
			for(int j=0; j<3; j++){
				d = d + filter[i][j];
			}
		}

		for(int i=1; i<255; i++){
			for(int j=1; j<255; j++){
				int sum = 0;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						sum = sum + inImg[i+a][j+b]*filter[a+1][b+1];
					}
				}
				sum = sum/d;
				if(sum < 0)   sum = 0;
				if(sum > 255) sum = 255;
				outImg[i][j] = sum;
			}
		}   
	}
	void median(unsigned char inImg[256][256], unsigned char outImg[256][256]){
		short filterM[9];
		for(int i = 0; i < 256; i++) {
			for(int j = 0; j < 256; j++) { 
				int n = 0;
				for(int fi = 0; fi < 3; fi++){
					for(int fj = 0; fj < 3; fj++){
						int x = (i + fi - 1 + 256) % 256;
						int y = (j + fj - 1 + 256) % 256;					
						filterM[n] = inImg[x][y];
						n++;
					}
				} 
				n--;
				for(; n > 1; --n){ 			
					for(int k=0; k < n; k++){
						if(filterM[k] > filterM[k+1]){
							int temp = filterM[k];
							filterM[k] = filterM[k+1];
							filterM[k+1] = temp;
						}
					}
				}        
				outImg[i][j] = filterM[5]; 
			}
		}
	}
	void outlinear(unsigned char inImg[256][256], unsigned char outImg[256][256]){
		unsigned long histogram[256];
		for(int i = 0 ; i  < 256; ++i){
			histogram[i] = 0;
		}
		for(int i = 0 ; i  < 256; ++i){
			for(int j = 0 ; j  < 256; ++j){
				histogram[inImg[i][j]] = histogram[inImg[i][j]] + 1;
			}
		}	
		int fmax = -1;
		for(int i = 0 ; i  < 256; ++i){
			if(histogram[i] > 0){
				if(fmax < i){
					fmax = i;
				}
			}
		}
		double th = fmax*0.07;
	
		for(int i = 0; i < 256; i++) {
			for(int j = 0; j < 256; j++) { 
				double sum = 0;
				for(int fi = 0; fi < 3; fi++){
					for(int fj = 0; fj < 3; fj++){
						int x = (i + fi - 1 + 256) % 256;
						int y = (j + fj - 1 + 256) % 256;					
						sum += inImg[x][y];
					}
				}
				sum -= 	inImg[i][j];
				sum = sum/8;
				double diff = sum - inImg[i][j];
				if(diff < 0) diff = -diff;
				if(diff > th){
					outImg[i][j] = (unsigned char)(sum + 0.5); 
				}
				else{
					outImg[i][j] = inImg[i][j] ;
				}
			}
		}
	}
int main(int argc, char *argv[])

{
	// file pointer
	FILE *file;
	//int Size = 256;
	int type = 0;
	
	if(argc < 4){
		cout << "usage: program_name input_image.raw output_image.raw type" << endl;
		return 0;
	}
	type = atoi(argv[3]);
	
	
	// image data array
	unsigned char Imagedata[256][256];

	// read image "ride.raw" into image data matrix
	if (!(file=fopen(argv[1],"rb")))
	{
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), 256*256, file);
	fclose(file);
	unsigned char OutputImagedata[256][256];	
	/////////////////////////LPF////////////////////////////
	if(type == 1 || type == 2 || type ==3){
		LPF(Imagedata, OutputImagedata, type);
	}
	////////////////Median/////////////////////////	
	if(type == 4){
		median(Imagedata, OutputImagedata);
    }   
	/////////////////////outlinear filter///////////
	if(type == 5){
		outlinear(Imagedata, OutputImagedata);
	}
	/////////////////write output files//////////
	if (!(file=fopen(argv[2],"wb")))
	{
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), 256*256, file);
	fclose(file);

	return 0;
}

