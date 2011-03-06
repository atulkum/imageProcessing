// The programe reads out the image data from an image file and write into another 
// last updated on 5/2/2007 by May-chen Kuo                                                               


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

struct block{
	int r1;
	int c1;
	int r2;
	int c2;
};
struct seg{
	int val1;
	int val2;
};

char letters[] = {'1','2','3','4','5','6','7','8','9','0','*','.'};

double cmoment(int p, int q, unsigned char im[256][256], int x1, int y1, int x2, int y2){
	int i,j,xb,yb;
	double m00 = 0.0, m10 = 0.0, m01 = 0.0, upq = 0.0;
	if((p == 1 && q == 0) || (p == 0 && q == 1)){
		return upq;
	}
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			m00 += im[i][j];
		}
	}
	if(p == 0 && q == 0) return m00;
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			m10 += j*im[i][j];
		}
	}
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			m01 += i*im[i][j];
		}
	}
	xb = floor(0.5 + m10/m00);
	yb = floor(0.5 + m01/m00);

	if(p ==0){
		for( i = x1; i <= x2; ++i){
			for( j = y1; j <= y2; ++j){
				upq += pow(j-yb, q)*im[i][j];
			}
		}
		return upq;
	}

	if(q ==0){
		for( i = x1; i <= x2; ++i){
			for( j = y1; j <= y2; ++j){
				upq += pow(i-xb, p)*im[i][j];
			}
		}
		return upq;
	}
		
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			upq += pow(j-yb, q)*pow(i-xb, p)*im[i][j];
		}
	}
	return upq;	
}

double avgArea(unsigned char im[256][256], int x1, int y1, int x2, int y2){
	double area = 0;
	int i, j;
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			if(im[i][j] == 0) area++;
		}
	}
	return area/((x1-x2)*(y1-y2)) ;	
}
double area(unsigned char im[256][256], int x1, int y1, int x2, int y2){
	double area = 0;
		int i, j;
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			if(im[i][j] == 0) area++;
		}
	}
	return area;
}

void centroid(unsigned char im[256][256], int x1, int y1, int x2, int y2, int*x, int*y){
	long i,j;
	int area_value, xcent = 0, ycent = 0;
	area_value = area(im, x1, y1, x2, y2);
	if(area_value == 0){
		*x = -1; *y = -1;
		return;
	}
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			if(im[i][j] == 0) {
				xcent+=j;
				ycent +=i;
			}
		}
	}
	*x = xcent/area_value + 1;
	*y = ycent/area_value + 1;
	return;
}
double circlarity(unsigned char im[256][256], int x1, int y1, int x2, int y2){
	long i, j, rows, points = 0;
	double mean = 0, temp, stdev = 0.0, variance;
	int xcenter, ycenter;
	centroid(im, x1,y1,x2,y2, &xcenter, &ycenter);
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			if(im[i][j] == 0){
				mean += sqrt(
					(i-xcenter)*(i-xcenter) +
					(j-xcenter)*(j-xcenter)) ;
				++points;
			}
		}
	}
	if(points == 0) return -1;
	mean /= (double) points;
	for( i = x1; i <= x2; ++i){
		for( j = y1; j <= y2; ++j){
			if(im[i][j] == 0){
				temp += sqrt(
					(i-xcenter)*(i-xcenter) +
					(j-xcenter)*(j-xcenter)) - mean ;
				stdev += temp*temp;	
			}
		}
	}
	stdev /= (double) points;
	variance = sqrt(stdev);
	return (mean/variance);
}

int main(int argc, char *argv[]){
	FILE *file;

	if(argc < 3){
		cout << "usage: program_name train_image.raw test_image.raw" << endl;
		return 0;
	}
	unsigned char in[256][256];
	if (!(file=fopen(argv[1],"rb"))){
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(in, sizeof(unsigned char), 256*256, file);
	fclose(file);
	
	unsigned long rowhist[256];
	unsigned long colhist[256];

	for(int i = 0 ; i  < 256; ++i){
		rowhist[i] = 0;
		colhist[i] = 0;
	}

	for(int i = 0 ; i  < 256; ++i){
		for(int j = 0 ; j  < 256; ++j){
			if(in[i][j] > 127) in[i][j] = 255;
			else in[i][j] = 0;
		}
	}
	for(int i = 0 ; i  < 256; ++i){
		for(int j = 0 ; j  < 256; ++j){
			if(in[i][j] == 0){
				rowhist[i] += 1;
			}
		}
	}
	for(int j = 0 ; j < 256; ++j){
		for(int i = 0 ; i  < 256; ++i){
			if(in[i][j] == 0){				
				colhist[j] += 1;
			}
		}
	}

	seg rs[5]; int rsl = 0;
	seg cs[10]; int csl = 0;

	int cur = -1;
	for(int i = 0 ; i  < 256; ++i){
		if(rowhist[i] == 0){
			if(cur != -1){
				rs[rsl].val1 = cur;
				rs[rsl].val2 = i-1;
				rsl++;
				cur = -1;
			}
		}
		else{
			if(cur == -1){
				cur = i;
			}			
		}
	}
	cur = -1;
	for(int i = 0 ; i  < 256; ++i){	
		if(colhist[i] == 0){
			if(cur != -1){
				cs[csl].val1 = cur;
				cs[csl].val2 = i-1;
				csl++;
				cur = -1;
			}
		}
		else{
			if(cur == -1){
				cur = i;
			}			
		}
	}
	block chars[30]; int charsl = 0;
	for(int i = 0 ; i  < rsl; ++i){
		for(int j = 0 ; j  < csl; ++j){
			int isValid = 0;
			for(int ii =  rs[i].val1; ii  < rs[i].val2; ++ii){
				for(int jj = cs[i].val1 ; jj  < cs[i].val2; ++jj){
					if(in[ii][jj] == 0) isValid = 1;
				}
			}
			if(isValid == 0) continue;

			chars[charsl].r1 = rs[i].val1;
			chars[charsl].r2 = rs[i].val2;
			chars[charsl].c1 = cs[j].val1;
			chars[charsl].c2 = cs[j].val2;
			

			charsl++;
		}
	}
	/////////////////////////////////////////////
	for(int i = 0 ; i  < charsl; ++i){
		double momnet = cmoment(3,4,in, chars[charsl].r1,chars[charsl].c1, chars[charsl].r2,chars[charsl].c2);
		double avAr = area(in, chars[charsl].r1,chars[charsl].c1, chars[charsl].r2,chars[charsl].c2);
		double circ = circlarity(in, chars[charsl].r1,chars[charsl].c1, chars[charsl].r2,chars[charsl].c2);
		printf("char %c %f %f %f\n", letters[i], momnet, avAr, circ);
	}
	/////////////////write output files//////////
	/*if (!(file=fopen(argv[2],"wb")))
	{
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size*Size, file);
	fclose(file);*/

	return 0;
}

