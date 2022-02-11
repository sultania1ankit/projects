/**

  
* ! WARNING
* ? INFO
* // NOT USING CURRENTLY
* TODO PENDING STUFF
* * HIGHLITER


*?-----------------------------------------------GENERAL_PACKET-----------------------------------------------

HEADER(4_BYTES)
PACKET_LENGTH(1_BYTE)
PACKET_TYPE(1_BYTE)
GENERATION_REASON(1_BYTE)
DATA_INFO(5_BYTES)
DATA(variable)
CRC(4_BYTES)

*?-----------------------------------------------HEADER-----------------------------------------------

Header is a fixed word - "COLD"

*?-----------------------------------------------PACKET_LENGTH-----------------------------------------------

Packet length is the length of whole packet minus ( [HEADER,PACKET_LENGTH,CRC] = 9 ).

*?-----------------------------------------------PACKET_TYPE-----------------------------------------------

Packet type is divided into 2 nibbles.
Upper nibble is for upstream packets and lower nibble is for downstream packets.

Stream direction is decided by processing power. (SERVER > PC > DEVICE)
IE: packet from device to pc should use upper nibble.
	packet from Server tp pc should use lower nibble.

There can be 15 packet types in both categories.

UPPER (XXXX-0000)
LOWER (0000-XXXX)

*?-----------------------------------------------GENERATION_REASON-----------------------------------------------

1 byte value to denote why the packet was generated.


*?-----------------------------------------------DATA_INFO-----------------------------------------------

BYTE 1 - total number of data
BYTE 2 - number of 1 byte data
BYTE 3 - number of 2 byte data
BYTE 4 - number of 4 byte data
BYTE 5 - number of variable byte data


*?-----------------------------------------------DATA-----------------------------------------------

*! Data type is big endian.

ID of 1 byte data
DATA1
.
.

ID of 2 byte data
DATA2
DATA2
.
.

ID of 4 byte data
DATA4
DATA4
DATA4
DATA4
.
.

ID of variable byte data
length of variable data (n)
DATAV1
DATAV2
.
.
DATAVn

*?-----------------------------------------------CRC-----------------------------------------------
*TODO .
The CRC should be calculated for the packet length only (GENERATION_REASON,PACKET_TYPE, DATA_INFO, DATA).
If CRC is not calculated, it should be replaced with HEADER.

*?-----------------------------------------------END_BASIC_STRUCTURE-----------------------------------------------

*/