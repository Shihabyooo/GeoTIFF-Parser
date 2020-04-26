#include "GeoTIFF_Parser.h"

//variables
bool viewTagsInCLI = false;
unsigned long int firstIFDOffset;
Array2D * bitMap = NULL; //the actual holder of the geoTIFF raster's data.

std::vector<GeoKey> intParamsGeoKeys;
std::vector<GeoKey> doubleParamsGeoKeys;
std::vector<GeoKey> asciiParamsGeoKeys;


//functions
//TODO replace all function definitions before LoadGeoTIFF() with forward declarations (except those declared in header), move definitions to the end (and sort according to use).
//TODO replace GetFieldDescription() and GetGeoKeyDescription with fixed 2D array (or any container of pairs with fast random access) hardcoded into a header file, and have those function only retrieve from those containers.
std::string GetFieldDescription(unsigned short int tagID)
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
		//GeoTIFF tags
	case (34735):
		desc = "GeoKeyDirectoryTag";
		break;
	case (34736):
		desc = "GeoDoubleParamsTag";
		break;
	case (34737):
		desc = "GeoAsciiParamsTag";
		break;
	case (33550):
		desc = "ModelPixelScaleTag";
		break;
	case (33922):
		desc = "ModelTiepointTag";
		break;
	case (34264):
		desc = "ModelTransformationTag";
		break;

		//Additional GeoTIFF-related Tags"
	case (42113):
		desc = "GDAL_NODATA";
		break;
		//Additional Tags:
	case (339):
		desc = "SampleFormat";
		break;
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

std::string GetGeoKeyDescription(unsigned short int keyID)
{
	std::string desc;

	switch (keyID)
	{
	case (1025):
		desc = "GTRasterTypeGeoKey";
		break;
	case (1024):
		desc = "GTModelTypeGeoKey";
		break;
	case (3072):
		desc = "ProjectedCRSGeoKey";
		break;
	case (2048):
		desc = "GeodeticCRSGeoKey";
		break;
	case (4096):
		desc = "VerticalGeoKey";
		break;
	case(1026):
		desc = "GTCitationGeoKey";
		break;
	case(2049):
		desc = "GeodeticCitationGeoKey";
		break;
	case(3073):
		desc = "ProjectedCitationGeoKey";
		break;
	case(4097):
		desc = "VerticalCitationGeoKey";
		break;
	case(2054):
		desc = "GeogAngularUnitsGeoKey";
		break;
	case(2060):
		desc = "GeogAzimuthUnitsGeoKey";
		break;
	case(2052):
		desc = "GeogLinearUnitsGeoKey";
		break;
	case(3076):
		desc = "ProjLinearUnitsGeoKey";
		break;
	case(4099):
		desc = "VerticalUnitsGeoKey";
		break;
	case(2057):
		desc = "EllipsoidSemiMajorAxisGeoKey";
		break;
	case(2055):
		desc = "GeoAngularUnitSizeGeoKey";
		break;
	case(2053):
		desc = "GeogLinearUnitSizeGeoKey";
		break;
	case(3077):
		desc = "ProjLinearUnitSizeGeoKey";
		break;
	case(2050):
		desc = "GeodeticDatumGeoKey";
		break;
	case(2051):
		desc = "PrimeMeridianGeoKey";
		break;
	case(2061):
		desc = "PrimeMeridianLongitudeGeoKey";
		break;
	case(2056):
		desc = "EllipsoidGeoKey";
		break;
	case(2058):
		desc = "EllipsoidSemiMinorAxisGeoKey";
		break;
	case(2059):
		desc = "EllipsoidInvFlatteningGeoKey";
		break;
	case(4098):
		desc = "VerticalDatumGeoKey";
		break;
	case(3074):
		desc = "ProjectionGeoKey";
		break;
	case(3075):
		desc = "ProjMethodGeoKey";
		break;
	case(3078):
		desc = "ProjStdParallel1GeoKey";
		break;
	case(3079):
		desc = "ProjStdParallel2GeoKey";
		break;
	case(3080):
		desc = "ProjNatOriginLongGeoKey";
		break;
	case(3081):
		desc = "ProjNatOriginLatGeoKey";
		break;
	case(3084):
		desc = "ProjFalseOriginLongGeoKey";
		break;
	case(3085):
		desc = "ProjFalseOriginLatGeoKey";
		break;
	case(3088):
		desc = "ProjCenterLongGeoKey";
		break;
	case(3089):
		desc = "ProjCenterLatGeoKey";
		break;
	case(3095):
		desc = "ProjStraightVertPoleLongGeoKey";
		break;
	case(3094):
		desc = "ProjAzimuthAngleGeoKey";
		break;
	case(3082):
		desc = "ProjFalseEastingGeoKey";
		break;
	case(3083):
		desc = "ProjFalseNorthingGeoKey";
		break;
	case(3086):
		desc = "ProjFalseOriginEastingGeoKey";
		break;
	case(3087):
		desc = "ProjFalseOriginNorthingGeoKey";
		break;
	case(3090):
		desc = "ProjCenterEastingGeoKey";
		break;
	case(3091):
		desc = "ProjCenterNorthingGeoKey";
		break;
	case(3092):
		desc = "ProjScaleAtNatOriginGeoKey";
		break;
	case(3093):
		desc = "ProjScaleAtCenterGeoKey";
		break;
	default:
		desc = "Unknown or Undefined GeoKey";
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

long int GetFieldIntData(const Tag * tag)
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

void GetFieldIntArrayData(const Tag * tag, long int * outputArray)
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

short int GetGeoKeyIntData(const GeoKey * geoKey, short int * dataArray, int valueOrderInKey = 0)
{
	short int result;
	if (dataArray == NULL || geoKey->tiffTagLocation == 0) //This assumes that this function is called only when value is set in a key's valueOffset field. This assumption also includes the GeoTIFF spec that values stored in key offsetValue
	{						//are of type short (not used here, but can cause issues) and that the count = 1 (so we return a single value)
		result = geoKey->offsetValue;
	}
	else
	{
		result = dataArray[geoKey->offsetValue + valueOrderInKey];
	}

	return result;
}

double GetGeoKeyDoubleData(const GeoKey * geoKey, double * dataArray, int valueOrderInKey = 0)
{
	short int result;
	if (dataArray == NULL || geoKey->tiffTagLocation == 0) //This assumes that this function is called only when value is set in a key's valueOffset field. This assumption also includes the GeoTIFF spec that values stored in key offsetValue
	{						//are of type short (not used here, but can cause issues) and that the count = 1 (so we return a single value)
		result = geoKey->offsetValue;
	}
	else
	{
		result = dataArray[geoKey->offsetValue + valueOrderInKey];
	}

	return result;
}

std::string ExtractAndMergeMultiASCIIValues(const GeoKey * geoKey, char * dataArray)
{
	std::string result = "";

	for (int i = geoKey->offsetValue; i < geoKey->offsetValue + geoKey->count; i++)
		result += dataArray[i];

	result += '\0';

	return result;
}

template <typename T>
void ProcessGeoKey(const GeoKey * geoKey, T * dataArray = NULL)
{
	switch (geoKey->keyID)
	{
	case(1024):  //GTModelTypeGeoKey
		geoDetails.modelType = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(1025): //GTRasterTypeGeoKey
		geoDetails.rasterSpace = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(2048): //GeodeticCRSGeoKey
		geoDetails.geodeticCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(3072): //GeodeticCRSGeoKey
		geoDetails.projectedCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(4096): //VerticalCRSGeoKey
		geoDetails.verticalCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;

	case(1026): //GTCitationGeoKey - ASCII
		break;
	case(2049): //GeodeticCitationGeoKey - ASCII
		geoDetails.geodeticCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*)dataArray);
		break;
	case(3073): //ProjectedCitationGeoKey  - ASCII
		geoDetails.projectedCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*)dataArray);
		break;
	case(4097): //VerticalCitationGeoKey  - ASCII
		geoDetails.verticalCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*) dataArray);
		break;

	case(2057): //EllipsoidSemiMajorAxisGeoKey - Double
		geoDetails.ellipsoidSemiMajorAxis = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;
	case(2058): //EllipsoidSemiMajorAxisGeoKey - Double
		geoDetails.ellipsoidSemiMinorAxis = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;
	case(2059): //EllipsoidInvFlatteningGeoKey - Double
		geoDetails.ellipsoidInvFlattening = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;

	default:
		break;
	}
}

void ProcessGeoKeyDirectory(const Tag * geoKeyDirectoryTag)
{
	std::cout << "Processing geoKeys." << std::endl;
	stream.seekg(geoKeyDirectoryTag->offsetValue, stream.beg);

	char word[2];
	unsigned short int keyDirectoryVersion, keyRevision, minorRevision, numberOfKeys;

	stream.read(word, sizeof(word));
	keyDirectoryVersion = BytesToInt16(word);
	stream.read(word, sizeof(word));
	keyRevision = BytesToInt16(word);
	stream.read(word, sizeof(word));
	minorRevision = BytesToInt16(word);
	stream.read(word, sizeof(word));
	numberOfKeys = BytesToInt16(word);

	std::cout << "Key Directory Version: " << keyDirectoryVersion << std::endl;
	std::cout << "Key Revision: " << keyRevision << std::endl;
	std::cout << "Minor Revision: " << minorRevision << std::endl;
	std::cout << "Number of Keys: " << numberOfKeys << std::endl;

	//TODO add a check here to make sure versions match the ones adopted in this code. Else stop execution of remainin of program (a universal bool and int for error code that LoadTIFF() checks after processing tags? Throw Exception?)

	for (int i = 0; i < numberOfKeys; i++)
	{
		std::unique_ptr<GeoKey> geoKey = std::unique_ptr<GeoKey>(new GeoKey);

		//-----------------------------------------
		stream.read(word, sizeof(word));
		geoKey.get()->keyID = BytesToInt16(word);
		//-----------------------------------------
		stream.read(word, sizeof(word));
		geoKey.get()->tiffTagLocation = BytesToInt16(word);
		//-----------------------------------------
		stream.read(word, sizeof(word));
		geoKey.get()->count = BytesToInt32(word);
		//-----------------------------------------
		stream.read(word, sizeof(word));
		geoKey.get()->offsetValue = BytesToInt32(word);


		if (viewTagsInCLI)
		{
			std::cout << "-----------------------------------------------------" << std::endl;
			std::cout << "Tag loop: " << i << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Field identifying tag: " << geoKey.get()->keyID << " -- ";
			GetGeoKeyDescription(geoKey.get()->keyID);
			std::cout << "Current file loc: " << stream.tellg() << "\t" <<"TIFF Tag Location: " << geoKey.get()->tiffTagLocation << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Count: " << geoKey.get()->count << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Value\\offset: " << geoKey.get()->offsetValue << std::endl;
		}

		if (geoKey->tiffTagLocation == geoKeyDirectoryTag->tagLocationOnFile) //means data are stored at end of GeoKeyDirectory (as short ints array).
		{
			GeoKey _key = { geoKey.get()->keyID, geoKey.get()->tiffTagLocation, geoKey.get()->count, geoKey.get()->offsetValue };
			intParamsGeoKeys.push_back(_key);
		}
		else if (geoKey->tiffTagLocation == 34736) //means data are stored in GeoDoubleParamsTag
		{
			GeoKey _key = {geoKey.get()->keyID, geoKey.get()->tiffTagLocation, geoKey.get()->count, geoKey.get()->offsetValue };
			doubleParamsGeoKeys.push_back(_key);
		}
		else if (geoKey->tiffTagLocation == 34737) //means data are stored in GeoAsciiParamsTag
		{
			GeoKey _key = { geoKey.get()->keyID, geoKey.get()->tiffTagLocation, geoKey.get()->count, geoKey.get()->offsetValue };
			asciiParamsGeoKeys.push_back(_key);
		}
		else if (geoKey->tiffTagLocation == 0) //These keys have their data inside their own valueOffset bytes.
		{
			short int * _nullPtr = NULL; //Because the compiler requires something to figure out typeof(T) with..
			ProcessGeoKey(geoKey.get(), _nullPtr);
		}
		else //shouldn't happen
		{
			std::cout << "WARNING! Found GeoKeys that can't be parsed according to GeoTIFF spec." << std::endl;
		}
	}

	/*std::cout << "No of Int Data After GeoKeyDirectory: " << intParamsGeoKeys.size() << std::endl;
	std::cout << "No of keys storing data in GeoDoubleParamsTag: " << doubleParamsGeoKeys.size() << std::endl;
	std::cout << "No of keys storing data in GeoAsciiParamsTag: " << asciiParamsGeoKeys.size() << std::endl;*/

	if (intParamsGeoKeys.size() > 0) //this means there are data (short int) after the end of this tag (GeoKeyDirectory), we have to parse them and assign them to keys stored in intParamsGeoKeys.
	{
		std::cout << "DEVWARNING! Current implementation lacks support for tags storing short values outside its valueOffset field" << std::endl;
		//TODO finish implementing this!

		for (std::vector<GeoKey>::iterator it = intParamsGeoKeys.begin(); it < intParamsGeoKeys.end(); ++it)
		{
			
		}
	}

}

void ProcessTag(const Tag * tag)
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
	case (34735): //GeoKeyDirectory
		ProcessGeoKeyDirectory(tag);
		break;
	case (34736): //DoubleParamsGeoKey
		if (doubleParamsGeoKeys.size() > 0)
		{
			//extract the params stored in this tag.
			std::unique_ptr<double> doubleParamsData = std::unique_ptr<double>(new double[tag->count]);
			double buffer;

			stream.seekg(tag->offsetValue);
			for (int i = 0; i < tag->count; i++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				doubleParamsData.get()[i] = buffer;
			}

			//Loop over keys stored in doubleParamsGeoKeys and process them.
			for (std::vector<GeoKey>::iterator it = doubleParamsGeoKeys.begin(); it < doubleParamsGeoKeys.end(); ++it)
			{
				ProcessGeoKey(&(*it), doubleParamsData.get());
			}
		}
		break;
	case (34737): //ASCIIParamsGeoKey
		if (asciiParamsGeoKeys.size() > 0)
		{
			//extract the params stored in this tag.
			std::unique_ptr<char> asciiParamsData = std::unique_ptr<char>(new char[tag->count]);

			stream.seekg(tag->offsetValue);

			char buffer = ' ';
			for (int i = 0; i < tag->count; i++)
			{
				stream.read(&buffer, sizeof(buffer));
				asciiParamsData.get()[i] = buffer;
			}

			//Loop over keys stored in asciiParamsGeoKeys and process them.
			for (std::vector<GeoKey>::iterator it = asciiParamsGeoKeys.begin(); it < asciiParamsGeoKeys.end(); ++it)
				ProcessGeoKey(&(*it), asciiParamsData.get());
		}
		break;
	case (33550): //ModelPixelScaleTag
	{
		if (geoDetails.transformationMethod == RasterToModelTransformationMethod::matrix)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelPixelScaleTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelPixelScaleTag" << std::endl;
			break;
		}
		else
			geoDetails.transformationMethod = RasterToModelTransformationMethod::tieAndScale;

		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 3; i++)
		{
			stream.read((char*)&buffer, sizeof(buffer));
			geoDetails.pixelScale[i] = buffer;
		}
	}
		break;
	case (33922): //ModelTiepointTag
	{
		if (geoDetails.transformationMethod == RasterToModelTransformationMethod::matrix)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelTiepointTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelTiepointTag" << std::endl;
			break;
		}
		else
			geoDetails.transformationMethod = RasterToModelTransformationMethod::tieAndScale;

		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				geoDetails.tiePoints[i][j] = buffer;
			}
		}
	}
		break;
	case (34264): //ModelTransformationTag
	{
		if (geoDetails.transformationMethod == RasterToModelTransformationMethod::tieAndScale)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelTiepointTag or ModePixelScaleTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelTransformationTag" << std::endl;
			break;
		}
		else
			geoDetails.transformationMethod = RasterToModelTransformationMethod::matrix;


		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				geoDetails.modelTransformationMatrix[i][j] = buffer;
			}
		}
	}
		break;
	default:
		break;
	}
}

bool ParseStripOrTileData(int stripOrTileID)
{
	switch (tiffDetails.compression)
	{
	case (1): //no compression
		ParseUncompressedStripOrTileData(stripOrTileID, bitMap);
		break;

	case (32773): //PackBits (Macintosh RLE)
		ParsePackBitsStripOrTileData(stripOrTileID, bitMap);
		break;

	case (2): //CCITT modified Huffman RLE
		std::cout << "ERROR! Unsupported compression algorithm - Modified Huffman RLE" << std::endl;
		return false;
		break;

	case (3): //CCITT Group 3 fax encoding
		std::cout << "ERROR! Unsupported compression algorithm - Group 3 Fax Encoding" << std::endl;
		return false;
		break;

	case (4): //CCITT Group 4 fax encoding
		std::cout << "ERROR! Unsupported compression algorithm - Group 4 Fax Encoding" << std::endl;
		return false;
		break;

	case (5): //LZW
		std::cout << "ERROR! Unsupported compression algorithm - LZW" << std::endl;
		return false;
		break;

	case (6): //JPEG (old style, deprecated?)
		std::cout << "ERROR! Unsupported compression algorithm - JPEG(Deprecated)" << std::endl;
		return false;
		break;

	case (7): //JPEG (new style)
		std::cout << "ERROR! Unsupported compression algorithm - JPEG" << std::endl;
		return false;
		break;

	case (8): //Deflate
		ParseDeflateStripOrTileData(stripOrTileID, bitMap);
		break;

	case (9): //"Defined by TIFF-F and TIFF-FX standard (RFC 2301) as ITU-T Rec. T.82 coding, using ITU-T Rec. T.85 (which boils down to JBIG on black and white). "
				//https://www.awaresystems.be/imaging/tiff/tifftags/compression.html
		std::cout << "ERROR! Unsupported compression algorithm - RFC 2301 - ITU-T Rec. T.85" << std::endl;
		return false;
		break;

	case (10): //"Defined by TIFF-F and TIFF-FX standard (RFC 2301) as ITU-T Rec. T.82 coding, using ITU-T Rec. T.43 (which boils down to JBIG on color). "
		std::cout << "ERROR! Unsupported compression algorithm - RFC 2301 - ITU-T Rec. T.43" << std::endl;
		return false;
		break;

	default:
		std::cout << "ERROR! Compression algorithm could not be determined." << std::endl;
		return false;
		break;
	}
	return true;
}

void DisplayTIFFDetailsOnCLI()
{
	std::cout << "=======================================================================================" << std::endl;
	std::cout << "\t\tTIFF Details" << std::endl;
	std::cout << "=======================================================================================" << std::endl;

	std::cout << "Dimensions :" << tiffDetails.width << " x " << tiffDetails.height << std::endl;
	std::cout << "Samples Per Pixel: " << tiffDetails.samplesPerPixel << std::endl;
	std::cout << "Bits Per Sample: " << tiffDetails.bitsPerSample << std::endl;
	std::cout << "Extra Samples Type: " << tiffDetails.extraSampleType << std::endl;
	std::cout << "Sample Format: " << tiffDetails.sampleFormat << std::endl;
	std::cout << "Compression: " << tiffDetails.compression << std::endl;

	std::cout << "Photmetric Interpretation: " << tiffDetails.photometricInterpretation << std::endl;
	std::cout << "Planar Configuration: " << tiffDetails.planarConfiguration << std::endl;



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

	std::cout << "=======================================================================================" << std::endl;
	std::cout << "=======================================================================================" << std::endl;
}

void DisplayGeoTIFFDetailsOnCLI()
{
	std::cout << "=======================================================================================" << std::endl;
	std::cout << "\t\tGeoTIFF Details" << std::endl;
	std::cout << "=======================================================================================" << std::endl;

	std::cout << "Raster Space: " << geoDetails.rasterSpace << std::endl;
	std::cout << "Model Type: " << geoDetails.modelType << std::endl;
	std::cout << "Transformation Method: " << (geoDetails.transformationMethod == RasterToModelTransformationMethod::matrix? "Matrix" : "Tie and Scale") << std::endl;


	if (geoDetails.transformationMethod == RasterToModelTransformationMethod::tieAndScale)
	{
		std::cout << "Tie Points" << std::endl;
		for (int i = 0; i < 2; i++)
			std::cout << "\t" << geoDetails.tiePoints[i][0] << ",  " << geoDetails.tiePoints[i][1] << ",  " << geoDetails.tiePoints[i][2] << std::endl;
		std::cout << "Pixel Scale: " << geoDetails.pixelScale[0] << " x " << geoDetails.pixelScale[1] << " x " << geoDetails.pixelScale[2] << std::endl;
	}
	else if (geoDetails.transformationMethod == RasterToModelTransformationMethod::matrix)
	{
		std::cout << "Raster to Mode Transformation Matrix:" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				std::cout << geoDetails.modelTransformationMatrix[i][j] << "\t";
			
			std::cout << std::endl;
		}
	}

	std::cout << "Projected CRS: " << geoDetails.projectedCRS << std::endl;
	std::cout << "Geodetic CRS: " << geoDetails.geodeticCRS << std::endl;
	std::cout << "Vertical CRS: " << geoDetails.verticalCRS << std::endl;

	std::cout << "GeoTIFF Citation: " << geoDetails.geotiffCitation.c_str() << std::endl;
	std::cout << "Geodetic CRS Citation: " << geoDetails.geodeticCRSCitation.c_str() << std::endl;
	std::cout << "Projected CRS Citation: " << geoDetails.projectedCRSCitation.c_str() << std::endl;
	std::cout << "Vertical CRS Citation: " << geoDetails.verticalCRSCitation.c_str() << std::endl;

	std::cout << "Ellipsoid: " << geoDetails.ellipsoid << std::endl;
	std::cout << "Ellispod Semi Major Axis: " << geoDetails.ellipsoidSemiMajorAxis << std::endl;
	std::cout << "Ellipsoid Semi Minor Axis: " << geoDetails.ellipsoidSemiMinorAxis << std::endl;
	std::cout << "Ellipsoid Inverse Flattening: " << geoDetails.ellipsoidInvFlattening << std::endl;

	std::cout << "Vertical Datum: " << geoDetails.verticalDatum << std::endl;

	std::cout << "=======================================================================================" << std::endl;
	std::cout << "=======================================================================================" << std::endl;
}

bool OpenTIFFFile(std::string filePath)
{
	stream.open(filePath, std::ios::binary | std::ios::in);

	if (!stream.is_open())
	{
		std::cout << "Could not open file " << filePath.c_str() << std::endl;
		return false;
	}

	return true;
}

bool ParseTIFFHeader()
{
	char byte[1];
	char word[2];
	char dword[4];
	//char qword[8];

	//determine endianness
	//std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	std::cout << "byte order: " << word[0] << word[1] << std::endl;

	switch (word[0])
	{
	case 'I':
		isBigEndian = false;
		//std::cout << "Byte order set to little-endian." << std::endl;
		break;
	case 'M':
		isBigEndian = true;
		//std::cout << "Byte order set to Big-endian." << std::endl;
		break;
	default:
		std::cout << "Could not determine the byte order of the file." << std::endl;
		return false;;
	}

	//check version (should always be 42)
	//std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	//std::cout << "Format version: " << BytesToInt16(word) << std::endl;
	
	if (BytesToInt16(word) != 42)
	{
		std::cout << "ERROR! File header does not indicate a TIFF file. The answer to the universe was not found." << std::endl;
		return false;
	}

	//check offset to first IFD (Image File Directory)
	//std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(dword, sizeof(dword));
	firstIFDOffset = BytesToInt32(dword);
	//std::cout << "First IFD offset: " << firstIFDOffset << std::endl;

	return true;
}

bool ParseFirstIFDHeader()
{
	char byte[1];
	char word[2];
	char dword[4];

	//seek to first IFD begining
	stream.seekg(firstIFDOffset, stream.beg);

	//check IFD header
	//std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	short int noOfTagsInIFD = BytesToInt16(word);
	//std::cout << "Number of IFD enteries: " << noOfTagsInIFD << std::endl;

	unsigned long int endOfLastTag = stream.tellg(); //Because ProcessTag() may modify the position in the stream and is called at end of loop, any further reading of tags wouldn't be correct.
													//So, we cache the position of the tag end before we call ProcessTag()
	for (int i = 0; i < noOfTagsInIFD; i++)
	{
		std::unique_ptr<Tag> tag = std::unique_ptr<Tag>(new Tag);

		stream.seekg(endOfLastTag); //in case the stream position was changed in ProcessTag() bellow, return to the location at the end of the last tag = begining of new tag.
		tag->tagLocationOnFile = stream.tellg();
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
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Field identifying tag: " << tag.get()->tagID << " -- ";
			GetFieldDescription(tag.get()->tagID);
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Field type ID: " << tag.get()->fieldTypeID << " -- " << GetType(tag.get()->fieldTypeID).description.c_str() << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Count: " << tag.get()->count << std::endl;
			std::cout << "Current file loc: " << stream.tellg() << "\t" << "Value\\offset: " << tag.get()->offsetValue << std::endl;
		}

		endOfLastTag = stream.tellg();

		ProcessTag(tag.get());
	}

	//Fill out our last remaining TIFFDetail, the Number of Pixels in each strip/tile.
	tiffDetails.noOfPixelsPerTileStrip = tiffDetails.tileStripByteCount / (tiffDetails.samplesPerPixel * tiffDetails.bitsPerSample / 8);

	if (viewTagsInCLI)
	{
		std::cout << "===================================================================" << std::endl;
		std::cout << "Finished processing tags" << std::endl;
		std::cout << "===================================================================" << std::endl;
	}

	return true;
}

bool AllocateBitmapMemory()
{
	//Allocate our bitmap in memory as an array of Array2D.
	//bitMap = std::unique_ptr<Array2D>(new Array2D[tiffDetails.height]);
	//for (int i = 0; i < tiffDetails.height; i++)
	//{
	//	bitMap.get()[i] = Array2D(tiffDetails.width, tiffDetails.samplesPerPixel);
	//}

	try
	{
		bitMap = new Array2D[tiffDetails.height];
		for (int i = 0; i < tiffDetails.height; i++)
		{
			bitMap[i] = Array2D(tiffDetails.width, tiffDetails.samplesPerPixel);
		}
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "ERROR! Could not allocate memory for the Bitmap." << std::endl;
		std::cout << e.what() << std::endl;
		return false;
	}

	return true;
}

void DeallocateBitmapMemory()
{
	if (bitMap != NULL)
	{
		for (int i = 0; i < tiffDetails.height; i++)
			bitMap[i].~Array2D();
		delete[] bitMap;
		bitMap = NULL;
	}
}

bool ParseFirstBitmap()
{
	if (tiffDetails.planarConfiguration != 1)
	{
		std::cout << "ERROR! This reader cannot parse non-chunky (TIFF6.0 Planar Configuration other than 1) TIFF files." << std::endl;
		return false;
	}
	else if (tiffDetails.bitsPerSample != 8 && tiffDetails.bitsPerSample != 16 && tiffDetails.bitsPerSample != 32)
	{
		std::cout << "ERROR! This reader can only parse 8, 16 and 32 bits-per-samples images." << std::endl;
		return false;
	}
	else
	{
		for (int i = 0; i < tiffDetails.noOfTilesOrStrips; i++)
		{
			std::cout << "Data for strip no " << i << std::endl;
			if (!ParseStripOrTileData(i))
				return false;
		}
	}

	return true;
}

void DisplayBitmapOnCLI()
{
	if (bitMap == NULL)
	{
		std::cout << "ERROR! No Bitmap is loaded to memory." << std::endl;
		return;
	}

	for (int i = 0; i < tiffDetails.height; i++)
	{
		for (int j = 0; j < bitMap[i].Rows(); j++)
		{
			for (int k = 0; k < bitMap[i].Columns(); k++)
			{
				if (k > 0)
					std::cout << ",";
				std::cout << bitMap[i][j][k];
			}
			std::cout << "\t";
		}
		std::cout << "[ROW_ " << i << "_END]" << std::endl << std::endl;
	}
}

bool LoadGeoTIFF(std::string filePath) //Primary entry point
{
	
	if (!OpenTIFFFile(filePath))
		return false;

	if (!ParseTIFFHeader())
		return false;

	if (!ParseFirstIFDHeader()) //Currently, no error checking is done in ParseFirstIFDHeader(), but future work should include some.
		return false;

	DisplayTIFFDetailsOnCLI();
	DisplayGeoTIFFDetailsOnCLI();

	if (!AllocateBitmapMemory())
		return false;

	if (!ParseFirstBitmap())
	{
		UnloadGeoTIFF();
		return false;
	}

	//DisplayBitmapOnCLI();

	stream.close();

	return true;
}

void UnloadGeoTIFF()
{
	DeallocateBitmapMemory();

	intParamsGeoKeys.clear();
	doubleParamsGeoKeys.clear();
	asciiParamsGeoKeys.clear();

	if (stream.is_open())
		stream.close();
	stream.clear();

	tiffDetails = TIFFDetails();
	geoDetails = GeoTIFFDetails();
}