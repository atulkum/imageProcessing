// The programe reads out the image data from an image file and write into another 
// last updated on 5/2/2007 by May-chen Kuo                                                               


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#define PI 3.14159265

void rotatePoint(int xorig, int yorig, double angle, int* xp, int* yp){
	int xt = *xp - xorig;
	int yt = *yp - yorig;			
		
	double xr = xt*cos(angle) - yt*sin(angle);
	double yr = xt*sin(angle) + yt*cos(angle);
						
	*xp = (int)xr + xorig;
	*yp = (int)yr + yorig;
}

int main(int argc, char *argv[]){
	FILE *file;
	int i,j;
	int s, theta, m, t;
	if(argc < 7){
		printf( "usage: program_name input_img output_img m theta s t\n" );
		return 0;
	}
	m = atoi(argv[3]);
	theta = atoi(argv[4]);
	s = atoi(argv[5]);
	t = atoi(argv[6]);
	
	unsigned char in[256][256];		
	
	if (!(file=fopen(argv[1],"rb"))){
		printf("Cannot open file: %s\n" , argv[1]);
		exit(1);
	}
	fread(in, sizeof(unsigned char), 256*256, file);
	fclose(file);
	////////scale first
	s = t*s;
	theta = t*theta;
	m = t*m;
	
	int size = (int)((256 * (100-s))/100.0 + 0.5);	
	if(size % 4 != 0) size += size%4;			
	int n = 256/size;				

	unsigned char scaled[512][512];		
	
    int ii, jj;
	for(i = n-1, ii=0; i < 256; i += n, ii++){
		for(j = n-1, jj =0; j < 256; j += n, jj++){
			scaled[ii + (512 - size)/2 ][jj + (512 - size)/2] = in[i][j]; 
		}					
	}
	unsigned char rotate[512][512];		
	
	double ang = theta*PI/180.0;
	for(i = 0; i < 512; ++i){
		for(j = 0; j < 512; ++j){
			int x=i, y = j;			
			rotatePoint(256, 256, -ang, (int*)&x, (int*)&y);
			if(x >= 0 && x < 512 && y >= 0 && y < 512){								
				rotate[i+m][j+m] = scaled[x][y];			
			}
		}
	}
	if (!(file=fopen(argv[2],"wb")))
	{
		printf( "Cannot open file: %s\n" , argv[2] );
		exit(1);
	}
	fwrite(rotate, sizeof(unsigned char), 512*512, file);
	fclose(file);

	return 0;
}

