
#include "Definitions.h"

const unsigned short PACKET_SEND = 0;
const unsigned short PACKET_SENT = 1;
const unsigned short PACKET_RECVD = 2;
const unsigned short PACKET_ACKD = 3;

typedef union GenericPacket
{
	PacketData *packetData;
	PacketACK *packetACK;
	void* packet;
} GenericPacket;

typedef struct PacketListItem
{
	GenericPacket packet;
	unsigned short status;
	time_t timestamp;
	PacketListItem* next;
} PacketListItem;

int insert(GenericPacket* packet, unsigned short status);
int remove(seqnum Id);
PacketListItem* get(seqnum Id);
int update(seqnum Id, unsigned short status);
int length();
int countByStatus(unsigned short status);
int getNextByStatus(unsigned short status);
void empty();

void rewind();
int hasNext();
PacketListItem* next();
