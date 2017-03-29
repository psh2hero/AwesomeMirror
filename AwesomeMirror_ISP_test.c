#include <stdio.h>
#include <stdlib.h>

void GetSkinInfo(unsigned char *rgb, int wid, int hgt, int gridX, int gridY, int *A, int *B, int *C);
void saveInfoFile(int *info, char *filename);

#define GRIDX 10
#define GRIDY 10
void main(int argv, char **argc)
{
	int i;
	int w, h;
	unsigned char *rgb;
	int *A, *B, *C;
	FILE *in = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Usage) AweSomeMirror Src_Bin_File Wid Hgt\n");
		exit(1);
	}
	
	w = atoi(argc[2]);
	h = atoi(argc[3]);
	
	rgb = (unsigned char *)malloc(w*h*3);
	A = (int *)malloc(GRIDX*GRIDY*sizeof(int));
	B = (int *)malloc(GRIDX*GRIDY*sizeof(int));
	C = (int *)malloc(GRIDX*GRIDY*sizeof(int));

	for (i=0;i<w*h;i++)
	{
		fread(rgb+i*3, 1, 3, in);
	}

	GetSkinInfo(rgb, w, h, GRIDX, GRIDY, A, B, C);
	
	saveInfoFile(A, "A.txt");
	saveInfoFile(B, "B.txt");
	saveInfoFile(C, "C.txt");

	free(rgb);
	free(A);
	free(B);
	free(C);
	
	fclose(in);
}

void saveInfoFile(int *info, char *filename)
{
	FILE *out = NULL;
	int i;
	
	out = fopen(filename, "wt");
	for (i=0;i<GRIDX*GRIDY;i++)
	{
		fprintf(out, "%3d\n", info[i]);
	}
	
	fclose(out);
}

void savePGMFile(unsigned char *gray, char *filename, int w, int h)
{
	FILE *out = NULL;
	int i;
	
	out = fopen(filename, "wt");
	fprintf(out, "P2\n%d %d\n255\n", w, h);
	
	for (i=0;i<w*h;i++)
	{
		fprintf(out, "%3d\n", gray[i]);
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
		fprintf(out, "%3d %3d %3d\n", rgb[i*3], rgb[i*3+1], rgb[i*3+2]);
	}
	
	fclose(out);
}

