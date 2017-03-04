#include <stdio.h>
#include <stdlib.h>

void savePGMFile(unsigned char *blemish, char *filename, int w, int h);
void fnFreckle(unsigned char *gray, unsigned char *freckle, int w, int h);

void main(int argv, char **argc)
{
	int i;
	unsigned char inchar[3];
	unsigned char *gray;
	unsigned char *freckle;
			
	int w, h;
	
	FILE *in = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Error\n");
	}
	
	w = atoi(argc[3]);
	h = atoi(argc[4]);
	
	freckle = (unsigned char *)malloc(w*h);
	gray = (unsigned char *)malloc(w*h);
	
	for (i=0;i<w*h;i++)
	{
		fread(inchar, 1, 3, in);
		gray[i] = ((int)inchar[0] + (int)inchar[1] + (int)inchar[2])/3;
	}
	
	fnFreckle(gray, freckle, w, h);
	savePGMFile(freckle, argc[2], w, h);
	//savePGMFile(gray, argc[2], w, h);
	
	free(freckle);
	free(gray);
	
	fclose(in);
}

#define FRECKLE_LEVEL 5
#define FRECKLE_LEN   100
void fnMedian(unsigned char *gray, unsigned char *median, int w, int h);
void fnFreckle(unsigned char *gray, unsigned char *freckle, int w, int h)
{
	unsigned char prev;
	int start = 0, len;
	int i, j, k;
	
	unsigned char *freckleX;
	unsigned char *freckleY;
	unsigned char *median;
	
	freckleX = (unsigned char *)malloc(w*h);
	freckleY = (unsigned char *)malloc(w*h);
	median = (unsigned char *)malloc(w*h);
	
	memset(freckleX, 0, w*h);
	memset(freckleY, 0, w*h);
	memset(median, 0, w*h);
	
	fnMedian(gray, median, w, h);
	
	for (i=0;i<h;i++)
	{
		for (j=0;j<w;j++)
		{
			if (j == 0) 
			{
				prev = median[i*w];
				start = 0;
				continue;
			}
			
			if (!start && (prev - median[i*w+j]) < FRECKLE_LEVEL) 
			{
				start = j;
			}
			
			if (start && (median[i*w+j]-prev) > FRECKLE_LEVEL)
			{
				len = j - start;
				if (len < FRECKLE_LEN && len > FRECKLE_LEN/2) 
				{
					memset(freckleX + i*w + start, 255, j-start);	
					printf("Xlen = %d\n\r", len);
				}
				start = 0;				
			}
			prev = median[i*w+j];
		}
	}
	
	for (i=0;i<w;i++)
	{
		for (j=0;j<h;j++)
		{
			if (j == 0) 
			{
				prev = median[i];
				start = 0;
				continue;
			}
			
			if (!start && (prev - median[i+j*w]) < FRECKLE_LEVEL) 
			{
				start = j;
			}
			
			if (start && (median[i+j*w]-prev) > FRECKLE_LEVEL)
			{
				len = j - start;
				if (len < FRECKLE_LEN && len > FRECKLE_LEN/2) 
				{
					for (k=start;k<j+1;k++)
					{
						freckleY[i + k*w] = 255;
					}
					printf("Ylen = %d\n\r", len);
				}
				start = 0;				
			}
			prev = median[i+j*w];
		}
	}
	
	for (i=0;i<h*w;i++)
	{
		freckle[i] = freckleX[i] & freckleY[i];
	}
	
	free(freckleX);
	free(freckleY);
	free(median);
}

#define MEDIAN_LEN 50
void fnMedian(unsigned char *gray, unsigned char *median, int w, int h)
{
	int i, j, k, l, index, chk;
	unsigned char sort[MEDIAN_LEN], tmp, tmp1;
	
	for (i=0;i<h;i++)
	{
		for (j=0; j<MEDIAN_LEN; j++)
		{
			sort[j] = gray[i*w + j];
		}
		//for (k=0;k<100;k++) printf("%d ", sort[k]);printf("END\r\n");
		for (k=0;k<MEDIAN_LEN-1;k++)
		{
			for (l=k+1;l<MEDIAN_LEN;l++)
			{
				if (sort[k] > sort[l])
				{
					tmp = sort[l];
					sort[l] = sort[k];
					sort[k] = tmp;
				}
			}
		}
		//for (k=0;k<100;k++) printf("%d ", sort[k]);printf("END(1))\r\n");
		median[i*w + MEDIAN_LEN/2-1] = sort[MEDIAN_LEN/2];
		for (j=MEDIAN_LEN;j<w;j++)
		{
			chk = 0;
			for (k=0;k<MEDIAN_LEN-1;k++)
			{
				if ((chk==0) && (gray[i*w + j-MEDIAN_LEN] == sort[k]))
				{
					chk = 1;
					//printf("CHK(%d))\r\n", sort[k]);
				}
				if (chk)
				{
					sort[k] = sort[k+1];
				}
			}
			
			chk = 0;
			index = MEDIAN_LEN-1;
			for (k=0;k<MEDIAN_LEN-1;k++)
			{
				if ((chk==0) && (gray[i*w + j] >= sort[k]) && (gray[i*w + j] < sort[k+1]) )
				{
					chk = 1;
					index = k;
					//printf("CHK(%d))\r\n", sort[k]);
					tmp = sort[k+1];
					sort[k+1] = sort[k];
				}
				else if (chk)
				{
					tmp1 = sort[k+1];
					sort[k+1] = tmp;
					tmp = tmp1;
				}
			}
			sort[index] = gray[i*w+j];			
			median[i*w+j - MEDIAN_LEN/2] = sort[MEDIAN_LEN/2];
			//for (k=0;k<100;k++) printf("%d ", sort[k]);printf("END(%d))\r\n", j);
		}
	}
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

