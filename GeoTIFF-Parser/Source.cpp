#include <iostream>
#include <fstream>
//#include <functional>
//#include <tchar.h>
//#include <math.h>

#include "Array2D.h"

//Structs
enum BitmapFormat
{
	strips, tiles, undefined
};

struct TIFFDetails
{
public:
	int width;
	int height;

	int samplesPerPixel;
	int bitsPerSample;
	int extraSampleType;
	int sampleFormat;
	int compression;

	int photometricInterpretation;
	int planarConfiguration;

	int noOfPixelsPerTileStrip;

	BitmapFormat bitmapFormat = BitmapFormat::undefined;

	std::unique_ptr<long int> tileStripOffset; //the offsets for the the tiles or strips.
	long int noOfTilesOrStrips;
	long int tileStripByteCount;
	long int rowsPerStrip;
	long int tileHeight;
	long int tileWidth;
};

struct Tag
{
	short int tagID;
	short int fieldTypeID;
	long int count;
	long int offsetValue;
};

struct Type
{
public:
	std::string description;
	int size; //in bytes

	bool isASCII;
	bool isFloat;
	bool isSigned;
	bool isRational;
	bool isUndefined;
	bool isUndetermined; //in case the program failed to determine the type, this is a program-breaking state.
};

//variables
bool viewTagsInCLI = true;
bool isBigEndian;
std::ifstream stream;
TIFFDetails tiffDetails;
//std::unique_ptr<Array2D> bitMap;	//image bitmap will be stored as an array of Array2D, each Array2D will hold the pixel colours of a single strip/tile.
									//each Array2D will be of size (image width, no of samples per pixel). This allows for the access format bitMap.get()[pixel row][pixel column][pixel channel]

Array2D * bitMap;

//functions
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

std::string GetFieldDescription(short int tagID)
{
	std::string desc;

	switch (tagID)
	{
		//Basline required tags:
			//Bi-level and Gray-scale Classes' required tags:
	case (254):
		desc = "NewSubFileType";
		break;
	case (256):
		desc = "ImageWidth";
		break;
	case (257):
		desc = "ImageLength";
		break;
	case (258):
		desc = "BitsPerSample";
		break;
	case (259):
		desc = "Compression";
		break;
	case (262):
		desc = "PhotometricInterpretation";
		break;
	case (273):
		desc = "StripOffsets";
		break;
	case (277):
		desc = "SamplesPerPixel";
		break;
	case (278):
		desc = "RowsPerStrip";
		break;
	case (279):
		desc = "StripByteCounts";
		break;
	case (282):
		desc = "XResolution";
		break;
	case (283):
		desc = "YResolution";
		break;
	case (296):
		desc = "ResolutionUnit";
		break;
		//Palette-color Class required tags [In addition to all the first thirteen tags]:
	case (320):
		desc = "ColorMap";
		break;
		//RGB Class required tags [In addition to all the first thirteen tags]:
	case (284):
		desc = "PlanarConfiguration";
		break;
		//YCbCr Class required tags [In addition to all the first thirteen tags]:
	case (529):
		desc = "YCbCrCoeffients";
		break;
	case (530):
		desc = "YCbCrSubSampling";
		break;
	case (531):
		desc = "YCbCrPositioning";
		break;
	case (532):
		desc = "ReferenceBlackWhite";
		break;
		//Class F required tags [In addition to all the first thirteen tags]:
	case (326):
		desc = "BadFaxLines";
		break;
	case (327):
		desc = "CleanFaxLines";
		break;
	case (328):
		desc = "ConsecutiveBadFaxLines";
		break;

		//Additional Tags:
	case (315):
		desc = "Artist";
		break;
	case (265):
		desc = "CellLength";
		break;
	case (264):
		desc = "CellWidth";
		break;
	case (1):
		desc = "Uncompressed";
		break;
	case (2):
		desc = "CCITT 1D";
		break;
	case (3):
		desc = "CCITT Group 3";
		break;
	case (4):
		desc = "CCITT Group 4";
		break;
	case (5):
		desc = "LZW";
		break;
	case (6):
		desc = "JPEG";
		break;
	case (32773):
		desc = "Packbits";
		break;
	case (33432):
		desc = "Copyright";
		break;
	case (306):
		desc = "DateTime";
		break;
	case (269):
		desc = "DocumentName";
		break;
	case (336):
		desc = "DotRange";
		break;
	case (338):
		desc = "ExtraSamples";
		break;
	case (226):
		desc = "FillOrder";
		break;
	case (289):
		desc = "FreeByteCounts";
		break;
	case (288):
		desc = "FreeOffsets";
		break;
	case (291):
		desc = "GrayResponseCurve";
		break;
	case (290):
		desc = "GrayResponseUnity";
		break;
	case (321):
		desc = "HalftoneHints";
		break;
	case (316):
		desc = "HostComputer";
		break;
	case (270):
		desc = "ImageDescription";
		break;
	case (333):
		desc = "InkNames";
		break;
	case (332):
		desc = "InkSet";
		break;
	case (305):
		desc = "Software";
		break;
	case (325):
		desc = "TileByteCount";
		break;
	case (323):
		desc = "TileLength";
		break;
	case (324):
		desc = "TileOffsets";
		break;
	case (322):
		desc = "TileWidth";
		break;
	default:
		desc = "Not yet defined or unknown";
		break;
	}
	std::cout << desc.c_str() << std::endl;
	return desc;
}

Type GetType(short int typeID)
{
	Type type;
	//since there are many shared values among the types, we initialize the struct to the lowest common denominators and save ourselves lines in the Switch bellow.
	type.description = "";
	type.size = 1;
	type.isASCII = false;
	type.isFloat = false;
	type.isSigned = false;
	type.isRational = false;
	type.isUndefined = false;
	type.isUndetermined = false;

	switch (typeID)
	{
	case 1:
		type.description = "BYTE | 8-bit unsigned integer";
		break;
	case 2:
		type.description = "ASCII | 8-bit byte that contains a 7-bit ASCII code; the last bytemust be NUL (binary zero)";
		type.isASCII = true;
		break;
	case 3:
		type.description = "SHORT | 16-bit (2-byte) unsigned integer";
		type.size = 2;
		break;
	case 4:
		type.description = "LONG | 32-bit (4-byte) unsigned integer";
		type.size = 4;
		break;
	case 5:
		type.description = "RATIONAL | Two LONGs:  the first represents the numerator of afraction; the second, the denominator.";
		type.size = 8;
		type.isRational = true;
		break;
	case 6:
		type.description = "SBYTE | An 8-bit signed (twos-complement) integer.";
		type.isSigned = true;
		break;
	case 7:
		type.description = "UNDEFINED | An 8-bit byte that may contain anything, depending onthe definition of the field";
		type.isUndefined = true;
		break;
	case 8:
		type.description = "SSHORT | A 16-bit (2-byte) signed (twos-complement) integer."; //===
		type.size = 2;
		type.isSigned = true;
		break;
	case 9:
		type.description = "SLONG | A 32-bit (4-byte) signed (twos-complement) integer.";
		type.size = 4;
		type.isSigned = true;
		break;
	case 10:
		type.description = "SRATIONAL | Two SLONG’s:  the first represents the numerator of afraction, the second the denominator.";
		type.size = 8;
		type.isSigned = true;
		type.isRational = true;
		break;
	case 11:
		type.description = "FLOAT | Single precision (4-byte) IEEE format.";
		type.size = 4;
		type.isSigned = true;
		type.isFloat = true;
		break;
	case 12:
		type.description = "DOUBLE | Double precision (8-byte) IEEE format";
		type.size = 8;
		type.isSigned = true;
		type.isFloat = true;
		break;
	default:
		type.description = "ERROR! Could not determine type of field";
		type.isUndetermined = true;
		break;
	}

	//std::cout << desc.c_str() << std::endl;
	return type;
}

long int GetFieldIntData(Tag * tag)
{
	bool isOffsetData = false;
	if (tag->count * GetType(tag->fieldTypeID).size > 4)
	{
		//std::cout << "Field value size is greater than header value field capacity. These bytes are pointers." << std::endl; //test
		isOffsetData = true;
	}
	long int currentFileStreamLocation = stream.tellg();

	long int value;

	if (isOffsetData)
	{
		stream.seekg(tag->offsetValue);

		if (tag->fieldTypeID == 1)
		{
			char buffer[1];
			stream.read(buffer, sizeof(buffer));
			value = BytesToInt8(buffer);
		}
		if (tag->fieldTypeID == 3) //short
		{
			char buffer[2];
			stream.read(buffer, sizeof(buffer));
			value = BytesToInt16(buffer);
		}
		else if (tag->fieldTypeID == 4) //long
		{
			char buffer[4];
			stream.read(buffer, sizeof(buffer));
			value = BytesToInt32(buffer);
		}
		stream.seekg(currentFileStreamLocation);
	}
	else
	{
		value = tag->offsetValue;
	}

	return value;
}

void GetFieldIntArrayData(Tag * tag, long int * outputArray)
{
	bool isOffsetData = false;
	if (tag->count * GetType(tag->fieldTypeID).size > 4)
	{
		std::cout << "Field value size is greater than header value field capacity. These bytes are pointers." << std::endl; //test
		isOffsetData = true;
	}

	long int currentFileStreamLocation = stream.tellg();

	if (isOffsetData)
		stream.seekg(tag->offsetValue);

	if (tag->fieldTypeID == 3) //short
	{
		char buffer[2];
		for (int i = 0; i < tag->count; i++)
		{
			if (isOffsetData)
				stream.read(buffer, sizeof(buffer));
			else //This would only trigger if count equals 1 or 2.
			{ //TODO test this implementation
				long int temp = tag->offsetValue;
				if (isBigEndian)
				{
					buffer[0] = (char)((temp & (0xFF000000 >> (8 * i))) >> (8 * (1 + (tag->count - i))));
					buffer[1] = (char)((temp & (0x00FF0000 >> (8 * i))) >> (8 * (tag->count - i)));
				}
				else
				{
					buffer[0] = (char)((temp & (0x000000FF << (8 * i))) >> (8 * i));
					buffer[1] = (char)((temp & (0x0000FF00) << (8 * i)) >> (8 * (i + 1)));
				}
			}

			outputArray[i] = BytesToInt16(buffer);
		}
	}
	else if (tag->fieldTypeID == 4) //long
	{
		char buffer[4];
		for (int i = 0; i < tag->count; i++)
		{
			/*if (isOffsetData)
			{
				stream.read(buffer, sizeof(buffer));
				outputArray[i] = BytesToInt32(buffer);
			}
			else
				outputArray[i] = tag->offsetValue;*/

			//Note: the commented out implementation above is a cleaner, optimized version. Both it and th one bellow output the same result.
			//The implementation bellow, while works, doesn't make sense because the result when isOffsetData == false will always be equal to that of tag->offsetValue, because isOffsetData == false
			//only and only if there is a single long int value inside the tag->offsetValue (because count == 1).
			
			if (isOffsetData)
				stream.read(buffer, sizeof(buffer));
			else
			{
				long int temp = tag->offsetValue;
				if (isBigEndian)
				{
					buffer[0] = (char)((temp & 0xFF000000) >> 24);
					buffer[1] = (char)((temp & 0x00FF0000) >> 16);
					buffer[2] = (char)((temp & 0x0000FF00) >> 8);
					buffer[3] = (char)((temp & 0x000000FF));
				}
				else
				{
					buffer[0] = (char)((temp & 0x000000FF));
					buffer[1] = (char)((temp & 0x0000FF00) >> 8);
					buffer[2] = (char)((temp & 0x00FF0000) >> 16);
					buffer[3] = (char)((temp & 0xFF000000) >> 24);
				}
			}

			outputArray[i] = BytesToInt32(buffer);
		}
	}

	//test
	std::cout << "GetFieldIntArry result: " << std::endl;
	for (int i = 0; i < tag->count; i++)
		std::cout << outputArray[i] << std::endl;;
	
	//endtest


	stream.seekg(currentFileStreamLocation);
}

void ProcessTag(Tag * tag)
{
	switch (tag->tagID)
	{
	case (256): //width
		tiffDetails.width = GetFieldIntData(tag);
		break;
	case (257): //height
		tiffDetails.height = GetFieldIntData(tag);
		break;
	case (258): //bps
		tiffDetails.bitsPerSample = GetFieldIntData(tag);
		break;
	case (259): //compression
		tiffDetails.compression = GetFieldIntData(tag);
		break;
	case (273): //stripoffsets
	{
		std::cout << "allocating tileStripOffset array of rows: " << tag->count << std::endl; //test
		tiffDetails.tileStripOffset = std::unique_ptr<long int>(new long int[tag->count]);
		tiffDetails.noOfTilesOrStrips = tag->count;
		GetFieldIntArrayData(tag, tiffDetails.tileStripOffset.get());

		std::cout << "Result using GetFieldIntData() : " << GetFieldIntData(tag) << std::endl;//test
	}
		break;
	case (278): //rowsperstrip
		tiffDetails.bitmapFormat = BitmapFormat::strips;
		tiffDetails.rowsPerStrip = GetFieldIntData(tag);
		break;
	case (279): //stripbytecount
		tiffDetails.tileStripByteCount = GetFieldIntData(tag);
		break;
	case (325): //tilebytecount
		tiffDetails.bitmapFormat = BitmapFormat::tiles;
		tiffDetails.tileStripByteCount = GetFieldIntData(tag);
		break;
	case (323): //tilelength
		tiffDetails.tileHeight = GetFieldIntData(tag);
		break;
	case (324): //tileoffsets
	{
		tiffDetails.tileStripOffset = std::unique_ptr<long int>(new long int[tag->count]);
		tiffDetails.noOfTilesOrStrips = tag->count;
		GetFieldIntArrayData(tag, tiffDetails.tileStripOffset.get());
	}
		break;
	case (322): //tilewidth
		tiffDetails.tileWidth = GetFieldIntData(tag);
		break;
	case (277): //samplesperpixel
		tiffDetails.samplesPerPixel = GetFieldIntData(tag);
		break;
	case (338): //extrasampletype
		tiffDetails.extraSampleType = GetFieldIntData(tag);
		break;
	case (262):
		tiffDetails.photometricInterpretation = GetFieldIntData(tag);
		break;
	case (284):
		tiffDetails.planarConfiguration = GetFieldIntData(tag);
		break;
	case (339):
		tiffDetails.sampleFormat = GetFieldIntData(tag);
		break;
	default:
		break;
	}
}

void ParseUncompressedStripOrTileData(int stripOrTileID)
{
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	//TODO add a check here that the bites per sample are either 8, 16 or 32

	char ** pixel;
	pixel = new char *[tiffDetails.samplesPerPixel];
	for (int i = 0; i < tiffDetails.samplesPerPixel; i++)
		pixel[i] = new char[tiffDetails.bitsPerSample / 8];

	std::cout << "pixel buffer size: " << tiffDetails.samplesPerPixel << " x " << tiffDetails.bitsPerSample / 8 << std::endl;

	for (int i = 0; i < tiffDetails.noOfPixelsPerTileStrip; i++)
	{
		int xCoord = (stripOrTileID * tiffDetails.rowsPerStrip) + floor((float)i / (float)tiffDetails.width);
		int yCoord = i % tiffDetails.width;

		std::cout << "coords: " << xCoord << ", " << yCoord << std::endl;


		for (int j = 0; j < tiffDetails.samplesPerPixel; j++)
		{
			switch (tiffDetails.sampleFormat)
			{
			case (1): //unsigned int
				stream.read(pixel[j], tiffDetails.bitsPerSample / 8);
				bitMap[xCoord].SetValue(yCoord, j, (double)BytesToIntX(pixel[j], tiffDetails.bitsPerSample));
				break;
			
			case (2): //two’s complement signed integer data
				std::cout << "ERROR! Two’s Complement Signed Integer TIFFs aren't supported yet." << std::endl;
				break;
			
			case (3): //floats
				if (tiffDetails.bitsPerSample <= 32)
				{
					float _sample;
					stream.read((char*)&_sample, tiffDetails.bitsPerSample / 8);
					bitMap[xCoord].SetValue(yCoord, j, _sample);
				}
				else
				{
					double _sample;
					stream.read((char*)&_sample, tiffDetails.bitsPerSample / 8);
					bitMap[xCoord].SetValue(yCoord, j, _sample);
				}
				break;
			default: //default is unsigned int (case 1)
				stream.read(pixel[j], tiffDetails.bitsPerSample / 8);
				bitMap[xCoord].SetValue(yCoord, j, (double)BytesToIntX(pixel[j], tiffDetails.bitsPerSample));
				break;
			}
			std::cout << "Pixel Channel: " << j << ", value: " << bitMap[xCoord][yCoord][j] << std::endl; //test
		}

	}

	//release the pixel array we allocated above
	for (int i = 0; i < tiffDetails.samplesPerPixel; i++)
		delete[] pixel[i];
	delete[] pixel;


}

void ParseDeflateStripOrTileData(int stripOrTileID)
{

	std::cout << "Attempting to decompress a Deflate data" << std::endl;
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	short int compressionMethod, compressionInfo;
	short int checkBits, presetDictionary, compressionLevel;
	int windowSize;
	char byte[1];

	//Check first byte
	stream.read(byte, sizeof(byte));
	
	//divide byte into two 4-bits and check them
	compressionMethod = ((unsigned char)byte[0] & 0x0F); //filter out the upper 4 bits, leave the lower 4.
	compressionInfo = ((unsigned char)byte[0] & 0xF0) >> 4; //filter out the lower 4 bits and shift the upper 4 down.
	
	if (compressionMethod != 8)
		std::cout << "WARNING! The Deflate header's CINFO field is not equal to 8. At strip/tile: " << stripOrTileID << std::endl;

	windowSize = pow(2, (compressionInfo + 8));

	//Check the second byte
	stream.read(byte, sizeof(byte));

	checkBits = ((unsigned char)byte[0] & 0x1F); //filter out the upper 3 bits, leave lower 5.
	presetDictionary = ((unsigned char)byte[0] & 0x20) >> 5; //filter out everything but the 6th bit and shift it down.
	compressionLevel = ((unsigned char)byte[0] & 0x30) >> 6; //filter out everything but the last two bits and shift them down.

	if (presetDictionary != 0)
		std::cout << "WARNING! The Deflate FDICT field is set. This is against the TIFF6.0 standard. At strip/tile: " << stripOrTileID << std::endl;


	//check first block header (3 bits)
	short int blockHeaderBit, blockType[2];
	
	stream.read(byte, sizeof(byte));
	
	//test
	short int bits[8];
	bits[0] = ((unsigned char)byte[0] & 0x01);
	bits[1] = ((unsigned char)byte[0] & 0x02) >> 1;
	bits[2] = ((unsigned char)byte[0] & 0x04) >> 2;
	bits[3] = ((unsigned char)byte[0] & 0x08) >> 3;
	bits[4] = ((unsigned char)byte[0] & 0x10) >> 4;
	bits[5] = ((unsigned char)byte[0] & 0x20) >> 5;
	bits[6] = ((unsigned char)byte[0] & 0x40) >> 6;
	bits[7] = ((unsigned char)byte[0] & 0x80) >> 7;
	std::cout << "bits: " << bits[7] << bits[6] << bits[5] << bits[4] << bits[3] << bits[2] << bits[1] << bits[0] << std::endl;
	//endtest



	//test output
	std::cout << "Compression Method: " << compressionMethod << ", Compression Info: " << compressionInfo << std::endl;
	std::cout << "Window Size: " << windowSize << std::endl;	
	std::cout << "Check Bits: " << checkBits << ", Preset Dictionary: " << presetDictionary << ", Compression Level: " << compressionLevel << std::endl;

}

bool ParseStripOrTileData(int stripOrTileID)
{
	switch (tiffDetails.compression)
	{
	case (1): //no compression
		ParseUncompressedStripOrTileData(stripOrTileID);
		break;

	case (32773): //PackBits (Macintosh RLE)
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (2): //CCITT modified Huffman RLE
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (3): //CCITT Group 3 fax encoding
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (4): //CCITT Group 4 fax encoding
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (5): //LZW
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (6): //JPEG (old style, deprecated?)
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (7): //JPEG (new style)
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (8): //Deflate
		/*std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;*/
		ParseDeflateStripOrTileData(stripOrTileID);
		break;

	case (9): //"Defined by TIFF-F and TIFF-FX standard (RFC 2301) as ITU-T Rec. T.82 coding, using ITU-T Rec. T.85 (which boils down to JBIG on black and white). "
				//https://www.awaresystems.be/imaging/tiff/tifftags/compression.html
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	case (10): //"Defined by TIFF-F and TIFF-FX standard (RFC 2301) as ITU-T Rec. T.82 coding, using ITU-T Rec. T.43 (which boils down to JBIG on color). "
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;

	default:
		std::cout << "ERROR! Unsupported compression algorithm" << std::endl;
		return false;
		break;
	}
	return true;
}


bool main()
{
	stream.open("dem_GCS.tif", std::ios::binary | std::ios::in);
	//stream.open("testTIFF.tif", std::ios::binary | std::ios::in);
	//stream.open("testTIFF_32.tif", std::ios::binary | std::ios::in);

	if (!stream.is_open())
	{
		std::cout << "Could not open file" << std::endl;
		return false;
	}

	char byte[1];
	char word[2];
	char dword[4];
	char qword[8];

	//determine endianness
	std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	std::cout << "byte order: " << word[0] << word[1] << std::endl;

	switch (word[0])
	{
	case 'I':
		isBigEndian = false;
		std::cout << "Byte order set to little-endian" << std::endl;
		break;
	case 'M':
		isBigEndian = true;
		std::cout << "Byte order set to Big-endian" << std::endl;
		break;
	default:
		std::cout << "Could not determine the byte order of the file" << std::endl;
		break;
	}

	//check version (should always be 42)
	std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	std::cout << "Format version: " << BytesToInt16(word) << std::endl;

	//check offset to first IFD (Image File Directory)
	std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(dword, sizeof(dword));
	long int firstIFDOffset = BytesToInt32(dword); //might need it later
	std::cout << "First IFD offsett: " << firstIFDOffset << std::endl;

	//seek to first IFD begining
	stream.seekg(firstIFDOffset, stream.beg);
	//stream.seekg(21, stream.beg);

	//check IFD header
	std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	short int numberOfIFDEntries = BytesToInt16(word);
	std::cout << "Number of IFD enteries: " << numberOfIFDEntries << std::endl;

	for (int i = 0; i < numberOfIFDEntries; i++)
	{
		std::unique_ptr<Tag> tag = std::unique_ptr<Tag>(new Tag);

		//-----------------------------------------
		stream.read(word, sizeof(word));
		tag.get()->tagID = BytesToInt16(word);
		//-----------------------------------------
		stream.read(word, sizeof(word));
		tag.get()->fieldTypeID = BytesToInt16(word);
		//-----------------------------------------
		stream.read(dword, sizeof(dword));
		tag.get()->count = BytesToInt32(dword);
		//-----------------------------------------
		stream.read(dword, sizeof(dword));
		tag.get()->offsetValue = BytesToInt32(dword);

		if (viewTagsInCLI)
		{
			std::cout << "===================================================================" << std::endl;
			std::cout << "Tag loop: " << i << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t";
			std::cout << "Field identifying tag: " << tag.get()->tagID << " -- ";
			GetFieldDescription(tag.get()->tagID);
			std::cout << "Current file loc: " << stream.tellg() << "\t";
			std::cout << "Field type ID: " << tag.get()->fieldTypeID << " -- " << GetType(tag.get()->fieldTypeID).description.c_str() << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t";
			std::cout << "Count: " << tag.get()->count << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t";
			std::cout << "Value offset: " << tag.get()->offsetValue << std::endl;
		}

		ProcessTag(tag.get());
	}

	std::cout << "===================================================================" << std::endl;
	std::cout << "Finished processing tags" << std::endl;
	std::cout << "===================================================================" << std::endl;
	std::cout << "Tiff Details:" << std::endl;

	std::cout << "Dimensions :" << tiffDetails.width << " x " << tiffDetails.height << std::endl;
	std::cout << "Samples Per Pixel: " << tiffDetails.samplesPerPixel << std::endl;
	std::cout << "Bits Per Sample: " << tiffDetails.bitsPerSample << std::endl;
	std::cout << "Extra Samples Type: " << tiffDetails.extraSampleType << std::endl;
	std::cout << "Sample Format: " << tiffDetails.sampleFormat << std::endl;
	std::cout << "Compression: " << tiffDetails.compression << std::endl;

	std::cout << "Photmetric Interpretation: " << tiffDetails.photometricInterpretation << std::endl;
	std::cout << "Planar Configuration: " << tiffDetails.planarConfiguration << std::endl;

	tiffDetails.noOfPixelsPerTileStrip = tiffDetails.tileStripByteCount / (tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8);


	switch (tiffDetails.bitmapFormat)
	{
	case BitmapFormat::strips:
		std::cout << "Bitmap format: " << "strips" << std::endl;
		std::cout << "No. of Strip:" << tiffDetails.noOfTilesOrStrips << std::endl;
		std::cout << "Strip Byte Count: " << tiffDetails.tileStripByteCount << std::endl;
		std::cout << "Rows per Strip: " << tiffDetails.rowsPerStrip << std::endl;
		std::cout << "No. of Pixels per Strip: " << tiffDetails.noOfPixelsPerTileStrip << std::endl;

		std::cout << "--------------------" << std::endl;
		std::cout << "Strip offsets:" << std::endl;
		for (int i = 0; i < tiffDetails.noOfTilesOrStrips; i++)
			std::cout << "Offset " << i << ": " << tiffDetails.tileStripOffset.get()[i] << std::endl;
		std::cout << "--------------------" << std::endl;
		break;

	case BitmapFormat::tiles:
		std::cout << "Bitmap format: " << "tiles" << std::endl;
		std::cout << "No. of Tiles:" << tiffDetails.noOfTilesOrStrips << std::endl;
		std::cout << "Tile Byte Count: " << tiffDetails.tileStripByteCount << std::endl;
		std::cout << "Tile Height: " << tiffDetails.tileHeight << std::endl;
		std::cout << "Tile Width: " << tiffDetails.tileWidth << std::endl;
		std::cout << "No. of Pixels per Tile: " << tiffDetails.noOfPixelsPerTileStrip << std::endl;

		std::cout << "--------------------" << std::endl;
		std::cout << "Tile offsets:" << std::endl;
		for (int i = 0; i < tiffDetails.noOfTilesOrStrips; i++)
			std::cout << "Offset " << i << ": " << tiffDetails.tileStripOffset.get()[i] << std::endl;
		std::cout << "--------------------" << std::endl;
		break;

	case BitmapFormat::undefined:
		std::cout << "Bitmap format: " << "undefined" << std::endl;
		break;

	default:
		break;
	}

	//Allocate our bitmap in memory as an array of Array2D.
	//bitMap = std::unique_ptr<Array2D>(new Array2D[tiffDetails.height]);
	//for (int i = 0; i < tiffDetails.height; i++)
	//{
	//	bitMap.get()[i] = Array2D(tiffDetails.width, tiffDetails.samplesPerPixel);
	//}

	bitMap = new Array2D[tiffDetails.height];
	for (int i = 0; i < tiffDetails.height; i++)
	{
		bitMap[i] = Array2D(tiffDetails.width, tiffDetails.samplesPerPixel);
	}



	if (tiffDetails.planarConfiguration != 1)
	{
		std::cout << "ERROR! This reader cannot parse non-chunky (TIFF6.0 Planar Configuration other than 1) TIFF files." << std::endl;
	}
	else if (tiffDetails.bitsPerSample != 8 && tiffDetails.bitsPerSample != 16 && tiffDetails.bitsPerSample != 32)
	{
		std::cout << "ERROR! This reader can only parse 8, 16 and 32 bits-per-samples images." << std::endl;
	}
	else
	{
		for (int i = 0; i < tiffDetails.noOfTilesOrStrips; i++)
		{
			std::cout << "Data for strip no " << i << std::endl;
			//stream.seekg(tiffDetails.tileStripOffset.get()[i]);

			ParseStripOrTileData(i);
		}
	}


	//for (int i = 0; i < tiffDetails.height; i++)
	//	bitMap.get()[i].DisplayArrayInCLI();

	std::cout << "FINISHED!" << std::endl; //test
	return true;
}

