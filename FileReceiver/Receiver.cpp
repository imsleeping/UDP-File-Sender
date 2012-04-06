
#include <stdio.h>

#include "Receiver.h"

#include "Definitions.h"
#include "FileOutput.h"
#include "RecvNetwork.h"
#include "PacketList.h"

int main(int argsc, char* argsv[])
{

	if (argsc < 2)
	{
		
		printf("Usage: %s <file>", argsv[0]);
		return 0;

	}
	
	// open file
	if (!createFile(argsv[1]))
	{
		
		printf("Unable to create file\r\n");
		return 0;

	}

	printf("Waiting for connection\r\n");

	if (!startConnection(PORT))
	{
		printf("Unable to start connection\r\n");
		return 0;
	}

	
	printf("\r\nConnected\r\n");

	recv();

	handleCloseHandshake();

	closeConnection();
	closeFile();


}

void recv()
{

	if (!connectionOpen())
		return;

	int completed = 0,
		i,
		j;
	seqnum base = 0, 
		lastWrite = 0;
	PacketData packet;
	PacketACK ack;
	PacketListItem *item;
	byte *buffer;

	while (!completed)
	{

		// get packet
		while (!recvPacket(&packet));

		printf("RECV %i\r\n", packet.id);

		// check it's within our window
		i = packet.id;
		if (!((i >= (base-WINDOW_SIZE) % USHRT_MAX && i < base + WINDOW_SIZE) || (i < WINDOW_SIZE && i < (base + WINDOW_SIZE) % USHRT_MAX)))
		{
			continue;
		}

		// send ACK
		memset(&ack,0,sizeof(PacketACK));
		ack.id = i;
		
		item = NULL;
		item = get(i);

		printf("ACK  %i\r\n", ack.id);

		if (isCorrupt(&packet))
		{
			ack.corrupt = 1;
			sendPacket(&ack);
			continue;
		}

		sendPacket(&ack);

		// store packet
		if (item == NULL)
		{
			insert((GenericPacket*)&packet,PACKET_RECVD);
		}

		if (ack.id == 0)
		{
			printf("test");
		}

		// fast forward base
		while (get(base) != NULL)
		{
			increment(base);
		}

		// handle writing to file
		i = lastWrite;
		while (get(i) != NULL)
		{
			 i = increment(i);
		}

		if (i > BUFFER_COUNT)
		{

			buffer = (byte*) calloc(i,sizeof(PACKET_SIZE));

			j = 0;
			while (lastWrite != i)
			{

				item = get(lastWrite);

				printf("WRTE %i\r\n" + item->packet.packetData->id);

				memcpy(&buffer[j],item->packet.packetData->payload,item->packet.packetData->length);
				j += item->packet.packetData->length;

				if (item->packet.packetData->eof)
				{
					completed = true;
				}

				remove(lastWrite);

				lastWrite = increment(lastWrite);

			}

			store(buffer,j);

			free(buffer);

		}

	}

}
