#include <stdio.h>
#include <stdlib.h>



void savePGMFile(unsigned char *blemish, char *filename, int w, int h);
void fnBlemish(unsigned char *gray, unsigned char *blemish, int w, int h);

void main(int argv, char **argc)
{
	int i;
	unsigned char inchar[3];
	unsigned char *gray;
	unsigned char *blemish;
			
	int w, h;
	
	FILE *in = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Error\n");
	}
	
	w = atoi(argc[3]);
	h = atoi(argc[4]);
	
	blemish = (unsigned char *)malloc(w*h);
	gray = (unsigned char *)malloc(w*h);
	
	for (i=0;i<w*h;i++)
	{
		fread(inchar, 1, 3, in);
		gray[i] = (inchar[0] + inchar[1] + inchar[2])/3;
	}
	
	fnBlemish(gray, blemish, w, h);
	savePGMFile(blemish, argc[2], w, h);
	//savePGMFile(gray, argc[2], w, h);
	
	free(blemish);
	free(gray);
	
	fclose(in);
}

#define BLEMISH_LEVEL 3
#define BLEMISH_LEN   15

void fnBlemish(unsigned char *gray, unsigned char *blemish, int w, int h)
{
	unsigned char prev;
	int start = 0, len;
	int i, j, k;
	
	unsigned char *blemishX;
	unsigned char *blemishY;
	
	blemishX = (unsigned char *)malloc(w*h);
	blemishY = (unsigned char *)malloc(w*h);
	memset(blemishX, 0, w*h);
	memset(blemishY, 0, w*h);
	
	for (i=0;i<h;i++)
	{
		for (j=0;j<w;j+=3)
		{
			if (j == 0) 
			{
				prev = gray[i*w];
				start = 0;
				continue;
			}
			
			if (!start && (prev - gray[i*w+j]) < BLEMISH_LEVEL) 
			{
				start = j-3;
			}
			
			if (start && (gray[i*w+j]-prev) > BLEMISH_LEVEL)
			{
				len = j - start;
				if (len < BLEMISH_LEN && len > BLEMISH_LEN/2) 
				{
					memset(blemishX + i*w + start, 255, j-start);	
					printf("Xlen = %d\n\r", len);
				}
				start = 0;				
			}
			prev = gray[i*w+j];
		}
	}
	
	for (i=0;i<w;i++)
	{
		for (j=0;j<h;j+=3)
		{
			if (j == 0) 
			{
				prev = gray[i];
				start = 0;
				continue;
			}
			
			if (!start && (prev - gray[i+j*w]) < BLEMISH_LEVEL) 
			{
				start = j-3;
			}
			
			if (start && (gray[i+j*w]-prev) > BLEMISH_LEVEL)
			{
				len = j - start;
				if (len < BLEMISH_LEN && len > BLEMISH_LEN/2) 
				{
					for (k=start;k<j+1;k++)
					{
						blemishY[i + k*w] = 255;
					}
					printf("Ylen = %d\n\r", len);
				}
				start = 0;				
			}
			prev = gray[i+j*w];
		}
	}
	
	for (i=0;i<h*w;i++)
	{
		blemish[i] = blemishX[i] & blemishY[i];
	}
	
	free(blemishX);
	free(blemishY);
}

void savePGMFile(unsigned char *blemish, char *filename, int w, int h)
{
	FILE *out = NULL;
	int i;
	
	out = fopen(filename, "wt");
	fprintf(out, "P2\n%d %d\n255\n", w, h);
	
	for (i=0;i<w*h;i++)
	{
		fprintf(out, "%3d\n", blemish[i]);
	}
	
	fclose(out);
}

