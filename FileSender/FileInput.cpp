
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Definitions.h"
#include "FileInput.h"
#include "SendNetwork.h"

FILE* file = NULL;
PacketData cache[CACHE_SIZE];
seqnum pnum = 0, base = 0;

// load the file
int loadFile(char* filename)
{
	
	// empty cache
	memset(cache,0,sizeof(PacketData) * CACHE_SIZE);

	file = fopen(filename,"rb");
	fseek(file,0,SEEK_END);
	printf("%s is %i bytes\r\n", filename, ftell(file));
	fseek(file,0,SEEK_SET);

	if (file == NULL || ferror(file))
	{
		return 0;
	}
	
	return 1;

}

// get last error number
int getFileError()
{

	if (file == NULL)
	{
		return -1;
	}

	return ferror(file);

}

// fill packet with data
int getPacket(PacketData* packet)
{

	if (file == NULL)
	{
		return 0;
	}

	int i;

	// if pnum is the next one after the cache
	// reload cache
	if (pnum == (base + CACHE_SIZE) % USHRT_MAX || pnum == 0)
	{

		if (pnum != 0)
			base = (base + CACHE_SIZE) % USHRT_MAX;

		for (i = 0; i < CACHE_SIZE; i++)
		{

			cache[i].length = fread(&cache[i].payload, 1, PACKET_SIZE, file);
			cache[i].id = (base + i) % USHRT_MAX;

			if (ferror(file))
			{
				printf("Error reading file");
				return 0;
			}

			if (cache[i].length != PACKET_SIZE)
			{
				if (feof(file))
				{
					cache[i].eof = 1;
					break; // last packet
				}
				else
				{
					return 0;
				}
			}

		}

	}

	memset(packet,0,sizeof(packet));
	
	if (pnum > base && pnum < base + CACHE_SIZE)
	{
		i = pnum - base;
	}
	else if (pnum < CACHE_SIZE)
	{
		i = pnum;
	}
	else
	{
		return 0;
	}

	memcpy(packet,&cache[i],sizeof(PacketData));

	pnum = increment(pnum);

	return 1;

}

int countPackets()
{

	if (file == NULL)
		return -1;

	// get file length
	fseek(file, 0, SEEK_END);
	return (int)ceil((double)ftell(file) / (double)PACKET_SIZE);

}

// clean up
void closeFile()
{
	if (file != NULL)
		fclose(file);
}