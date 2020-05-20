#include "Decompressor.h"

//The "Smart" way to implement decompression was to have a universal parser function that is oblivious to compression type and only converts bytes to samples/pixels, and have it use a custom stream
//that is handled by a decompression function that decompresses (if ncessary) the source data and returns requested bytes.
//This is, however, beyond my capabilities at the moment, so I'll stick to the bruteforce (read: memory inefficient) way: Decompress entire strip/tile into memory, convert and copy data to specified
//bitmap array, deallocate decompressed, raw data.
//Alternatively, we *could* decompress to disk, then read from that disk into our bitmap. This trades memory use for speed.


//This code needs some serious DRY'ing...

//=====================================================================================================================================================================
//-----------------------------------------Uncompressed
//=====================================================================================================================================================================


void SetBitmapPixel(unsigned long int _uv[2], const double * const _pixel, Array2D * const _bitMap)
{
	for (unsigned long int i = 0; i < tiffDetails.samplesPerPixel; i++)
	{
		_bitMap[_uv[0]][_uv[1]][i] = _pixel[i];
		
		//if (_uv[0] >= tiffDetails.width - 1 && _uv[1] >= tiffDetails.height - 1) //test
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

double GetIntSamepleFromMemoryData(const unsigned char * data, unsigned long int position)
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

double GetFloatSampleFromMemoryData(const unsigned char * data, unsigned long int position)
{
	unsigned short int _bytesPerSample = tiffDetails.bitsPerSample / 8;
	char * sample = new char[_bytesPerSample];

	position = position * _bytesPerSample;

	for (int i = 0; i < _bytesPerSample; i++)
		sample[i] = data[position + i];

	//for (int i = _bytesPerSample - 1; i >= 0; i--)
	//	sample[i] = data[position + (_bytesPerSample - i - 1)];

	double result = *reinterpret_cast<float *>(sample);

	//std::cout << "\nbytespersample: " << _bytesPerSample << std::endl;
	//std::cout << "returning: " << result << std::endl;

	delete[] sample;
	return result;
}

double GetDoubleSampleFromMemoryData(const unsigned char * data, unsigned long int position)
{
	unsigned short int _bytesPerSample = tiffDetails.bitsPerSample / 8;
	char * sample = new char[_bytesPerSample];

	position = position * _bytesPerSample;

	for (int i = 0; i < _bytesPerSample; i++)
		sample[i] = data[position + i];
	
	//for (int i = _bytesPerSample - 1; i >= 0; i--)
	//	sample[i] = data[position + (_bytesPerSample - i - 1)];
	
	double result = *reinterpret_cast<double *>(sample);

	delete[] sample;
	return result;
}

void ParseDecompressedDataFromMemory(unsigned int stripOrTileID,
										Array2D * const _bitMap,
										const unsigned char * data, //raw data container.
										unsigned long int noOfPixelsToParse, //For Compression = 1 TIFFs, this should equal the entire pixel count of the strip/tile.
										unsigned long int firstPixelOrder = 0) //relative to the current strip/tile. Used for when parsing data mid-strip or mid-tile (like in uncompressed blocks in deflate streams.)
{	
		double * pixel = new double[tiffDetails.samplesPerPixel];
	
		for (unsigned long int i = firstPixelOrder; i < noOfPixelsToParse; i++)
		{
			//cache the pixel's location in image. Note: These formulae are only tested for stripped images.
			//TODO wrap the xCoord and yCoord formulae bellow in an if-statement checking that the format is stripped image. And another if-statement (and of-course, do the math) for tiled images.
	
			unsigned long int uv[2]; //coordinates of pixel in imagespace (x, y).
			uv[0] = i % tiffDetails.width; //xCoord
			uv[1] = (stripOrTileID * tiffDetails.rowsPerStrip) + static_cast<unsigned long int>(floor((float)i / (float)tiffDetails.width)); //yCoord.

			//TODO there is a bit in the references that states a tile/strip may contain data not used in the image (due to division issues), check how to handle that case and adjust the check bellow accordingly.
			if (uv[0] >= tiffDetails.width || uv[1] >= tiffDetails.height)
			{
				std::cout << "Found unused pixels at i = " << i << std::endl;
				return;
			}

			for (unsigned long int j = 0; j < tiffDetails.samplesPerPixel; j++)
			{
				switch (tiffDetails.sampleFormat)
				{
				case (1): //unsigned int
					pixel[j] = GetIntSamepleFromMemoryData(data, i+j);
					break;
	
				case (2): //two’s complement signed integer data
					pixel[j] = GetIntSamepleFromMemoryData(data, i+j);
					break;
	
				case (3): //floating points
					if (tiffDetails.bitsPerSample <= 32) //single precision floats (float).
					{
						pixel[j] = GetFloatSampleFromMemoryData(data, i + j);
					}
					else //double precision floats (double).
						pixel[j] = GetDoubleSampleFromMemoryData(data, i + j);

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

void ParseUncompressedStripOrTileData(int stripOrTileID,  Array2D * const _bitMap)
{
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	double * pixel = new double[tiffDetails.samplesPerPixel];

	for (unsigned long int i = 0; i < tiffDetails.noOfPixelsPerTileStrip; i++)
	{
		//cache the pixel's location in image. Note: These formulae are only tested for stripped images.
		//TODO wrap the xCoord and yCoord formulae bellow in an if-statement checking that the format is stripped image. And another if-statement (and of-course, do the math) for tiled images.

		unsigned long int uv[2]; //coordinates of pixel in imagespace (x, y).
		uv[0] = i % tiffDetails.width; //xCoord.
		uv[1] = (stripOrTileID * tiffDetails.rowsPerStrip) + static_cast<unsigned long>(floor((float)i / (float)tiffDetails.width)); //yCoord.

		//TODO there is a bit in the refences that states a tile/strip may contain data not used in the image (due to division issues), check how to handle that case and adjust the check bellow accordingly.
		if (uv[0] > tiffDetails.width || uv[1] > tiffDetails.height)
		{
			std::cout << "Found unused pixels at i = " << i << std::endl;
			return;
		}

		for (unsigned long int j = 0; j < tiffDetails.samplesPerPixel; j++)
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
}

//=====================================================================================================================================================================
//-----------------------------------------Deflate
//=====================================================================================================================================================================
#pragma region Deflate Decompression
unsigned short int noOfLiteralLengthCodes = 0, noOfDistanceCodes = 0, noOfCodeLengthCodes = 0;
unsigned short int huffmanCodeLengthsLiterals[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
unsigned short int huffmanCodeLengths[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
std::unique_ptr<unsigned short int> literalLengthsAlphabet; //code lengths for the literal/length alphabet
std::unique_ptr<unsigned short int> distanceAlphabet; //code lengths for the distance alphabet

unsigned short int bitsRemainingInByte = 0;
unsigned char currentByte;

class HuffmanTreeNode
{
public:

	HuffmanTreeNode() 
	{
		left = std::unique_ptr<HuffmanTreeNode>(nullptr);
		right = std::unique_ptr<HuffmanTreeNode>(nullptr);
		value = std::unique_ptr<unsigned short int>(nullptr);
	};
	~HuffmanTreeNode() 
	{ 
	};

	HuffmanTreeNode * NextNode(unsigned int direction)
	{
		if (direction == 0)
			return left.get();
		else if (direction == 1)
			return right.get();
		else
			std::cout << "ERRROR! Attempting to traverse Huffman tree with a non binary value." << std::endl;
		return NULL;
	};

	unsigned short int * Value()
	{
		return value.get();
	};

	HuffmanTreeNode * AddNode(unsigned int direction)
	{
		if (direction == 0)
		{
			left = std::unique_ptr<HuffmanTreeNode>( new HuffmanTreeNode());
			return left.get();
		}
		else if (direction == 1)
		{
			right = std::unique_ptr<HuffmanTreeNode>(new HuffmanTreeNode());
			return right.get();
		}
		else
			std::cout << "ERRROR! Attempting to add a Huffman tree node with a non binary direction." << std::endl;
		return NULL;
	};
	
	void SetValue(unsigned char _value)
	{
		if (value.get() == NULL)
			value = std::unique_ptr<unsigned short int>(new unsigned short int);


		*value = _value;
	}

private:
 	std::unique_ptr<HuffmanTreeNode> left;
	std::unique_ptr<HuffmanTreeNode> right;
	std::unique_ptr<unsigned short int> value;
};

std::unique_ptr<HuffmanTreeNode> codeLengthHuffmanTree;
std::unique_ptr<HuffmanTreeNode> litLengthHuffmanTree;
std::unique_ptr<HuffmanTreeNode> distHuffmanTree;

unsigned char GetNextBit() //returns next bit stored as the lest significant bit of a byte (because CPP doesn't support stand-alone bits)
{
	if (bitsRemainingInByte < 1)
	{
		stream.read((char*)&currentByte, sizeof(currentByte));
		bitsRemainingInByte = 8;
	}

	//unsigned char result = ((currentByte & (unsigned char)pow(2, bitsRemainingInByte - 1)) >> (bitsRemainingInByte - 1)); //this returns the most-significant bit firs
	unsigned char result = ((currentByte & (unsigned char)pow(2, 8 - bitsRemainingInByte)) >> (8 - bitsRemainingInByte)); //this returns least significant bit first.

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

unsigned short int DecodeNextValue(const HuffmanTreeNode * tree)
{

	unsigned short int * nodeValue = NULL;
	HuffmanTreeNode * node = codeLengthHuffmanTree.get();

	while (nodeValue == NULL)
	{
		node = node->NextNode((unsigned short int)GetNextBit());

		if (node == NULL)
		{
			std::cout << "ERROR! Found a code in datastream not described in the Huffman Tree." << std::endl;
			return 256;
		}

		nodeValue = node->Value();
	}

	return *nodeValue;
}

void SetSortedHuffmanCodeLengthCode(unsigned short int unsortedOrder, unsigned short int value)
{
	if (unsortedOrder > 18)
		std::cout << "ERROR! Attempting to set a value for a code length > 18." << std::endl;

	huffmanCodeLengths[huffmanCodeLengthsLiterals[unsortedOrder]] = value;
}

bool BuildHuffmanTree(const unsigned short int * codeLengths, const unsigned short int noOfCodes, const int maxCode, std::unique_ptr < HuffmanTreeNode> * tree)
{
	std::cout << "Building tree" << std::endl;
	const int maxBits = 15;
	std::unique_ptr<unsigned short int> nextCode = std::unique_ptr<unsigned short int>(new unsigned short int[maxBits]);
	std::unique_ptr<unsigned short int> codes = std::unique_ptr<unsigned short int>( new unsigned short int[maxCode]);
	for (int i = 0; i < maxCode; i++)
		codes.get()[i] = 0;

	std::unique_ptr<unsigned short int> codeLengthFrequency = std::unique_ptr<unsigned short int>(new unsigned short int[maxBits]);

	//Step 1: Count length frequencies.
	for (int i = 0; i <= maxBits; i++)
	{
		int count = 0;
		for (int j = 0; j < noOfCodes; j++)
		{
			if (codeLengths[j] == i)
				count++;
		}
		codeLengthFrequency.get()[i] = count;
		std::cout << count << "\t"; //test
	}
	std::cout << std::endl; //test


	//Step 2: finding numerical value of the smallest code for each code length.
	unsigned short int _code = 0;
	codeLengthFrequency.get()[0] = 0;

	for (int i = 1; i <= maxBits; i++)
	{
		_code = ((_code + codeLengthFrequency.get()[i - 1]) << 1);
		nextCode.get()[i] = _code;
	}

	//Step 3: Assign numerical values to all codes.

	for (int i = 0; i <= noOfCodes; i++)
	{
		unsigned short int length = codeLengths[i];
		if (length != 0)
		{
			codes.get()[i] = nextCode.get()[length];
			nextCode.get()[length]++;
		}
	}

	//Fill the tree's linked list.
	*tree = std::unique_ptr<HuffmanTreeNode>(new HuffmanTreeNode());

	for (int i = 0; i <= noOfCodes; i++)
	{
		//unsigned long int _size = 8 * sizeof(codes.get()[0]);
		unsigned short int * bits = new unsigned short int[maxBits];
		for (int j = 0; j < maxBits; j++)
			bits[j] = ((unsigned char)codes.get()[i] & (long int)pow(2, j)) >> j;

		HuffmanTreeNode * node = tree->get();

		std::cout << "Code: " << i << "\t";
		for (int j = codeLengths[i] - 1; j >= 0; j--)
		{
			//std::cout << bits[j] << " "; //test
			if (node->NextNode(bits[j]) == NULL)
			{
				node->AddNode(bits[j]);
			}
			node = node->NextNode(bits[j]);
		}
		//std::cout << std::endl; //test
		node->SetValue(i);

		delete[] bits;
	}

	std::cout << "Finished building tree" << std::endl;

	return true;
}

bool DecompressHuffmanAlphabet(std::vector<unsigned short int> * alphabetContainer, int noOfAlphabets)
{
	std::cout << "Decompressing alphabets" << std::endl; //test

	while (alphabetContainer->size() < noOfAlphabets)
	{
		unsigned short int nodeValue = DecodeNextValue(codeLengthHuffmanTree.get());

		if (nodeValue < 16)
		{
			alphabetContainer->push_back(nodeValue);
		}
		else
		{
			unsigned short int _noOfRepeats;
			unsigned short int _repeatValue;
			unsigned short int _noOfRepeatCodeBits;
			unsigned short int _repeatCodePaddingVal; //value to convert repeat code to no of repeats (by normal addition).
			unsigned char _repeatCode = 0x00;

			if (nodeValue == 16)
			{
				_noOfRepeatCodeBits = 2;
				_repeatCodePaddingVal = 3;
				_repeatValue = alphabetContainer->back();
			}
			else if (nodeValue == 17)
			{
				_noOfRepeatCodeBits = 3;
				_repeatCodePaddingVal = 3;
				_repeatValue = 0;
			}
			else if (nodeValue == 18)
			{
				_noOfRepeatCodeBits = 7;
				_repeatCodePaddingVal = 11;
				_repeatValue = 0;
			}
			else //should not happen.
			{
				std::cout << "ERROR! Unexpected Code Length Code: " << nodeValue << " found when constructing dynamic huffman code." << std::endl;
				return false;
			}

			for (int k = 0; k < _noOfRepeatCodeBits; k++)
				_repeatCode = (_repeatCode << 1) | GetNextBit();

			_noOfRepeats = (unsigned short int) _repeatCode + _repeatCodePaddingVal;

			for (int j = 0; j < _noOfRepeats; j++)
				alphabetContainer->push_back(_repeatValue);
		}
	}
	
	std::cout << "Finished decompressing alphabet" << std::endl; //test

	return true;
}

void InitializeHuffmanDecoder()
{
	std::cout << "Building dynamic Huffman tree" << std::endl; //test

	//read No. of literal/length codes - HLIT
	for (int i = 0; i < 5; i++)
		noOfLiteralLengthCodes = ((unsigned char)noOfLiteralLengthCodes) | (GetNextBit() << i); //This conforms to puff.c's approach
	noOfLiteralLengthCodes += 257;

	//read No. of Distance Codes - HDIST
	for (int i = 0; i < 5; i++)
		noOfDistanceCodes = ((unsigned char)noOfDistanceCodes ) | (GetNextBit() << i);
	noOfDistanceCodes += 1;

	//read No. of Code Length codes - HCLEN
	for (int i = 0; i < 4; i++)
		noOfCodeLengthCodes = ((unsigned char)noOfCodeLengthCodes) | (GetNextBit() << i);
	noOfCodeLengthCodes += 4;

	//Zero out the huffmanCodeLenghts (in case this is the second call of this function)
	for (int i = 0; i < 19; i++)
		huffmanCodeLengths[i] = 0;

	for (int i = 0; i < noOfCodeLengthCodes; i++)
	{
		unsigned short int _count = 0;
		for (int j = 0; j < 3; j++)
			_count = ((unsigned char)_count | (GetNextBit() << j));
		
		std::cout << _count << "\t";
		if (i < 19)
			//huffmanCodeLengths[i] = _count;
			SetSortedHuffmanCodeLengthCode(i, _count);
		else
			std::cout << "ERROR! Number of Code Length Codes are more than 19." << std::endl;
	}

	//The part bellow is somewhat redundant. Decompressing the Huffman tree is done into std::vectors, but the actuall end result is mean to be a basic arrays (std::unique_ptr).
	//TODO remove this redundancy by either using the vectors as an end container, or estimate size and allocate the pointers beforehand.

	//extract literl/lengths and distance alphabets.
	std::vector<unsigned short int> _literalLengthAlphabet;
	std::vector<unsigned short int> _distanceAlphabet;
	//BuildFirstHuffmanTree();
	BuildHuffmanTree(huffmanCodeLengths, 18, 18, &codeLengthHuffmanTree);

	DecompressHuffmanAlphabet(&_literalLengthAlphabet, noOfLiteralLengthCodes);
	DecompressHuffmanAlphabet(&_distanceAlphabet, noOfDistanceCodes);

	//initialize and copy to our main alphabet containers.
	literalLengthsAlphabet = std::unique_ptr<unsigned short int>(new unsigned short int[_literalLengthAlphabet.size()]);
	distanceAlphabet = std::unique_ptr<unsigned short int>(new unsigned short int[_distanceAlphabet.size()]);
	
	for (int i = 0; i < _literalLengthAlphabet.size(); i++)
		literalLengthsAlphabet.get()[i] = _literalLengthAlphabet[i];
	
	for (int i = 0; i < _distanceAlphabet.size(); i++)
		distanceAlphabet.get()[i] = _distanceAlphabet[i];

	BuildHuffmanTree(literalLengthsAlphabet.get(), noOfLiteralLengthCodes, 286, &litLengthHuffmanTree);
	BuildHuffmanTree(distanceAlphabet.get(), noOfDistanceCodes, 30, &distHuffmanTree);


	//test
	std::cout << "noOfLiteralLengthCodes: " << noOfLiteralLengthCodes << std::endl;
	std::cout << "noOfDistanceCodes: " << noOfDistanceCodes << std::endl;
	std::cout << "noOfCodeLengthCodes: " << noOfCodeLengthCodes << std::endl;
	std::cout << "---------------------------------------------------------";
	std::cout << "Literal/Length codes: No: " << _literalLengthAlphabet.size() << std::endl;
	/*for (std::vector<unsigned short int>::iterator it = _literalLengthAlphabet.begin(); it < _literalLengthAlphabet.end(); ++it)
		std::cout << (unsigned short int)*it << "\t";
	std::cout << std::endl;*/
	std::cout << "---------------------------------------------------------";
	std::cout << "Distance codes: No: " << _distanceAlphabet.size() << std::endl;
	/*for (std::vector<unsigned short int>::iterator it = _distanceAlphabet.begin(); it < _distanceAlphabet.end(); ++it)
		std::cout << (unsigned short int)*it << "\t";
	std::cout << std::endl;*/
}

void DecodeDynamicHuffmanBlock(std::vector<unsigned char> * uncompressedRawData)
{
	//first, build the Huffman tree.
	InitializeHuffmanDecoder();

	//at this point, the stream should be at the begining of the compressed data.
	while (!stream.eof())
	{
		//unsigned short int _data;
		char _data; //TODO test this (Previous failed experiment were using the commented out test above, I've changed this while cleaning compiler warnings, but I haven't tested it yet.
		_data = DecodeNextValue(litLengthHuffmanTree.get());

		if (_data < 256) //literal.
		{
			//std::cout << "Found a Literal: " << _data << std::endl; //test
			uncompressedRawData->push_back(_data);
		}
		else if (_data >= 257 && _data <= 285)
		{
			std::cout << "Found Length: " << _data << std::endl; //test

			unsigned long int _length, _extraBits = 0x00, _lengthOffset;
			unsigned short int _noOfExtraBits;

			//translate the current _data value to length
			if (_data <= 264) //not extra bites
			{
				_noOfExtraBits = 0;
				_lengthOffset = 254;
			}
			else if (_data >= 256 && _data <= 268)
			{
				_noOfExtraBits = 1;
				_lengthOffset = 251 + (268 - _data) * (pow(2, _noOfExtraBits) - 1);
			}
			else if (_data >= 269 && _data <= 272)
			{
				_noOfExtraBits = 2;
				_lengthOffset = 241 + (272 - _data)* (pow(2, _noOfExtraBits) - 1);
			}
			else if (_data >= 273 && _data <= 276)
			{
				_noOfExtraBits = 3;
				_lengthOffset = 217 + (276 - _data)* (pow(2, _noOfExtraBits) - 1);
			}
			else if (_data >= 277 && _data <= 280)
			{
				_noOfExtraBits = 4;
				_lengthOffset = 165 + (280 - _data)* (pow(2, _noOfExtraBits) - 1);
			}
			else if (_data >= 281 && _data <= 284)
			{
				_noOfExtraBits = 5;
				_lengthOffset = 57 + (284 - _data)* (pow(2, _noOfExtraBits) - 1);
			}
			else if (_data == 258)
			{
				_noOfExtraBits = 0;
				_data = 512; //hack, so that the equation bellow yields 258.
				_lengthOffset = 254;
			}

			//read _extraBits;
			for (int i = 0; i < _noOfExtraBits; i++)
				_extraBits = ((unsigned char)_extraBits << 1) | GetNextBit();

			//compute length
			_length = (_data - _lengthOffset) | +_extraBits;

			//decode distance value
			_data = DecodeNextValue(distHuffmanTree.get());
			if (_data >= 0 && _data <= 3)
			{
				_noOfExtraBits = 0;
				_lengthOffset = 1;
			}
			else if (_data >= 4 && _data <= 29)
			{
				_noOfExtraBits = floor((_data - 2) / 2);
				_lengthOffset = pow(2, _data - _noOfExtraBits - 1) + 1 - (_data % 2) * pow(2, _noOfExtraBits);
			}
			else
				std::cout << "ERROR! Decoded an unexpected distance value" << std::endl;

			//read _extraBits;
			_extraBits = 0x00;
			for (int i = 0; i < _noOfExtraBits; i++)
				_extraBits = ((unsigned char)_extraBits << 1) | GetNextBit();

			unsigned short int _dist = _lengthOffset + _extraBits;

			if (_dist + _length > uncompressedRawData->size())
				std::cout << "ERROR! Attempting to to seek outside the decompressed data." << std::endl;
			else
			{
				std::unique_ptr<unsigned char> _tempBuffer = std::unique_ptr<unsigned char>(new unsigned char[_length]);
				for (int i = 0; i < _length; i++)
					_tempBuffer.get()[i] = (*uncompressedRawData)[uncompressedRawData->size() - _dist - 1 + i];

				for (int i = 0; i < _length; i++)
					uncompressedRawData->push_back(_tempBuffer.get()[i]);
			}

		}
		else if (_data == 256) //end of compressed data
		{
			std::cout << "Found end of compressed data. " << std::endl; //test
			break;
		}
		else
			std::cout << "ERROR! Decoded an unexpected literal value." << std::endl;

		//test
		if (stream.eof())
			std::cout << "ERROR! Reached end of file before reaching end of compressed data stream." << std::endl;
	}
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
		//read out the block header (read entire byte then extract 3-top bits.
		
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
		case DeflateBlockType::noCompression:
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

		case DeflateBlockType::fixedHuffman:
		{
			std::cout << "Found fixed-Huffman block." << std::endl;//test
		}
			break;

		case DeflateBlockType::dynamicHuffman:
		{
			std::cout << "Found dynamic-Huffman block." << std::endl;//test
			DecodeDynamicHuffmanBlock(&uncompressedRawData);
		}
			break;

		case DeflateBlockType::unused:
			std::cout << "ERROR! Block compression type is set to reserved value. This is against spec." << std::endl;
			return;
			break;

		default:
			break;
		}
	}

	std::unique_ptr<unsigned char> _uncompressedRawData = std::unique_ptr<unsigned char>(new unsigned char[uncompressedRawData.size()]);
	for (int i = 0; i < uncompressedRawData.size(); i++)
		_uncompressedRawData.get()[i] = uncompressedRawData[i];

	ParseDecompressedDataFromMemory(stripOrTileID, _bitMap, _uncompressedRawData.get(), tiffDetails.noOfPixelsPerTileStrip, 0);
	std::cout << "Finished decompression." << std::endl;
}
#pragma endregion
//=====================================================================================================================================================================
//-----------------------------------------PackBits
//=====================================================================================================================================================================
#pragma region PackBits Decompression
void ParsePackBitsStripOrTileData(int stripOrTileID, Array2D * const _bitMap)
{
	//std::cout << "Attempted to decompress PackBits data. Strip " << stripOrTileID << " of " << tiffDetails.noOfTilesOrStrips << std::endl; //test
	stream.seekg(tiffDetails.tileStripOffset.get()[stripOrTileID]);

	//estimate number of bytes we excpect to have
	//I couldn't figure out the correct way to estimate the uncompressed byte count (the references aren't helpful), the loops bellow always overshoots the expected count. So I use 
	//the logical estimate (noOfBytesInStrip), which is the maximum byte count the ParseDecompressedDataFromMemory() function will make use for anyway, and have a seperate buffer
	//called noOfBytes, now set to 2x the logical est. and will be used to store the decompressed bytes. This is memory inefficient, but it works...

	unsigned long int noOfBytesInStrip = tiffDetails.rowsPerStrip * (tiffDetails.width + 7) * tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8;
	//unsigned long int noOfBytes = tiffDetails.noOfPixelsPerTileStrip * tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8;
	//unsigned long int noOfBytes = tiffDetails.rowsPerStrip * (tiffDetails.width + 7) * tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8;
	unsigned long int noOfBytes =  2 * noOfBytesInStrip;
	//std::cout << "noOfBytes: " << noOfBytes << std::endl; //test
	//std::cout << "noOfBytesInStrip: " << noOfBytesInStrip << std::endl; //test

	unsigned long int counter = 0; //counter is used to track of how many bytes we've extracted.
	std::unique_ptr<unsigned char> uncompressedRawData = std::unique_ptr<unsigned char>(new unsigned char[noOfBytes]);
	
	

	char _header; //Used only for the header. The bytes are read as signed characters, so we don't use our BytesToInt8() function as it casts the bytes as unsigned chars.

	while (counter < noOfBytesInStrip)
	{
		stream.read(&_header, sizeof(_header)); //read header and store in the direct use buffer without converting using BytesToInt8().
		
		if (_header >= 0 && _header <= 127)//read n+1 bytes.
		{
			//Three approaches were tried here: 1: Have a loop that reads a single byte from file then assigns to to uncompressedRawData (slowest)
			//2: Read a length of _header+1 from file, load it to memory, and have a loop copying from that memory to uncompressedRawData (faster)
			//3: Read directly from file to uncompressedRawData (fastest, yet).

			stream.read((char*)&uncompressedRawData.get()[counter], (unsigned int)_header + 1);
			counter += _header+1;
		}
		else if (_header <= -1 && _header >= -127) //read one byte, repeast 1 - n times.
		{
			unsigned char _byte;
			stream.read((char*)&_byte, sizeof(_byte));
			for (int i = 0; i < 1 - _header; i++)
				uncompressedRawData.get()[counter + i] = _byte;
				
			counter += 1 - _header;
		}
		else if (_header > 127 || _header < -127)
		{
			std::cout << "ERROR! Found an out-of-spec Packbits header." << std::endl;
		}
		else
		{
			//std::cout << "Packbits - NoOp." << std::endl;
		}
	}
	
	//std::cout << "counter: " << counter << std::endl;//test

	ParseDecompressedDataFromMemory(stripOrTileID, _bitMap, uncompressedRawData.get(), tiffDetails.noOfPixelsPerTileStrip, 0);

	//std::cout << "Finished decompression." << std::endl;
}
#pragma endregion