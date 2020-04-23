#include "Decompressor.h"

//The "Smart" way to implement decompression was to have a universal parser function that is oblivious to compression type and only converts bytes to samples/pixels, and have it use a custom stream
//that is handled by a decompression function that decompresses (if ncessary) the source data and returns requested bytes.
//This is, however, beyond my capabilities at the moment, so I'll stick to the bruteforce (read: memory inefficient) way: Decompress entire strip/tile into memory, convert and copy data to specified
//bitmap array, deallocate decompressed, raw data.
//Alternatively, we *could* decompress to disk, then read from that disk into our bitmap. This trades memory use for speed.


//=====================================================================================================================================================================
//-----------------------------------------Uncompressed
//=====================================================================================================================================================================


void SetBitmapPixel(int _uv[2], double * const _pixel, Array2D * const _bitMap)
{
	for (int i = 0; i < tiffDetails.samplesPerPixel; i++)
	{
		_bitMap[_uv[0]][_uv[1]][i] = _pixel[i];
		//std::cout << "Set sample: " << _uv[0] << ", " << _uv[1] << ", " << i << ": " << _pixel[i] << std::endl; //test
	}
}

double GetIntSampleCurrentStreamPosition() //ONLY FOR USE WITH UNCOMPRESSED DATA READ FROM DISK!
{
	char * sample = new char [tiffDetails.bitsPerSample / 8];
	stream.read(sample, tiffDetails.bitsPerSample / 8);

	double result = (double)BytesToIntX(sample, tiffDetails.bitsPerSample);

	delete[] sample;
	return result;
}

double GetIntSamepleFromMemoryData(unsigned char * data, unsigned long int position)
{
	unsigned short int _bytesPerSample = tiffDetails.bitsPerSample / 8;
	char * sample = new char[_bytesPerSample];

	position = position * _bytesPerSample;

	for (int i = 0; i < _bytesPerSample; i++)
		sample[i] = data[position + i];

	double result = (double)BytesToIntX(sample, tiffDetails.bitsPerSample);

	delete[] sample;
	return result;
}

double GetDoubleSampleFromMemoryData(unsigned char * data, unsigned long int position)
{
	unsigned short int _bytesPerSample = tiffDetails.bitsPerSample / 8;
	char * sample = new char[_bytesPerSample];

	position = position * _bytesPerSample;

	for (int i = 0; i < _bytesPerSample; i++)
		sample[i] = data[_bytesPerSample + i];

	//double result = static_cast<double>(&sample);
	double result = 0;

	delete[] sample;
	return result;
}

void ParseDecompressedDataFromMemory(int stripOrTileID,
										Array2D * const _bitMap,
										unsigned char * data, //data container.
										unsigned long int noOfPixelsToParse, //For Compression = 1 TIFFs, this should equal the entire pixel count of the strip/tile.
										unsigned long int firstPixelOrder = 0) //relative to the current strip/tile. Used for when parsing data mid-strip or mid-tile (like in uncompressed blocks in deflate streams.)
{	
		double * pixel = new double[tiffDetails.samplesPerPixel];
	
		for (int i = firstPixelOrder; i < noOfPixelsToParse; i++)
		{
			//cache the pixel's location in image. Note: These formulae are only tested for stripped images.
			//TODO wrap the xCoord and yCoord formulae bellow in an if-statement checking that the format is stripped image. And another if-statement (and of-course, do the math) for tiled images.
	
			int uv[2]; //coordinates of pixel in imagespace (x, y).
			uv[0] = (stripOrTileID * tiffDetails.rowsPerStrip) + floor((float)i / (float)tiffDetails.width); //formerly: xCoord.
			uv[1] = i % tiffDetails.width; //formerly: yCoord.
	
			//TODO there is a bit in the refences that states a tile/strip may contain data not used in the image (due to division issues), check how to handle that case and adjust the check bellow accordingly.
			if (uv[0] > tiffDetails.width || uv[1] > tiffDetails.height)
			{
				std::cout << "Found unused pixels." << std::endl;
				return;
			}

			for (int j = 0; j < tiffDetails.samplesPerPixel; j++)
			{
				switch (tiffDetails.sampleFormat)
				{
				case (1): //unsigned int
					pixel[j] = GetIntSamepleFromMemoryData(data, i+j);
					break;
	
				case (2): //two’s complement signed integer data
				{
					/*std::cout << "ERROR! Two’s Complement Signed Integer TIFFs aren't supported yet." << std::endl;
					delete[] pixel;
					return;*/
					pixel[j] = GetIntSamepleFromMemoryData(data, i+j);
				}
				break;
	
				case (3): //floating points
					if (tiffDetails.bitsPerSample <= 32) //single precision floats (float).
					{
						//float _sample; //outputing the read value directly as double causes issues.
						//stream.read((char*)&_sample, tiffDetails.bitsPerSample / 8);
						//pixel[j] = _sample;
						std::cout << "Float PackBits data combination not yet implemented" << std::endl;
					}
					else //double precision floats (double).
						//stream.read((char*)&pixel[j], tiffDetails.bitsPerSample / 8);
						std::cout << "Double PackBits data combination not yet implemented" << std::endl;
					break;
	
				default: //default is unsigned int (case 1)		
					pixel[j] = GetIntSamepleFromMemoryData(data, i+j);
					break;
				}
			}
	
			SetBitmapPixel(uv, pixel, _bitMap);
		}
	
		delete[] pixel;
}

//The commented-out function bellow is a failed experiemnt.

//void ParseUncompressedData(	int stripOrTileID,
//							Array2D * const _bitMap,
//							unsigned long int dataBegining, //Position in file stream to begin reading uncompressed data from. For TIFFs with compression type = 1, this the begining of the strip/tile.
//							unsigned long int noOfPixelsToParse, //For Compression = 1 TIFFs, this should equal the entire pixel count of the strip/tile.
//							unsigned long int firstPixelOrder = 0 //relative to the current strip/tile. Used for when parsing data mid-strip or mid-tile (like in uncompressed blocks in deflate streams.
//							)
//{
//	stream.seekg(dataBegining);
//
//	double * pixel = new double[tiffDetails.samplesPerPixel];
//
//	for (int i = firstPixelOrder; i < noOfPixelsToParse; i++)
//	{
//		//cache the pixel's location in image. Note: These formulae are only tested for stripped images.
//		//TODO wrap the xCoord and yCoord formulae bellow in an if-statement checking that the format is stripped image. And another if-statement (and of-course, do the math) for tiled images.
//
//		int uv[2]; //coordinates of pixel in imagespace (x, y).
//		uv[0] = (stripOrTileID * tiffDetails.rowsPerStrip) + floor((float)i / (float)tiffDetails.width); //formerly: xCoord.
//		uv[1] = i % tiffDetails.width; //formerly: yCoord.
//
//		for (int j = 0; j < tiffDetails.samplesPerPixel; j++)
//		{
//			switch (tiffDetails.sampleFormat)
//			{
//			case (1): //unsigned int
//				pixel[j] = GetUIntSampleCurrentStreamPosition();
//				break;
//
//			case (2): //two’s complement signed integer data
//			{
//				std::cout << "ERROR! Two’s Complement Signed Integer TIFFs aren't supported yet." << std::endl;
//				return;
//			}
//			break;
//
//			case (3): //floating points
//				if (tiffDetails.bitsPerSample <= 32) //single precision floats (float).
//				{
//					float _sample; //outputing the read value directly as double causes issues.
//					stream.read((char*)&_sample, tiffDetails.bitsPerSample / 8);
//					pixel[j] = _sample;
//				}
//				else //double precision floats (double).
//					stream.read((char*)&pixel[j], tiffDetails.bitsPerSample / 8);
//				break;
//
//			default: //default is unsigned int (case 1)		
//				pixel[j] = GetUIntSampleCurrentStreamPosition();
//				break;
//			}
//		}
//
//		SetBitmapPixel(uv, pixel, _bitMap);
//	}
//
//	delete[] pixel;
//}


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

		//TODO there is a bit in the refences that states a tile/strip may contain data not used in the image (due to division issues), check how to handle that case and adjust the check bellow accordingly.
		if (uv[0] > tiffDetails.width || uv[1] > tiffDetails.height)
		{
			std::cout << "Found unused pixels." << std::endl;
			return;
		}

		for (int j = 0; j < tiffDetails.samplesPerPixel; j++)
		{
			switch (tiffDetails.sampleFormat)
			{
			case (1): //unsigned int
				pixel[j] = GetIntSampleCurrentStreamPosition();
				break;

			case (2): //two’s complement signed integer data
			{
				pixel[j] = GetIntSampleCurrentStreamPosition();
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
				pixel[j] = GetIntSampleCurrentStreamPosition();
				break;
			}
		}

		SetBitmapPixel(uv, pixel, _bitMap);
	}

	delete[] pixel;

	//ParseUncompressedData(stripOrTileID, _bitMap, tiffDetails.tileStripOffset.get()[stripOrTileID], tiffDetails.noOfPixelsPerTileStrip);
}



//=====================================================================================================================================================================
//-----------------------------------------Deflate
//=====================================================================================================================================================================

unsigned short int noOfLiteralLengthCodes = 0, noOfDistanceCodes = 0, noOfCodeLengthCodes = 0;
unsigned short int huffmanCodeLengths[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
unsigned short int huffmanCodeLegnthsCodes[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//std::unique_ptr<unsigned short int> codeLengthCodes; //code lengths for the code length alphabet
std::unique_ptr<unsigned short int> literalLengthsAlphabet; //code lengths for the literal/length alphabet
std::unique_ptr<unsigned short int> distanceAlphabet; //code lengths for the distance alphabet

unsigned short int bitsRemainingInByte = 0;
unsigned char currentByte;

unsigned char GetNextBit() //returns next bit stored as the lest significant bit of a byte (because CPP doesn't support stand-alone bits)
{
	if (bitsRemainingInByte < 1)
	{
		stream.read((char*)&currentByte, sizeof(currentByte));
		bitsRemainingInByte = 8;
	}

	//unsigned char result = ((currentByte & (unsigned char)pow(2, bitsRemainingInByte - 1)) >> (bitsRemainingInByte - 1)); //this returns the most-significant bit firs
	unsigned char result = ((currentByte & (unsigned char)pow(2, 8 - bitsRemainingInByte)) >> (8 - bitsRemainingInByte)); //this returns least significant bit first.

	//std::cout << "returning bit: " << (unsigned short int)result <<std::endl;//test

	bitsRemainingInByte--;
	return result;
}

unsigned char GetNextOctet() //Assumes least significant bit is stored first (i.e. for use in data other than the Huffman tree).
{
	unsigned char result = 0x00;
	
	for (int i = 0; i < 8; i++)
		result = result | (GetNextBit() << i);

	return result;
}


//TODO the function bellow may not be as simple as it is now. Should research this more.
void DecompressHuffmanAlphabet(std::vector<unsigned char> * targetAlphabet, int noOfCodes)
{
	for (int i = 0; i < noOfCodes; i++)
	{
		unsigned char _code = 0x00;
		for (int j = 0; j < 3; j++)
			_code = ((unsigned char)_code << 1) | GetNextBit();

		//if ((unsigned short int) _code > 0)
		{
			//if ((unsigned short int) _code > 0 && (unsigned short int) _code < 16) //means the code is the length.
			if ( (unsigned short int) _code < 16) //means the code is the length.
				targetAlphabet->push_back(_code);
			else //means we need to copy the last length a set number of times.
			{
				unsigned short int _noOfRepeats;
				unsigned short int _noOfRepeatCodeBits;
				unsigned short int _repeatCodePaddingVal; //value to convert repeat code to no of repeats (by normal addition).
				unsigned char _repeatCode = 0x00;

				if ((unsigned short int) _code == 16)
				{
					_noOfRepeatCodeBits = 2;
					_repeatCodePaddingVal = 3;
				}
				else if ((unsigned short int) _code == 17)
				{
					_noOfRepeatCodeBits = 3;
					_repeatCodePaddingVal = 3;
				}
				else if ((unsigned short int) _code == 18)
				{
					_noOfRepeatCodeBits = 7;
					_repeatCodePaddingVal = 11;
				}
				else //should not happen.
				{
					std::cout << "ERROR! Unexpected Code Length Code: " << (unsigned short int) _code <<  " found when constructing dynamic huffman code." << std::endl;
				}

				for (int k = 0; k < _noOfRepeatCodeBits; k++)
					_repeatCode = (_repeatCode << 1) | GetNextBit();

				_noOfRepeats = (unsigned short int) _repeatCode + _repeatCodePaddingVal;

				for (int j = 0; j < _noOfRepeats; j++)
					targetAlphabet->push_back(targetAlphabet->back());
			}
		}
	}
}

void BuildHuffmanTree()
{
	std::cout << "Building dynamic Huffman tree" << std::endl; //test


	//read No. of literal/length codes - HLIT
	for (int i = 0; i < 5; i++)
		noOfLiteralLengthCodes = ((unsigned char)noOfLiteralLengthCodes << 1) | GetNextBit();
	noOfLiteralLengthCodes += 257;

	//read No. of Distance Codes - HDIST
	for (int i = 0; i < 5; i++)
		noOfDistanceCodes = ((unsigned char)noOfDistanceCodes << 1) | GetNextBit();
	noOfDistanceCodes += 1;

	//read No. of Code Length codes - HCLEN
	for (int i = 0; i < 4; i++)
		noOfCodeLengthCodes = ((unsigned char)noOfCodeLengthCodes << 1) | GetNextBit();
	noOfCodeLengthCodes += 4;

	//codeLengthCodes = std::unique_ptr<unsigned short int>(new unsigned short int[19]);
	for (int i = 0; i < noOfCodeLengthCodes + 4; i++)
	{
		unsigned short int _count = 0;
		for (int j = 0; j < 3; j++)
			_count = ((unsigned char)_count << 1) | GetNextBit();
		
		if (i < 19)
			huffmanCodeLegnthsCodes[i] = _count;
		else
			std::cout << "ERROR! Number of Code Length Codes is greater than 19." << std::endl;
	}

	//The part bellow is somewhat redundant. Decompressing the Huffman tree is done into std::vectors, but the actuall end result is mean to be a basic arrays (std::unique_ptr).
	//TODO remove this redundancy by either using the vectors as an end container, or estimate size and allocate the pointers beforehand.

	//extract literl/lengths alphabet.
	std::vector<unsigned char> _literalLengthAlphabet;
	DecompressHuffmanAlphabet(&_literalLengthAlphabet, noOfLiteralLengthCodes);

	//extract distance alphabet's code lengths
	std::vector<unsigned char> _distanceAlphabet;
	DecompressHuffmanAlphabet(&_distanceAlphabet, noOfDistanceCodes);


	//initialize and copy to our main alphabet containers.
	literalLengthsAlphabet = std::unique_ptr<unsigned short int>(new unsigned short int[_literalLengthAlphabet.size()]);
	distanceAlphabet = std::unique_ptr<unsigned short int>(new unsigned short int[_distanceAlphabet.size()]);
	
	for (int i = 0; i < _literalLengthAlphabet.size(); i++)
		literalLengthsAlphabet.get()[i] = _literalLengthAlphabet[i];
	
	for (int i = 0; i < _distanceAlphabet.size(); i++)
		distanceAlphabet.get()[i] = _distanceAlphabet[i];

	//test
	std::cout << "noOfLiteralLengthCodes: " << noOfLiteralLengthCodes << std::endl;
	std::cout << "noOfDistanceCodes: " << noOfDistanceCodes << std::endl;
	std::cout << "noOfCodeLengthCodes: " << noOfCodeLengthCodes << std::endl;
	std::cout << "---------------------------------------------------------";
	std::cout << "Literal/Length codes: No: " << _literalLengthAlphabet.size() << std::endl;
	for (std::vector<unsigned char>::iterator it = _literalLengthAlphabet.begin(); it < _literalLengthAlphabet.end(); ++it)
		std::cout << (unsigned short int)*it << "\t";
	std::cout << std::endl;
	std::cout << "---------------------------------------------------------";
	std::cout << "Distance codes: No: " << _distanceAlphabet.size() << std::endl;
	for (std::vector<unsigned char>::iterator it = _distanceAlphabet.begin(); it < _distanceAlphabet.end(); ++it)
		std::cout << (unsigned short int)*it << "\t";
	std::cout << std::endl;



}

unsigned char DecodeValue(unsigned char value)
{
	unsigned char result = 0x00;

	return result;
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
	std::vector<unsigned char> uncompressedRawData; //the buffer we're putting into our unompressed data.
											//Using a vector is questionable here. Performance should be tested.

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
	unsigned long int currentPixelOrder = 0;

	while (!isLastBlock && !stream.eof())
	{
		//TODO The part bellow assumes header begins at byte boundary, this is false. Only the first block's header is.

		//read out the block header (read entire byte then extract 3-top bits.
		
		/*stream.read(byte, sizeof(byte));
		blockHeaderBit = ((unsigned char)byte[0] & 0x01);
		blockTypeBits[0] = ((unsigned char)byte[0] & 0x02) >> 1;
		blockTypeBits[1] = ((unsigned char)byte[0] & 0x04) >> 2;
*/

		blockHeaderBit = GetNextBit();
		blockTypeBits[0] = GetNextBit();
		blockTypeBits[1] = GetNextBit();

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

		//test
		std::string _bType = blockType == DeflateBlockType::noCompression ? "No Compression" : blockType == DeflateBlockType::fixedHuffman ? "Fixed Huffman" : blockType == DeflateBlockType::dynamicHuffman ? "Dynamic Huffman" : "Reserved";
		std::cout << "Block Type: " << _bType.c_str() << ", Final Block Bit: " << blockHeaderBit << std::endl;
		//end test

		//Decompress the block itself.
		switch (blockType)
		{
		case noCompression:
		{
			std::cout << "Found non-compressed block." << std::endl;//test
			//in noCompression case, the byte where the header is located has no other usefull data so we skip it. We don't need to use GetNextBit() here.
			//The following word (2bytes) is the block length
			stream.read(word, sizeof(word));
			
			short int _blockLength = BytesToInt16(word); //block length is size of uncompressed block in bytes.
			
			std::cout << "Block Length: " << _blockLength << std::endl;//test
			
			//the next word is the 1's compliment of _blockLength. Useless?
			stream.read(word, sizeof(word));

			//The remaining data we put into our compressed data buffer
			for (int i = 0; i < _blockLength; i++)
			{
				stream.read(byte, sizeof(byte));
				uncompressedRawData.push_back((unsigned char)byte[0]);
			}
		}
			break;

		case fixedHuffman:
		{
			std::cout << "Found fixed-Huffman block." << std::endl;//test




		}
			break;

		case dynamicHuffman:
		{
			std::cout << "Found dynamic-Huffman block." << std::endl;//test

			//first, build the Huffman tree.
			BuildHuffmanTree();
			
			//at this point, the stream should be at the begining of the compressed data.
			while (!stream.eof())
			{
				unsigned char _byte;
				_byte = DecodeValue(GetNextOctet());

				if ((unsigned short int) _byte < 256) //literal.
					uncompressedRawData.push_back(_byte);
				//else if ()


				else if ((unsigned short int) _byte == 256) //end of compressed data
					break;
			}
		}
			break;

		case unused:
			std::cout << "ERROR! Block compression type is set to reserved value. This is against spec." << std::endl;
			return;
			break;

		default:
			break;
		}

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


	//TODO convert the uncompressedRawData buffer to actual pixel/sample values and copy them into _bitMap 
	
	////test output
	//std::cout << "Compression Method: " << compressionMethod << ", Compression Info: " << compressionInfo << std::endl;
	//std::cout << "Window Size: " << windowSize << std::endl;
	//std::cout << "Check Bits: " << checkBits << ", Preset Dictionary: " << presetDictionary << ", Compression Level: " << compressionLevel << std::endl;
}

//=====================================================================================================================================================================
//-----------------------------------------PackBits
//=====================================================================================================================================================================


void ParsePackBitsStripOrTileData(int stripOrTileID, Array2D * const _bitMap)
{
	std::cout << "Attempted to decompress PackBits data." << std::endl; //test
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);


	//estimate number of bytes we excpect to have
	//unsigned long int noOfBytes = tiffDetails.noOfPixelsPerTileStrip * tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8;
	unsigned long int noOfBytes = tiffDetails.height * (tiffDetails.width + 7) * tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8;
	std::cout << "noOfBytes: " << noOfBytes << std::endl; //test


	unsigned long int counter = 0; //counter is used to track of how many bytes we've extracted.
	std::unique_ptr<unsigned char> uncompressedRawData = std::unique_ptr<unsigned char>(new unsigned char[noOfBytes]);
	char byte[1]; //Used only for the header.

	while (counter < noOfBytes)
	{
		stream.read(byte, sizeof(byte));
		//short int _window = (short int) byte[0]; //For packbites:  0 <= _window <= 127: literal bytes; -127 <= _window <=-1 repeated bytes; _window = -128 no operation.
		short int _window = BytesToInt8(byte); //For packbites:  0 <= _window <= 127: literal bytes; -127 <= _window <=-1 repeated bytes; _window = -128 no operation.

		std::cout << "Counter: " << counter << ", _window: " << _window << std::endl; //test

		unsigned char _byte; //used for the actuall datastream bytes.
		//unsigned char _word[2];
		if (_window > -1 && _window < 128)//read n+1 bytes.
		{
			for (int i = 0; i < _window + 1; i++) 
			{
				stream.read((char*) &_byte, sizeof(_byte));
				uncompressedRawData.get()[counter] = _byte;
				counter++;

				/*stream.read((char*)_word, sizeof(_word));
				uncompressedRawData.get()[counter] = _word[0];
				uncompressedRawData.get()[counter + 1] = _word[1];
				counter += 2;*/

				if (counter >= noOfBytes)
					break;
			}
		}
		else if (_window < 0 && _window > -128) //read one byte, repeast 1 - n times.
		{

			stream.read((char*)&_byte, sizeof(_byte));
			//stream.read((char*)_word, sizeof(_word));
			for (int i = 0; i < 1 - _window ; i++)
			{
				uncompressedRawData.get()[counter] = _byte;
				counter++;

				//uncompressedRawData.get()[counter] = _word[0];
				//uncompressedRawData.get()[counter + 1] = _word[1];
				//counter += 2;
				
				if (counter >= noOfBytes)
					break;
			}

		}
		else
		{
			std::cout << "Packbits - NoOp." << std::endl;
		}
		
	}
	//test
	std::cout << "Extracted : " << counter << std::endl; 

	for (int i = 0; i < counter; i++)
		std::cout << (short int)uncompressedRawData.get()[i] << "\t";
	//end test
	
	ParseDecompressedDataFromMemory(stripOrTileID, _bitMap, uncompressedRawData.get(), tiffDetails.noOfPixelsPerTileStrip, 0);

	std::cout << "Finished decompression." << std::endl;
}