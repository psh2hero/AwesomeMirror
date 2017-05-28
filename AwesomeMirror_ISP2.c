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

void rgb2yuv(unsigned char *rgb, int w, int h, ST_AREA grid, unsigned char *y, unsigned char *u, unsigned char *v);
int  findMax(unsigned char *grey, int w, int h);
void findEdge(unsigned char *y, unsigned char *edge, int w, int h);
long getSum(unsigned char *grey, int w, int h);
void getDiff(unsigned char *y1, unsigned char *y2, unsigned char *diff, int w, int h);
void doGetSkinInfo(unsigned char *y, unsigned char *u, unsigned char *v, 
					int wid, int hgt, int *A, int *B, int *C);

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
	unsigned char *y, *u, *v;
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
			
			y = (unsigned char *)malloc(grid.wid*grid.hgt);
			u = (unsigned char *)malloc(grid.wid*grid.hgt);
			v = (unsigned char *)malloc(grid.wid*grid.hgt);

			rgb2yuv(rgb, wid, hgt, grid, y, u, v);
			doGetSkinInfo(y, u, v, grid.wid, grid.hgt, A+index, B+index, C+index);

			free(y);
			free(u);
			free(v);
			
			index ++;
		}
	}
}

void doGetSkinInfo(unsigned char *y, unsigned char *u, unsigned char *v, 
					int wid, int hgt, int *A, int *B, int *C)
{
	int i, vuDiffSum, vuDiffMax, yEdgeMax, allSum;
	unsigned char *e, *d;

	e = (unsigned char *)calloc(wid*hgt, 1);
	d = (unsigned char *)malloc(wid*hgt);

	findEdge(y, e, wid, hgt);
	getDiff(v, u, d, wid, hgt);

	vuDiffSum = getSum(d, wid, hgt);
	vuDiffMax = findMax(d, wid, hgt);
	yEdgeMax = findMax(e, wid, hgt);
	allSum = vuDiffMax + vuDiffSum + yEdgeMax;

	if (allSum < 50)   // Clean
	{
		*A = 0;
		*B = 0;
		*C = 0;
	}
	else if (allSum < 100)  // Pores
	{
		*A = allSum - 50;
		*B = 0;
		*C = 0;
	}
	else if (yEdgeMax > 40) // freckles
	{
		*A = 0;
		*B = allSum > 200 ? 100 : allSum - 100;
		*C = 0;
	}
	else					// pimples
	{
		*A = 0;
		*B = 0;
		*C = allSum > 200 ? 100 : allSum - 100;
	}

	free(e);
	free(d);
}

void rgb2yuv(unsigned char *rgb, int wid, int hgt, ST_AREA grid, 
			unsigned char *y, unsigned char *u, unsigned char *v)
{
    int i, j, yuv_index, rgb_index;
	unsigned char r, g, b;	

	yuv_index = 0;
	for (i=grid.startY;i<grid.endY;i++)
	{
		rgb_index = i*wid + grid.startX;
		for (j=grid.startX;j<grid.endX;j++)
		{
			r = rgb[rgb_index*3+0];
	    	g = rgb[rgb_index*3+1];
	    	b = rgb[rgb_index*3+2];

	    	y[yuv_index] = (unsigned char)( (66*r + 129*g + 25*b + 128) >> 8) + 16;
	        u[yuv_index] = (unsigned char)( (-38*r - 74*g + 112*b + 128) >> 8) + 128;
	        v[yuv_index] = (unsigned char)( (112*r - 94*g - 18*b + 128) >> 8) + 128;

			yuv_index ++;
			rgb_index ++;
		}
	}
}

int findMax(unsigned char *grey, int w, int h)
{
	int i, min, max;

	min = 0xff;
	max = 0;

	for (i=0;i<w*h;i++)
	{
		if (grey[i] > max) max = grey[i];
		else if (grey[i] < min) min = grey[i];
	}

	//printf("Min : %d, Max : %d\n", min, max);
	return max;
}

void findEdge(unsigned char *y, unsigned char *edge, int w, int h)
{
	int i, j, diff;

	for (i=0;i<h-1;i++)
	{
		for (j=0; j<w-1; j++)
		{
			diff = ((y[i*w+j] - y[i*w+1]) * (y[i*w+j] - y[i*w+1]))
					+ ((y[i*w+j] - y[(i+1)*w]) * (y[i*w+j] - y[(i+1)*w]))
					+ ((y[i*w+j] - y[(i+1)*w+1]) * (y[i*w+j] - y[(i+1)*w+1]));

			edge[i*w+j] = diff > (255*255) ? 255 : diff < 0 ? 0 : diff/255;
		}
	}
}

long getSum(unsigned char *grey, int w, int h)
{
	int i, man = 0;
	long sum = 0;

	for (i=0;i<w*h;i++)
	{
		sum += grey[i];
	}

	return sum/w/h;
}

void getDiff(unsigned char *y1, unsigned char *y2, unsigned char *diff, int w, int h)
{
	int i, d;

	for (i=0;i<w*h;i++)
	{
		d = y1[i] - y2[i];
		diff[i] = d<0 ? 0 : d;
	}
}
