#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

//type : 1-s,2-t, 3-k
	
int scond[58] = {272,80,20,17,144,48,24,18,52,22,19,25,88,208,400,304,308,464,23,89,51,306,30,180,54,27,216,432,307,94,55,310,31,91,217,472,496,436,311,95,473,500,438,63,219,504,439,502,127,319,223,475,505,508,503,383,479,509};
int tcond[46] = {50,26,152,176,308,464,23,89,51,306,30,180,54,27,216,432,307,94,55,310,31,91,217,472,496,436,311,95,473,500,438,63,219,504,439,502,127,319,223,475,505,508,503,383,479,509};
int kcond[40] = {50,26,152,176,308,464,23,89,55,310,31,91,217,472,496,436,311,95,473,500,438,63,219,504,439,502,127,319,223,475,505,508,503,383,479,509,447,255,507,510};

int sUncond[8] = {20,17,144,48,30,51,306,180};
int tUncond[16] = {20,17,144,48,30,51,306,180,52,22,19,25,88,208,400,304};
int kUncond[12] = {272,80,20,17,144,48,24,18,152,176,26,50};

int stUn[21][3][3] = {
	{{0,2,1},{0,1,2},{1,0,0}},
	{{1,2,0},{2,1,0},{0,0,1}},
	{{0,0,1},{2,1,0},{1,2,0}},
	{{1,0,0},{0,1,2},{0,2,1}},
    {{1,1,-1},{1,1,-1},{-1,-1,-1}},
    {{-1,1,0},{1,1,1},{-1,0,0}},
	{{0,1,-1},{1,1,1},{0,0,-1}},
	{{0,0,-1},{1,1,1},{0,1,-1}},
    {{-1,0,0},{1,1,1},{-1,1,0}},
	{{-1,1,0},{1,1,0},{0,1,0}},
    {{0,1,0},{1,1,0},{-1,1,-1}},
	{{0,1,0},{0,1,1},{-1,1,-1}},
	{{-1,1,0},{0,1,1},{0,1,0}},
	{{1,-1,1},{-1,1,-1},{2,2,2}},
    {{1,-1,2},{-1,1,2},{1,-1,2}},
    {{2,2,2},{-1,1,-1},{1,-1,1}},
	{{2,-1,1},{2,1,-1},{2, -1, 1}},
	{{-1,1,0},{0,1,1},{1,0,-1}},
	{{0,1,-1},{1,1,0},{-1,0,1}},
	{{-1,0,1},{1,1,0},{0,1,-1}},
	{{1,0,-1},{0,1,1},{-1,1,0}}
};

int kUn[14][3][3] = {
	{{1,1,-1},{1,1,-1},{-1,-1,-1}},
	{{-1,-1,-1},{-1,1,1},{-1,1,1}},
	{{-1,1,-1},{1,1,1},{-1,-1,-1}},
	{{-1,1,-1},{1,1,-1},{-1,1,-1}},
	{{-1,-1,-1},{1,1,1},{-1,1,-1}},
	{{-1,1,-1},{-1,1,1},{-1,1,-1}},
	{{1,-1,1},{-1,1,-1},{2,2,2}},
	{{1,-1,2},{-1,1,2},{1,-1,2}},
	{{2,2,2},{-1,1,-1},{1,-1,1}},
	{{2,-1,1},{2,1,-1},{2,-1,1}},
	{{-1,1,0},{0,1,1},{1,0,-1}},
	{{0,1,-1},{1,1,0},{-1,0,1}},
	{{-1,0,1},{1,1,0},{0,1,-1}},
	{{1,0,-1},{0,1,1},{-1,1,0}},
};

int find(int v, int t){
	if(t == 1){		
		for(int i = 0 ; i < 58; i++){
			if(scond[i] == v) return 1;
		}
	}
	else if(t == 2){
		for(int i = 0 ; i < 46; i++){
			if(tcond[i] == v) return 1;
		}
	}
	else if(t == 3){
		for(int i = 0 ; i < 40; i++){
			if(kcond[i] == v) return 1;
		}
	}
	return 0;
}

int findUnconditional(int v, int t){
	if(t == 1){
		for(int i = 0 ; i < 8; i++){
			if(sUncond[i] == v) return 1;
		}
		
	}
	else if(t == 2){
		for(int i = 0 ; i < 16; i++){
			if(tUncond[i] == v) return 1;
		}
		
	}
	else if(t == 3){
		for(int i = 0 ; i < 12; i++){
			if(kUncond[i] == v) return 1;
		}		
	}
	return 0;
}
int main(int argc, char *argv[]){
	FILE *file;
	int type = 0;
	if(argc < 4){
		cout << "usage: program_name input_image.raw output_image.raw type" << endl;
		return 0;
	}
	type = atoi(argv[3]);	
	
	unsigned char inImg[256][256];	
	if (!(file=fopen(argv[1],"rb"))){
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(inImg, sizeof(unsigned char), 256*256, file);
	fclose(file);	
	
	unsigned char G[256][256];
	unsigned char G1[256][256];
	unsigned char G2[256][256];
	for(int i = 0 ; i  < 256; ++i){
		for(int j = 0 ; j  < 256; ++j){
			if(inImg[i][j] == 255) G[i][j] = 0;
			else G[i][j] = 1;
		}
	}
	int same = 0;	
	while(!same){	
		//put boundary etc	
		for(int i = 1 ; i < 255; ++i){
			for(int j = 1 ; j  < 255; ++j){				
				long val = 0;
				int ex = 1;			
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						if(G[i+a][j+b] == 1){
							val += ex;
						}
						ex *= 2;
					}
				}				
				if(find(val, type) == 1) G1[i][j] = 1;
				else G1[i][j] = 0;
			}
		}
		for(int i = 1 ; i  < 255; ++i){
			for(int j = 1 ; j  < 255; ++j){
				if(G1[i][j] == 0) {
					G2[i][j] = G[i][j];
					continue;
				}			
				long val = 0;
				int ex = 1;
			
				for(int a=-1; a<2; a++){
					for(int b=-1; b<2; b++){
						if(G[i+a][j+b] == 1){
							val += ex;
						}
						ex *= 2;
					}
				}					
				int found = 0;
				if(findUnconditional(val, type) == 1){//hit
					found = 1;	
				}
				else{
					if(type == 1 || type == 2){
						for(int itr = 0; itr < 21; ++itr){
							int abc = 2;
							int isTrue = 1;
							for(int a=-1; a<2; a++){
								for(int b=-1; b<2; b++){
									if(stUn[itr][1+a][1+b] == -1) continue;								
									else if(stUn[itr][1+a][1+b] == 2){
										if(G1[i+a][j+b] != 1 && abc != 1) abc = 0;
										else abc = 1;
									}
									else if(G1[i+a][j+b] != stUn[itr][1+a][1+b]){
										isTrue = 0;
										break;
									}
								}
								if(isTrue == 0) break; 
							}							
							if(isTrue == 1 && abc != 0){
								found = 1;
								break; 
							}
						}
					}
					else if(type == 3){
						for(int itr = 0; itr < 14; ++itr){
							int abc = 2;
							int isTrue = 1;
							for(int a=-1; a<2; a++){
								for(int b=-1; b<2; b++){
									if(kUn[itr][1+a][1+b] == -1) continue;								
									else if(kUn[itr][1+a][1+b] == 2){
										if(G1[i+a][j+b] != 1 && abc != 1) abc = 0;
										else abc = 1;
									}
									else if(G1[i+a][j+b] != kUn[itr][1+a][1+b]){
										isTrue = 0;
										break;
									}
								}
								if(isTrue == 0) break; 
							}						
							if(isTrue == 1 && abc != 0){
								found = 1;
								break; 
							}			
						}
					}
				}			
				if(found == 1){
					G2[i][j] = G[i][j];
				}
				else{
					G2[i][j] = 0;
				}
			}
		}
		int isSame = 1;
		for(int i = 1; i  < 255; ++i){
			for(int j = 1; j  < 255; ++j){
				if(G2[i][j] != G[i][j]) {
					isSame = 0;
					break;
				}
			}
			if(!isSame) break;
		}
		for(int i = 1; i  < 255; ++i){
			for(int j = 1; j  < 255; ++j){
				G[i][j] = G2[i][j];
			}
		}
		same = isSame;
		
/////////////////////////////////////////	
	/*	unsigned char outImg[256][256];	
		for(int i = 0 ; i  < 256; ++i){
			for(int j = 0 ; j  < 256; ++j){
				if(G[i][j] == 0) outImg[i][j] = 255;
				else outImg[i][j] = 0;
			}
		}
		if (!(file=fopen(argv[2],"wb"))){
			cout << "Cannot open file: " << argv[2] << endl;
			exit(1);
		}
		fwrite(outImg, sizeof(unsigned char), 256*256, file);
		fclose(file);*/
/////////////////////////////////////////////
	}

	unsigned char outImg[256][256];	
	for(int i = 0 ; i  < 256; ++i){
		for(int j = 0 ; j  < 256; ++j){
			if(G[i][j] == 0) outImg[i][j] = 255;
			else outImg[i][j] = 0;
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
