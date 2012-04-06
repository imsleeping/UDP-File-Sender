
#include <stdio.h>

#include "FileOutput.h"
#include "Definitions.h"


FILE *file = NULL;
seqnum base = 0;

int createFile(char* filename)
{
	
	if (file != NULL)
		return 0;
	
	file = fopen(filename,"wb+");

	return 1;

}

int store(byte *data, int length)
{

	fwrite(data,length,1,file);

	return 1;

}

void closeFile()
{
	
	if (file != NULL)
		fclose(file);

}
