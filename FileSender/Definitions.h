
#ifndef HEADER_DEFINITIONS
#define HEADER_DEFINITIONS

const int PORT = 9899;

const int PACKET_SIZE = 1024;

const int MODE_READY = 1;
const int MODE_COMPLETE = 2;

typedef unsigned char byte;
typedef unsigned short seqnum;

typedef struct PacketData
{

	seqnum id;
	byte eof;
	unsigned short length;
	unsigned short checksum;
	byte payload[PACKET_SIZE];

} PacketData;

typedef struct PacketACK
{

	seqnum id;
	byte corrupt;

} PacketACK;

typedef struct PacketHandshake
{
	byte mode;
} PacketHandshake;

#endif