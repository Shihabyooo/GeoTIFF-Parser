#include "GeoTIFF_Parser_Globals.h"

bool isBigEndian;
std::ifstream stream;
TIFFDetails tiffDetails;
Array2D * bitMap;


long int BytesToInt32(const char bytes[4])//, bool _isBigEndian)
{
	if (isBigEndian)
		return (((unsigned char)bytes[0] << 24) | ((unsigned char)bytes[1] << 16) | ((unsigned char)bytes[2] << 8) | (unsigned char)bytes[3]);
	else
		return (((unsigned char)bytes[3] << 24) | ((unsigned char)bytes[2] << 16) | ((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
}

int BytesToInt16(const char bytes[2])//, bool _isBigEndian)
{
	if (isBigEndian)
		return (((unsigned char)bytes[0] << 8) | (unsigned char)bytes[1]);
	else
		return (((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
}

short int BytesToInt8(const char bytes[1])
{
	return (unsigned char)bytes[0];
}

long int BytesToIntX(const char bytes[4], int intSize) //int size in bits
{
	long int result;

	switch (intSize)
	{
	case (8):
		result = BytesToInt8(bytes);
		break;
	case (16):
		result = BytesToInt16(bytes);
		break;
	case (32):
		result = BytesToInt32(bytes);
		break;
	default:
		std::cout << "ERROR! Attempting to convert a bytestream to an unsupported size for integers. Supported sizes: 1, 2 and 4 bytes only. Returning zero" << std::endl;
		result = 0;
		break;
	}
	return result;
}