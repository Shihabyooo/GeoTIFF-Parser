#pragma once
#include <fstream>

#include "Array2D.h"


//Structs and enums
enum BitmapFormat
{
	strips, tiles, undefined
};

enum RasterToModelTransformationMethod
{
	tieAndScale, matrix
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

	int noOfIFDs;

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

struct GeoTIFFDetails
{
public:
	short int rasterSpace; //0: undefined, 1: PixelIsArea, 2:PixelIsPoint, 32767: User Defined.
	short int modelType; //0: Undefined or Unknown, 1: 2D Projected CRS, 2: Geographic 2D CRS, 3: Cartesian 3D CRS, 32767: User Defined.
	RasterToModelTransformationMethod transformationMethod;
	double tiePoints[3][2]; //Only used if transformationMethod is tieAndScale.
	double pixelScale[3]; //XYZ scale. For XY, positive scale indicate increase in XY coord as raster space UV increase, negatives denote an inverse relation. Only used if transformationMethod is tieAndScale.

	short int projectedCRS; //Ranges 1-1023 reserved, 1024-32766 EPSG Projected CRS Codes, 32767 is User Defined, 32768-65535 are private.
	short int goedeticCRS; //Ranges 1-1023 reserved, 1024-32766 EPSG Geographic 2D or Geocentric CRS, 32767 User Defined, 32768-65535 private.
	short int verticalCRS; //Ranges 1-1023 reserved, 1024-32766 EPSG Geographic 2D or Geocentric CRS, 32767 User Defined, 32768-65535 private.

	std::string geotiffCitation = "";
	std::string geodeticCRSCitation = "";
	std::string projectedCRSCitation = "";
	std::string verticalCRSCitation = "";


	//TODO add parts relevant to User Defined CRSs (Section 7.5 of standard)
};

struct Tag
{
	unsigned short int tagID;
	unsigned short int fieldTypeID;
	unsigned long int count;
	unsigned long int offsetValue;
};

struct GeoKey
{
	unsigned short int keyID;
	unsigned short int tiffTagLocation; //if ==0, then offsetVale contains the value (of type short int) and count is implied to be = 1.
	unsigned short int count;
	unsigned short int offsetValue;
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
extern GeoTIFFDetails geoDetails;
extern Array2D * bitMap;


//Functions
long int BytesToInt32(const char bytes[4]);

int BytesToInt16(const char bytes[2]);

short int BytesToInt8(const char bytes[1]);

long int BytesToIntX(const char bytes[4], int intSize);