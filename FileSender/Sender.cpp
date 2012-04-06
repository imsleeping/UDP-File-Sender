
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Sender.h"

#include "Definitions.h"
#include "FileInput.h"
#include "SendNetwork.h"
#include "PacketList.h"

int main(int argsc, char* argsv[])
{

	int i;

	// check args
	if (argsc < 3)
	{
		
		printf("Usage: %s <file> <receiver ip>", argsv[0]);
		return 0;

	}
	
	// open file
	if (!loadFile(argsv[1]))
	{
		
		printf("Unable to load file");
		return 0;

	}

	printf("Attempting to connect\r\n");

	i = 0;
	while (!startConnection(argsv[2], PORT))
	{
		Sleep(2500);
		i++;
		printf("\rRetry: %i", i);
	}

	printf("\r\nConnected\r\n");

	send();

	handleCloseHandshake();

	// clean up
	closeConnection();
	closeFile();
	
}

void send()
{
	
	if (!connectionOpen())
		return;

	//const int packetCount = countPackets();
	int base = 0;
	int i;
	PacketListItem *item = NULL;
	PacketData *packet = NULL;
	PacketACK ack;
	int complete = 0;

	while (!complete)
	{

		// check for any timeouts
		rewind();
		while (hasNext())
		{
			
			item = next();

			// if it isn't in the air
			if (item->status != PACKET_SENT)
				continue;

			if (item->timestamp + TIMEOUT < getTime())
			{
				item->status = PACKET_SEND;
			}

			if (item->packet.packetData->id == 1)
				printf("1");

		}

		// fill window
		i = WINDOW_SIZE - (countByStatus(PACKET_SENT) + countByStatus(PACKET_SEND));
		while (i > 0)
		{
			packet = (PacketData*)malloc(sizeof(PacketData));

			getPacket(packet);

			insert((GenericPacket*)packet,PACKET_SEND);

			i--;
		}

		// send any in queue
		while ((i = getNextByStatus(PACKET_SEND)) != -1)
		{

			item = get(i);

			printf("SEND %i\r\n", i);

			sendPacket(get(i)->packet.packetData);
			item->status = PACKET_SENT;
			item->timestamp = getTime();
			
		}

		// receive any packets waiting
		while (recvPacket(&ack))
		{

			

			item = get(ack.id);

			// received out of sequence ID, ignore it
			if (item == NULL)
			{
				continue;
			}

			// if corrupt resend
			if (ack.corrupt)
			{
				printf("ACKD %i	[CORRUPT]\r\n", ack.id);
				item->status = PACKET_SEND;
			}
			else
			{
				printf("ACKD %i\r\n", ack.id);
				item->status = PACKET_ACKD;
			}

		}

		// fast forward base
		while ((item = get(base)) != NULL && item->status == PACKET_ACKD)
		{
			
			if (item->packet.packetData->eof)
				complete = 1;

			remove(base);
			increment(base);

		}

		// update status
		//printf("\r%.2f%%", (float)countByStatus(PACKET_ACKD)/(float)packetCount);

	}

	// free all queue items
	empty();

}

time_t getTime()
{
	time_t time = GetTickCount();
	return time;
}
