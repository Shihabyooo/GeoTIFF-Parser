#pragma once
#include <fstream>

#include "Array2D.h"


//Structs and enums
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


//Variables
extern bool isBigEndian;
extern std::ifstream stream;
extern TIFFDetails tiffDetails;
extern Array2D * bitMap;


//Functions
long int BytesToInt32(const char bytes[4]);

int BytesToInt16(const char bytes[2]);

short int BytesToInt8(const char bytes[1]);

long int BytesToIntX(const char bytes[4], int intSize);