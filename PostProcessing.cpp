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

	unsigned char outImg[256][256];		
	for(int i=0; i<256; i++){
		for(int j=0; j<256; j++){
			outImg[i][j] = inImg[i][j];
		}
	}
	for(int i=1; i<255; i++){
		for(int j=1; j<255; j++){
			if(type == 1){//isolated noise deduction
				int isIso = 1;
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						if( a == 0 && b == 0){
							continue;
						}
						if(inImg[i+a][j+b] == 0){
							isIso = 0;
							break;
						}					
					}
					if(isIso == 0){
						break;
					}
				}
				if(isIso == 1){
					outImg[i][j] = 255;
				}
			}
			else if(type == 2){
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


