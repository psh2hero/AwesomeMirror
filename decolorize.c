#include <stdio.h>
#include <stdlib.h>



void savePGMFile(unsigned char *blemish, char *filename, int w, int h);
void fnDecoloize(unsigned char *gray, unsigned char *decoloize, int w, int h);

void main(int argv, char **argc)
{
	int i;
	unsigned char inchar[3];
	char *rb;
	unsigned char *decoloize;
			
	int w, h;
	
	FILE *in = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Error\n");
	}
	
	w = atoi(argc[3]);
	h = atoi(argc[4]);
	
	decoloize = (unsigned char *)malloc(w*h);
	rb = (char *)malloc(w*h);
	
	for (i=0;i<w*h;i++)
	{
		fread(inchar, 1, 3, in);
		rb[i] = (inchar[0] - inchar[2])/3;
		printf("rb:%d\r\n", rb[i]);
	}
	
	savePGMFile(decoloize, argc[2], w, h);
	
	free(decoloize);
	free(rb);
	
	fclose(in);
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

