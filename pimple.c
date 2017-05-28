#include <stdio.h>
#include <stdlib.h>

void savePGMFile(unsigned char *blemish, char *filename, int w, int h);
void savePPMFile(unsigned char *rgb, char *filename, int w, int h);
void fnPimple(unsigned char *grey, unsigned char *blemish, int w, int h);

void rgb2yuv(unsigned char *rgb, unsigned char *y, unsigned char *u, unsigned char *v, int w, int h)
{
	int i;
	unsigned char r, g, b;

	for (i = 0; i < w*h; i++)
    {
    	r = rgb[i*3+0];
    	g = rgb[i*3+1];
    	b = rgb[i*3+2];

    	y[i] = (unsigned char)( (66*r + 129*g + 25*b + 128) >> 8) + 16;
        u[i] = (unsigned char)( (-38*r - 74*g + 112*b + 128) >> 8) + 128;
        v[i] = (unsigned char)( (112*r - 94*g - 18*b + 128) >> 8) + 128;
    }
}

void findMinMax(unsigned char *grey, int w, int h)
{
	int i, min, max;

	min = 0xff;
	max = 0;

	for (i=0;i<w*h;i++)
	{
		if (grey[i] > max) max = grey[i];
		else if (grey[i] < min) min = grey[i];
	}

	printf("Min : %d, Max : %d\n", min, max);
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
/*
		if ((sum - man*10000) > 10000) 
		{
			printf("sum : %d\n", sum);
			man ++;
		}*/
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

void main(int argv, char **argc)
{
	int i;
	
	unsigned char *y, *u, *v, *e, *d;
	unsigned char *rgb;
			
	int w, h;
	
	FILE *in = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Error\n");
	}
	
	w = 80;//atoi(argc[2]);
	h = 80;//atoi(argc[3]);
	
	y = (unsigned char *)malloc(w*h);
	u = (unsigned char *)malloc(w*h);
	v = (unsigned char *)malloc(w*h);
	e = (unsigned char *)calloc(w*h, 1);
	d = (unsigned char *)malloc(w*h);
	rgb = (unsigned char *)malloc(w*h*3);
	
	fread(rgb, w*h*3, 1, in);
	rgb2yuv(rgb, y, u, v, w, h);
	findEdge(y, e, w, h);
	getDiff(v, u, d, w, h);
	
	savePGMFile(y, "y.pgm", w, h);
	savePGMFile(u, "u.pgm", w, h);
	savePGMFile(v, "v.pgm", w, h);
	savePPMFile(rgb, "rgb.ppm", w, h);
	savePGMFile(e, "edge.pgm", w, h);
	savePGMFile(d, "diff.pgm", w, h);
	
//	printf("<u value>\n");findMinMax(u, w, h);
//	printf("<v value>\n");findMinMax(v, w, h);
	printf("<Y edge value>\n");findMinMax(e, w, h);
	printf("Y edge sum : %ld\n", getSum(e, w, h));
	printf("<v, u diff value>\n");findMinMax(d, w, h);
	printf("V-U sum : %ld\n", getSum(d, w, h));

	free(rgb);
	free(y);
	free(u);
	free(v);
	free(e);
	free(d);
	
	fclose(in);
}

void savePGMFile(unsigned char *grey, char *filename, int w, int h)
{
	FILE *out = NULL;
	int i;
	
	out = fopen(filename, "wt");
	fprintf(out, "P2\n%d %d\n255\n", w, h);
	
	for (i=0;i<w*h;i++)
	{
		fprintf(out, "%3d\n", grey[i]);
	}
	
	fclose(out);
}

void savePPMFile(unsigned char *rgb, char *filename, int w, int h)
{
	FILE *out = NULL;
	int i;
	
	out = fopen(filename, "wt");
	fprintf(out, "P3\n%d %d\n255\n", w, h);
	
	for (i=0;i<w*h;i++)
	{
		fprintf(out, "%3d %3d %3d\n", rgb[i*3+0], rgb[i*3+1], rgb[i*3+2]);
	}
	
	fclose(out);
}

