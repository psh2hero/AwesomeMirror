#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int startX;
	int startY;
	int endX;
	int endY;
	int wid;
	int hgt;
} ST_AREA;

void convertRgb2Gray(unsigned char *rgb, int wid, int hgt, ST_AREA grid, unsigned char *gray);
void convertRgb2Rb(unsigned char *rgb, int wid, int hgt, ST_AREA grid, unsigned char *rb);
void getBlemish(unsigned char *gray, int wid, int hgt, int *A);
void getFreckle(unsigned char *gray, int wid, int hgt, int *B);
void getDecolorize(unsigned char *rb, int wid, int hgt, int *C);

/************************************************************/
// Function Name : GetSkinInfo
//
// Input 
// - unsigned char *rgb : RGB 24bit Image Data
// - int wid : Width of Image
// - int hgt : Height of Image
// - int gridX : Count of Horizontal Grid
// - int gridY : Count of Vertical Grid
// 
// Output
// - int *A : Array of Blemish(?창횈쩌)
// - int *B : Array of Freckle(짹창쨔횑)
// - int *C : Array of Decolorize(쨩철횁쨋횇쨩쨩철) 
/************************************************************/
void GetSkinInfo(unsigned char *rgb, int wid, int hgt, int gridX, int gridY, int *A, int *B, int *C)
{
	unsigned char *gray;
	unsigned char *rb;
	int x_index, y_index, index;
	ST_AREA grid;
	float fStepX, fStepY;
	
	fStepX = wid / gridX;
	fStepY = hgt / gridY;
	
	index = 0;
	for (x_index=0;x_index<gridX;x_index++)
	{
		for (y_index=0;y_index<gridY;y_index++)
		{
			grid.startX = (x_index*fStepX);
			grid.startY = (y_index*fStepY);
			grid.endX = ((x_index+1)*fStepX);
			grid.endY = ((y_index+1)*fStepY);
			grid.wid = grid.endX-grid.startX;
			grid.hgt = grid.endY-grid.startY;
			
			gray = (unsigned char *)malloc(grid.wid*grid.hgt);
			rb = (unsigned char *)malloc(grid.wid*grid.hgt);
	
			convertRgb2Gray(rgb, wid, hgt, grid, gray);
			convertRgb2Rb(rgb, wid, hgt, grid, rb);
	
			getBlemish(gray, grid.wid, grid.hgt, A+index);
			getFreckle(gray, grid.wid, grid.hgt, B+index);
			getDecolorize(rb, grid.wid, grid.hgt, C+index);
	
			free(gray);
			free(rb);
			
			/* Percent Conversion */
			A[index] = A[index] /10;
			B[index] = B[index] /10;
			C[index] = (C[index]>128)?(C[index]-128)*100/128:(128-C[index])*100/128;

			index ++;
		}
	}
}

void convertRgb2Gray(unsigned char *rgb, int wid, int hgt, ST_AREA grid, unsigned char *gray)
{
	int i, j, gray_index, rgb_index;
	
	gray_index = 0;
	for (i=grid.startY;i<grid.endY;i++)
	{
		rgb_index = i*wid + grid.startX;
		for (j=grid.startX;j<grid.endX;j++)
		{
			gray[gray_index] = (rgb[rgb_index*3] + rgb[rgb_index*3+1] + rgb[rgb_index*3+2])/3;	
			
			gray_index ++;
			rgb_index ++;
		}
	}
}

void convertRgb2Rb(unsigned char *rgb, int wid, int hgt, ST_AREA grid, unsigned char *rb)
{
	int i, j, rb_index, rgb_index;
	
	rb_index = 0;
	for (i=grid.startY;i<grid.endY;i++)
	{
		rgb_index = i*wid + grid.startX;
		for (j=grid.startX;j<grid.endX;j++)
		{
			rb[rb_index] = (rgb[rgb_index*3+2] - rgb[rgb_index*3])/2 + 128;;	
			
			rb_index ++;
			rgb_index ++;
		}
	}
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
				if (len < BLEMISH_LEN /*&& len > BLEMISH_LEN/2*/) 
				{
					memset(blemishX + i*w + start, 255, j-start);	
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

void getBlemish(unsigned char *gray, int wid, int hgt, int *A)
{
	int i, count;
	unsigned char *blemish;
	
	blemish = (unsigned char *)malloc(wid*hgt);
	fnBlemish(gray, blemish, wid, hgt);
	
	count = 0;
	for (i=0;i<hgt*wid;i++)
	{
		if (blemish[i] == 255) count++;		
	}
	*A = count/8;
	
	free(blemish);
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
		median[i*w + MEDIAN_LEN/2-1] = sort[MEDIAN_LEN/2];
		for (j=MEDIAN_LEN;j<w;j++)
		{
			chk = 0;
			for (k=0;k<MEDIAN_LEN-1;k++)
			{
				if ((chk==0) && (gray[i*w + j-MEDIAN_LEN] == sort[k]))
				{
					chk = 1;
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
		}
	}
}

#define FRECKLE_LEVEL 5
#define FRECKLE_LEN   100
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

void getFreckle(unsigned char *gray, int wid, int hgt, int *B)
{
	int i;
	int count;
	
	unsigned char *freckle;
	
	freckle = (unsigned char *)malloc(wid*hgt);
	fnFreckle(gray, freckle, wid, hgt);
	
	for (i=0;i<hgt*wid;i++)
	{
		if (freckle[i] == 255) count++;
	}
	
	*B = (count/8);
	free(freckle);	
}

void getDecolorize(unsigned char *rb, int wid, int hgt, int *C)
{
	int i;
	int count, rbSum;

	rbSum = 0;	
	for (i=0;i<hgt*wid;i++)
	{
		rbSum += rb[i];
	}
	*C = rbSum/(wid*hgt);
}
