// The programe reads out the image data from an image file and write into another 
// last updated on 5/2/2007 by May-chen Kuo                                                               


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

/* k_means algorithm  */
/* @ I. Katsavounidis */
/* June 5, 1992       */

/* -------------------------------------------------------------
The usage of the program is as follows.
"feature" is an 2-D array (pointer to pointer) of floating number.
"feature[i][j]" represents the value of feature number "j" at the 
image location "i".  The index "i" is the row-ordered 
(or column-ordered) image location number.  The index "j" is
the feature number.  For example, if an 256*256 image is used,
and there are 3 features, "i" will be from 0 to 65535, and "j"   
will be from 0 to 2.

"num_of_pixels" is an integer, which represents the total number 
of pixels used.  For example, if an 256*256 image is used (and 
each location has its feature vector), then "num_of_pixels" is 
equal to 256*256 (=65536).

"num_of_features" is an integer, which represents the number of
features used.

"means" is an 2-D array of floating number.  "mean[i][j]" represents
the average of the feature vector component "j" that belongs to 
class "i".

"num_of_classes" is an integer, which represents the number of
classes that the image would be segmented into.

"index" is an (output) 1-D array of integer. "index[i]" represents
the class number assigned to each corresponding location number "i".
"i" is the row-ordered (or column-ordered) location number 
of the image.  The class number starts at 0.
For example, if an 256*256 image is used, and there are 2 classes
in the image, then "index[i]" will be either 0 or 1, where
"i" is from 0 to 65535.

"thre" is a floating number, which represents the error threshold 
required to terminate the algorithm.  This should be a very small
number.  For example, 1.0e-5, or 1.0e-6, or sometimes 0.

Documentation is provided by Junavit Chalidabhongse.

---------------------------------------------------------------- */

void k_means(feature,num_of_pixels,num_of_features,means,num_of_classes,index,thre)
float **feature,thre,**means;
int num_of_pixels,num_of_features,num_of_classes,*index;
{
   register int i,j,k;
   float **p1,**p2;
   float *point1,*point2,*point3,*point4;
   int *ioint1;
   float temp,temp_tmp,min,max;
   int maxindi,minind;
   float *oldbook;
   int *nofvectors;
   int iter,mistakes;
   float error,olderror,rel_error;

   nofvectors=(int *) calloc (num_of_classes,sizeof(int));
   if (nofvectors==NULL)
   {
      printf("Not enough memory for nofvectors\n");
      exit(1);
   }
   oldbook=(float *) calloc (num_of_pixels,sizeof(float));
   if (oldbook==NULL)
   {
      printf("Not enough memory for oldbook\n");
      exit(1);
   }

/* Get the first initial mean, by picking the most distant from the origin */

   if(num_of_pixels>0)
   {
      p1=feature;
      maxindi=0;
      point1=(*p1++);
	  max=0.0;

      for(i=0;i<num_of_features;i++)
      {
         temp_tmp=(*point1++);
         max+=temp_tmp*temp_tmp;
      }

      for(i=1;i<num_of_pixels;i++)
      {

         point1=(*p1++);
         temp=0.0;
         for(j=0;j<num_of_features;j++)
         {
            temp_tmp=(*point1++);
            temp+=temp_tmp*temp_tmp;
         }
         if(temp>max)
         {
            max=temp;
            maxindi=i;
         }
      }

/* After you get the first initial mean, assign the correct values to them */

      p2=means;
      point2=point1=(*p2++);
      point3=feature[maxindi];
      for(i=0;i<num_of_features;i++)
      {
         (*point1++)=(*point3++);
      }
      printf("k=%d  max_distance(from the origin)=%f  maxind=%d\n",0,max,maxindi);
   }


/* Classify the whole set of vectors into the first initial mean, using index      */
/* Also, keep track of the minimum distance for each vector (on oldbook)           */

   max=0.0;
   maxindi=0;
   p1=feature;
   point4=oldbook;
   for(i=0;i<num_of_pixels;i++)
   {
      point1=point2;
/* point1=means[0] */
      point3=(*p1++);
      temp=0.0;
      for(j=0;j<num_of_features;j++)
      {
         temp_tmp=(*point1++)-(*point3++);
         temp+=temp_tmp*temp_tmp;
      }
/* temp is the distance from feature[i] to means[0] */
      (*point4++)=temp;
      if(temp>max)
      {
         max=temp;
         maxindi=i;
      }
   }

/* We know that the max(min) is on var. max. Its index is maxindi */

/* p2 points to means[1] */
   for(k=1;k<num_of_classes;k++)
   {
/* p2 points to means[k] */
      point1=feature[maxindi];
      point4=point2=(*p2++);
/* We need two copies of *p2 ... */
      for(i=0;i<num_of_features;i++)
      {
         (*point2++)=(*point1++);
      }
      printf("k=%d  max_distance=%f  maxindi=%d\n",k,max,maxindi);
      max=0.0;
      maxindi=0;
/* p1 points to the next means */
      p1=feature;
      ioint1=index;
      point3=oldbook;
      for(i=0;i<num_of_pixels;i++,ioint1++,point3++)
      {
         temp=0.0;
         point1=point4;
         point2=(*p1++);
         for(j=0;j<num_of_features;j++)
         {
            temp_tmp=(*point1++)-(*point2++);
            temp+=temp_tmp*temp_tmp;
         }
         min=(*point3);
         if(temp<min)
         {
            (*point3)=min=temp;
            (*ioint1)=k;
         }
         if(min>max)
	 {
            max=min;
            maxindi=i;
	 }
      }
   }
   printf("k=%d  max_distance=%f  maxindi=%d\n",k,max,maxindi);

/* initial means have been selected according to maximin principal */
   printf("Initial means have been selected\n");

   iter=0;
   do
   {
      if(iter)
      {
         mistakes=0;
         olderror=error;
         error=0.0;
         p2=feature;
         ioint1=index;
         for(i=0;i<num_of_pixels;i++,ioint1++)
         {
            min=0.0;
            minind=0;
            p1=means;
            point1=(*p1++);
            point3=point2=(*p2++);
            for(j=0;j<num_of_features;j++)
            {
               temp_tmp=(*point1++)-(*point2++);
               min+=temp_tmp*temp_tmp;
            }
            for(k=1;k<num_of_classes;k++)
            {
               temp=0.0;
               point1=(*p1++);
               point2=point3;
               for(j=0;j<num_of_features;j++)
               {
                  temp_tmp=(*point1++)-(*point2++);
                  temp+=temp_tmp*temp_tmp;
               }
               if(temp<min)
               {
                  min=temp;
                  minind=k;
               }
            }
            if((*ioint1)!=minind)
            {
               mistakes++;
               (*ioint1)=minind;
            }
            error+=min;
         }
      }
      else
      {
         error=0.0;
         point1=oldbook;
         for(i=0;i<num_of_pixels;i++)
         {
            error+=(*point1++);
         }
      }
      p1=means;
      ioint1=nofvectors;
      for(i=0;i<num_of_classes;i++)
      {
         (*ioint1++)=0;
         for(j=0,point1=(*p1++);j<num_of_features;j++)
            (*point1++)=0.0;
      }
      p2=feature;
      ioint1=index;
      for(i=0;i<num_of_pixels;i++)
      {
         minind=(*ioint1++);
         nofvectors[minind]++;
         point1=means[minind];
         point2=(*p2++);
         for(k=0;k<num_of_features;k++)
            (*point1++)+=(*point2++);
      }

      p1=means;
      ioint1=nofvectors;
      for(i=0;i<num_of_classes;i++)
      {
         min=(float) (*ioint1++);
         point1=(*p1++);
         for(j=0;j<num_of_features;j++)
         {
            (*point1++)/=min;
         }
      }

      rel_error=(olderror-error)/error;
      if(rel_error<0.0) rel_error=(-rel_error);
      printf("iteration=%d  mistakes=%d  error=%f  rel.error=%f\n",++iter,mistakes,error/(num_of_pixels*num_of_features),rel_error);
   }
   while(((mistakes!=0) && (rel_error>thre)) || (iter==1));

   for(i=0;i<num_of_classes;i++)
   {
      printf("# of vectors in class %d: %d\n",i,nofvectors[i]);
   }

   free(oldbook);
   free(nofvectors);
}



char laws[9][3][3] ={
{
{1,2,1},
{2,4,2},
{1,2,1}
},

{
{1,0,-1},
{2,0,-2},
{1,0,-1}
},

{
{-1,2,-1},
{-2,4,-2},
{-1,2,-1}
},

{
{-1,-2,-1},
{0,0,0},
{1,2,1}
},

{
{1,0,-1},
{0,0,0},
{-1,0,1}
},

{
{-1,2,-1},
{0,0,0},
{1,-2,1}
},

{
{-1,-2,-1},
{2,4,2},
{-1,-2,-1}
},

{
{-1,0,1},
{2,0,-2},
{-1,0,1}
},

{
{1,-2,1},
{-2,4,-2},
{1,-2,1}
}
};

char lawcoeff[9] = {36,12,12,12,4,4,12,4,4};

int main(int argc, char *argv[]){
	FILE *file;
	int i,j,k;
	if(argc < 2){
		printf( "usage: program_name image_dir\n" );
		return 0;
	}
	char filename[256];
	unsigned char in[64][64];	
	
	float **feature;
	feature = (float**)calloc(15,sizeof(float*));
	for( i=0; i<15; i++ ){
		feature[i] = (float*)calloc(9,sizeof(float));		
	}

	float m[64][64][9];
	int im, a, b;
	for(im = 0; im < 15; ++im){
		memset(filename, 0, 256);
		sprintf(filename, "%s/sample%d.raw", argv[1], im+1);		
		if (!(file=fopen(filename,"rb"))){
			printf("Cannot open file: %s\n" , filename);
			exit(1);
		}
		fread(in, sizeof(unsigned char), 64*64, file);
		fclose(file);
				
		for(i=0; i<64; i++){
			for(j=0; j<64; j++){
				for(k=0; k < 9; k++){
					float sum = 0;
					for(a=-1; a<2; a++){
						for(b=-1; b<2; b++){
							if(i+a < 0 || i+a >= 64 || j+b < 0 || j+b >= 64 )
								sum += 255*laws[k][a+1][b+1];
							else
								sum += in[i+a][j+b]*laws[k][a+1][b+1];
						}
					}
					m[i][j][k] = sum/lawcoeff[k];
				}
			}
		}
		//calculate energy
		float energy = 0;
		for(k=0; k < 9; k++){
			energy = 0;
			for(i=0; i<64; i++){
				for(j=0; j<64; j++){		
					energy += m[i][j][k]*m[i][j][k];
				}
			}
			feature[im][k] = energy/(64*64);
		}
	}

	int fea[15];
	for(i = 0 ;i < 15; ++i){
		fea[i] = i;
	}
	
	float **means;
	means = (float**)calloc(5,sizeof(float*));
	for( i=0; i<5; i++ ){
		means[i] = (float*)calloc(9,sizeof(float));		
	}
	srand((unsigned int)time(0));
	for(i = 0 ;i < 5; ++i){
		int id = rand()%(15-i);
		while(fea[id] == -1) id = (id + 1)%15;		
		for(k = 0; k < 9; ++k){
			means[i][k] = feature[fea[id]][k];
		}
		fea[id] = -1;
	}
	for(i = 0 ;i < 15; ++i){
		fea[i] = 0;
	}
	k_means(feature,15,9,means,5,fea,0);

	for(i = 0 ;i < 15; ++i){
		printf("sample%d.raw assigned label %d\n", i + 1, fea[i]);
	}
	
	for( i=0; i<15; i++ ){
		free(feature[i]);
	}
	free(feature);

	for( i=0; i<5; i++ ){
		free(means[i]);
	}
	free(means);
	return 0;
}

