
#include <stdlib.h>

#include "Definitions.h"

#include "PacketList.h"

PacketListItem* head = NULL,
	*iterator = NULL;

int insert(GenericPacket* packet, unsigned short status)
{

	PacketListItem* item = (PacketListItem*)malloc(sizeof(PacketListItem));

	if (item == NULL)
		return 0;

	item->packet.packet = packet;
	item->status = status;
	item->next = head;

	head = item;

	return 1;

}

int remove(seqnum Id)
{
	
	// find item
	PacketListItem *next = head, *prev = NULL;
	while (next->packet.packetData->id != Id && next != NULL)
	{
		prev = next;
		next = next->next;
	}

	if (next == NULL)
	{
		return 0;
	}

	if (prev == NULL)
	{
		head = next->next;
	}
	else
	{
		prev->next = next->next;
	}
	
	free(next->packet.packetData);
	free(next);
	return 1;

}

PacketListItem* get(seqnum Id)
{
	
	PacketListItem *next = head;

	if (next == NULL)
		return NULL;

	while (next != NULL && next->packet.packetData->id != Id)
	{
		next = next->next;
	}

	if (next == NULL)
	{
		return NULL;
	}

	return next;

}

int update(seqnum Id, unsigned short status)
{
	
	PacketListItem *item = get(Id);

	if (item == NULL)
	{
		return 0;
	}

	item->status = status;
	return 1;

}

int length()
{

	int i = 0;
	PacketListItem *next = head;
	while (next != NULL)
	{
		i++;
		next = next->next;
	}

	return i;

}

int countByStatus(unsigned short status)
{

	int i = 0;
	PacketListItem *next = head;
	while (next != NULL)
	{
		if (next->status == status) i++;

		next = next->next;
	}

	return i;

}

int getNextByStatus(unsigned short status)
{

	PacketListItem *next = head;
	while (next != NULL)
	{
		if (next->status == status) 
			return next->packet.packetData->id;

		next = next->next;
	}

	return -1;

}

void empty()
{

	PacketListItem *item,
		*_iterator;

	_iterator = head;
	
	if (_iterator == NULL)
		return;

	do
	{
		
		item = _iterator;
		_iterator = _iterator->next;

		free(item->packet.packetData);
		free(item);

	}
	while (_iterator != NULL);

}

void rewind()
{
	iterator = head;
}

int hasNext()
{
	
	if (iterator == NULL)
		return 0;

	if (iterator->next == NULL)
		return 0;

	return 1;

}

PacketListItem* next()
{

	if (!hasNext())
		return NULL;

	PacketListItem *item = iterator;

	iterator = iterator->next;

	return item;

}
