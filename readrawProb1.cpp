// The programe reads out the image data from an image file and write into another 
// last updated on 5/2/2007 by May-chen Kuo                                                               


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

// Here we assume the image is of size 256*256 and is of raw format
// You will need to make corresponding changes to accommodate images of different sizes and types


int main(int argc, char *argv[])

{
	// file pointer
	FILE *file;
	int BytesPerPixel;
	int Size = 256;
	int type = 0;
	if(argc < 5){
		cout << "usage: program_name input_image.raw output_image.raw type BytesPerPixel [logo_file.raw]" << endl;
		return 0;
	}
	type = atoi(argv[3]);	
	BytesPerPixel = atoi(argv[4]);	
	///////////////////to grey scale
	if(type == 1){
		unsigned char Imagedata[Size][Size][BytesPerPixel];	
		if (!(file=fopen(argv[1],"rb")))
		{
			cout << "Cannot open file: " << argv[1] <<endl;
			exit(1);
		}
		fread(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
		fclose(file);	

		unsigned char OutputImagedata[Size][Size];
		for(int i = 0; i < Size; ++i){
			for(int j = 0; j < Size; ++j){
				OutputImagedata[i][j] = (unsigned char)(0.299*Imagedata[i][j][0] + 0.587*Imagedata[i][j][1] + 0.114*Imagedata[i][j][2]); 
			}
		}

		if (!(file=fopen(argv[2],"wb")))
		{
			cout << "Cannot open file: " << argv[2] << endl;
			exit(1);
		}
		fwrite(OutputImagedata, sizeof(unsigned char), Size*Size, file);
		fclose(file);
	}
	////////////////to negative
	else if(type == 3){		
		unsigned char Imagedata[Size][Size][BytesPerPixel];	
		if (!(file=fopen(argv[1],"rb")))
		{
			cout << "Cannot open file: " << argv[1] <<endl;
			exit(1);
		}
		fread(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
		fclose(file);	


		unsigned long histogram[256][3];
		for(int i = 0 ; i  < 256; ++i){
			histogram[i][0] = 0;
			histogram[i][1] = 0;
			histogram[i][2] = 0;
		}

		for(int i = 0 ; i  < Size; ++i){
			for(int j = 0 ; j  < Size; ++j){
				histogram[Imagedata[i][j][0]][0] = histogram[Imagedata[i][j][0]][0] + 1;
				histogram[Imagedata[i][j][1]][1] = histogram[Imagedata[i][j][1]][1] + 1;
				histogram[Imagedata[i][j][2]][2] = histogram[Imagedata[i][j][2]][2] + 1;
			}
		}
		char filename[256];
		//write histogram
		memset(filename, 0, 256);
		sprintf(filename, "%s_redHist.txt", argv[1]);	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "inHist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][0]);
		}
		fclose(file);

		memset(filename, 0, 256);
		sprintf(filename, "%s_greenHist.txt", argv[1]);
	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "Hist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][1]);
		}
		fclose(file);

		memset(filename, 0, 256);
		sprintf(filename, "%s_blueHist.txt", argv[1]);
	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "Hist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][2]);
		}
		fclose(file);

		unsigned char OutputImagedata[Size][Size][BytesPerPixel];
		for(int i = 0; i < Size; ++i){
			for(int j = 0; j < Size; ++j){
				for(int k = 0; k < BytesPerPixel; ++k){
					OutputImagedata[i][j][k] = 255 - Imagedata[i][j][k];
				}
			}
		}
		if (!(file=fopen(argv[2],"wb")))
		{
			cout << "Cannot open file: " << argv[2] << endl;
			exit(1);
		}
		fwrite(OutputImagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
		fclose(file);
		for(int i = 0 ; i  < 256; ++i){
			histogram[i][0] = 0;
			histogram[i][1] = 0;
			histogram[i][2] = 0;
		}

		for(int i = 0 ; i  < Size; ++i){
			for(int j = 0 ; j  < Size; ++j){
				histogram[OutputImagedata[i][j][0]][0] = histogram[OutputImagedata[i][j][0]][0] + 1;
				histogram[OutputImagedata[i][j][1]][1] = histogram[OutputImagedata[i][j][1]][1] + 1;
				histogram[OutputImagedata[i][j][2]][2] = histogram[OutputImagedata[i][j][2]][2] + 1;
			}
		}
		//write histogram
		memset(filename, 0, 256);
		sprintf(filename, "%s_redHist.txt", argv[2]);	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "inHist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][0]);
		}
		fclose(file);

		memset(filename, 0, 256);
		sprintf(filename, "%s_greenHist.txt", argv[2]);
	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "Hist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][1]);
		}
		fclose(file);

		memset(filename, 0, 256);
		sprintf(filename, "%s_blueHist.txt", argv[2]);
	
		if (!(file=fopen(filename,"w")))
		{
			cout << "Cannot open file: " << "Hist.txt" << endl;
			exit(1);
		}
		for(int i = 0 ; i  < 256; ++i){
			fprintf(file, "%d %ld\n", i, histogram[i][2]);
		}
		fclose(file);
	}
	
	else if(type == 2){
		if(BytesPerPixel == 1){			
			unsigned char Imagedata[Size][Size];	
			if (!(file=fopen(argv[1],"rb")))
			{
				cout << "Cannot open file: " << argv[1] <<endl;
				exit(1);
			}
			fread(Imagedata, sizeof(unsigned char), Size*Size, file);
			fclose(file);	
			unsigned char logo[128][128];
			if (!(file=fopen(argv[5],"rb")))
			{
				cout << "Cannot open file: " << argv[1] <<endl;
				exit(1);
			}
			fread(logo, sizeof(unsigned char), 128*128, file);
			fclose(file);
	
			int diff = Size/2 - 128/2;

			for(int i = 0; i < 128; ++i){
				for(int j = 0; j < 128; ++j){
						if(logo[i][j] < 255){
							Imagedata[i + diff][j + diff] = logo[i][j];
						}
				}
			}
	
			if (!(file=fopen(argv[2],"wb")))
			{
				cout << "Cannot open file: " << argv[2] << endl;
				exit(1);
			}
			fwrite(Imagedata, sizeof(unsigned char), Size*Size, file);
			fclose(file);
		}
		else{			
			unsigned char Imagedata[Size][Size][BytesPerPixel];	
			if (!(file=fopen(argv[1],"rb")))
			{
				cout << "Cannot open file: " << argv[1] <<endl;
				exit(1);
			}
			fread(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
			fclose(file);	
			unsigned char logo[128][128][BytesPerPixel];
			if (!(file=fopen(argv[5],"rb")))
			{
				cout << "Cannot open file: " << argv[1] <<endl;
				exit(1);
			}
			fread(logo, sizeof(unsigned char), 128*128*BytesPerPixel, file);
			fclose(file);
	
			int diff = Size/2 - 128/2;

			for(int i = 0; i < 128; ++i){
				for(int j = 0; j < 128; ++j){
					for(int k = 0; k < BytesPerPixel; ++k){
						if(logo[i][j][k] < 255){
							Imagedata[i + diff][j + diff][k] = logo[i][j][k];
						}
					}
				}
			}
	
			if (!(file=fopen(argv[2],"wb")))
			{
				cout << "Cannot open file: " << argv[2] << endl;
				exit(1);
			}
			fwrite(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
			fclose(file);
		}
	}
	return 0;
}
