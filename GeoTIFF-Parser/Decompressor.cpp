#include "Decompressor.h"


void SetBitmapPixel(int _uv[2], double * const _pixel, Array2D * const _bitMap)
{
	for (int i = 0; i < tiffDetails.samplesPerPixel; i++)
		_bitMap[_uv[0]][_uv[1]][i] = _pixel[i];
}

double GetUIntSampleCurrentStreamPosition() //ONLY FOR USE WITH UNCOMPRESSED DATA!
{
	char * sample = new char [tiffDetails.bitsPerSample / 8];
	stream.read(sample, tiffDetails.bitsPerSample / 8);

	double result = (double)BytesToIntX(sample, tiffDetails.bitsPerSample);

	delete[] sample;
	return result;
}

void ParseUncompressedStripOrTileData(int stripOrTileID,  Array2D * const _bitMap)
{
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	double * pixel = new double[tiffDetails.samplesPerPixel];

	for (int i = 0; i < tiffDetails.noOfPixelsPerTileStrip; i++)
	{
		//cache the pixel's location in image. Note: These formulae are only tested for stripped images.
		//TODO wrap the xCoord and yCoord formulae bellow in an if-statement checking that the format is stripped image. And another if-statement (and of-course, do the math) for tiled images.

		int uv[2]; //coordinates of pixel in imagespace (x, y).
		uv[0] = (stripOrTileID * tiffDetails.rowsPerStrip) + floor((float)i / (float)tiffDetails.width); //formerly: xCoord.
		uv[1] = i % tiffDetails.width; //formerly: yCoord.

		for (int j = 0; j < tiffDetails.samplesPerPixel; j++)
		{
			switch (tiffDetails.sampleFormat)
			{
			case (1): //unsigned int
				pixel[j] = GetUIntSampleCurrentStreamPosition();
				break;

			case (2): //two’s complement signed integer data
			{
				std::cout << "ERROR! Two’s Complement Signed Integer TIFFs aren't supported yet." << std::endl;
				return;
			}
				break;

			case (3): //floating points
				if (tiffDetails.bitsPerSample <= 32) //single precision floats (float).
				{
					float _sample; //outputing the read value directly as double causes issues.
					stream.read((char*) &_sample, tiffDetails.bitsPerSample / 8);
					pixel[j] = _sample;
				}
				else //double precision floats (double).
					stream.read((char*)&pixel[j], tiffDetails.bitsPerSample / 8);
				break;

			default: //default is unsigned int (case 1)		
				pixel[j] = GetUIntSampleCurrentStreamPosition();
				break;
			}
		}

		SetBitmapPixel(uv, pixel, _bitMap);
	}

	delete[] pixel;
}



void ParseDeflateStripOrTileData(int stripOrTileID, Array2D * const _bitMap)
{
	std::cout << "Attempting to decompress a Deflate data" << std::endl;
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	short int compressionMethod, compressionInfo;
	short int checkBits, presetDictionary, compressionLevel;
	int windowSize;
	char byte[1];
	char word[2];

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
	short int blockHeaderBit, blockTypeBits[2]; //in this blockTypeBits array, bits are stored RTL (i.e. blockTypeBits[0] is least significant)
	DeflateBlockType blockType;
	bool isLastBlock = false;

	while (!isLastBlock)
	{
		////read out the block header (read entire byte then extract 3-top bits.
		stream.read(byte, sizeof(byte));
		blockHeaderBit = ((unsigned char)byte[0] & 0x01);
		blockTypeBits[0] = ((unsigned char)byte[0] & 0x02) >> 1;
		blockTypeBits[1] = ((unsigned char)byte[0] & 0x04) >> 2;

		if (blockTypeBits[0] == 0 && blockTypeBits[1] == 0)
			blockType = DeflateBlockType::noCompression;
		else if (blockTypeBits[0] == 1 && blockTypeBits[1] == 0)
			blockType = DeflateBlockType::fixedHuffman;
		else if (blockTypeBits[0] == 0 && blockTypeBits[1] == 1)
			blockType = DeflateBlockType::dynamicHuffman;
		else
			blockType = DeflateBlockType::unused;
		
		if (blockHeaderBit == 1)
			isLastBlock = true;

		std::string _bType = blockType == DeflateBlockType::noCompression ? "No Compression" : blockType == DeflateBlockType::fixedHuffman ? "Fixed Huffman" : blockType == DeflateBlockType::dynamicHuffman ? "Dynamic Huffman" : "Reserved";
		std::cout << "Block Type: " << _bType.c_str() << ", Final Block Bit: " << blockHeaderBit << std::endl;


		switch (blockType)
		{
		case noCompression:
		{
			std::cout << "Found non-compressed block." << std::endl;//test
			//in noCompression case, the byte where the header is located has no other usefull data so we skip it.
			//The following word (2bytes) is the block length
			stream.read(word, sizeof(word));
			short int _blockLength = BytesToInt16(word);
			
			std::cout << "Block Length: " << _blockLength << std::endl;//test
			
			//the next word is the 1's compliment of _blockLength. Useless?
			stream.read(word, sizeof(word));

			//Now we have _blockLength bytes of uncompressed data.
			
		}
			break;

		case fixedHuffman:
			break;

		case dynamicHuffman:
			break;

		case unused:
			std::cout << "ERROR! Block compression type is set to reserved value." << std::endl;
			break;

		default:
			break;
		}




		//TODO uncomment the part above when you start working on this decompressor again.

		////test
		//std::cout << "---------------------------------------------------" << std::endl;
		//for (int i = 0; i < 10; i++)
		//{
		//	stream.read(byte, sizeof(byte));
		//	short int bits[8];
		//	bits[0] = ((unsigned char)byte[0] & 0x01);
		//	bits[1] = ((unsigned char)byte[0] & 0x02) >> 1;
		//	bits[2] = ((unsigned char)byte[0] & 0x04) >> 2;
		//	bits[3] = ((unsigned char)byte[0] & 0x08) >> 3;
		//	bits[4] = ((unsigned char)byte[0] & 0x10) >> 4;
		//	bits[5] = ((unsigned char)byte[0] & 0x20) >> 5;
		//	bits[6] = ((unsigned char)byte[0] & 0x40) >> 6;
		//	bits[7] = ((unsigned char)byte[0] & 0x80) >> 7;
		//	//std::cout << "bits: " << bits[7] << bits[6] << bits[5] << bits[4] << bits[3] << bits[2] << bits[1] << bits[0] << std::endl;
		//	std::cout << "bits: " << bits[0] << bits[1] << bits[2] << bits[3] << bits[4] << bits[5] << bits[6] << bits[7] << std::endl;
		//}
		//isLastBlock = true;
		//std::cout << "---------------------------------------------------" << std::endl;
		////end test

	}


	//test output
	std::cout << "Compression Method: " << compressionMethod << ", Compression Info: " << compressionInfo << std::endl;
	std::cout << "Window Size: " << windowSize << std::endl;
	std::cout << "Check Bits: " << checkBits << ", Preset Dictionary: " << presetDictionary << ", Compression Level: " << compressionLevel << std::endl;
}