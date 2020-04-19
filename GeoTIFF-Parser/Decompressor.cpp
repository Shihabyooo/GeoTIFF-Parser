#include "Decompressor.h"

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