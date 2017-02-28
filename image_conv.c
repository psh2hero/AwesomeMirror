#include <stdio.h>
#include <stdlib.h>

void main(int argv, char **argc)
{
	int i;
	unsigned char inchar[3];
	int w, h;
	
	FILE *in = NULL, *out = NULL;
	
	in = fopen(argc[1], "rb");
	if (in == NULL)
	{
		printf("Error\n");
	}
	
	w = atoi(argc[3]);
	h = atoi(argc[4]);
	
	out = fopen(argc[2], "wt");
	fprintf(out, "P3\n%d %d\n255\n", w, h);
	
	for (i=0;i<w*h;i++)
	{
		fread(inchar, 1, 3, in);
		fprintf(out, "%3d %3d %3d\n", inchar[0], inchar[1], inchar[2]);
	}
}
