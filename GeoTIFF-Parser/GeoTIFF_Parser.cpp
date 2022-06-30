#include "GeoTIFF_Parser.h"
#include "Decompressor.h"

//variables
bool viewTagsInCLI = true;
unsigned long int firstIFDOffset;
std::vector<Matrix_f64 *> bitMaps;

std::vector<GeoKey> intParamsGeoKeys;
std::vector<GeoKey> doubleParamsGeoKeys;
std::vector<GeoKey> asciiParamsGeoKeys;

//functions
//TODO replace all function definitions before LoadGeoTIFF() with forward declarations (except those declared in header), move definitions to the end (and sort according to use).
//TODO replace GetFieldDescription() and GetGeoKeyDescription with fixed 2D array (or any container of pairs with fast random access) hardcoded into a header file, and have those function only retrieve from those containers.
bool IsLoadedRaster(int rasterID)
{
	return (rasterID >= 0) && rasterID < bitMaps.size();
}

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
	//std::cout << desc.c_str() << std::endl;
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
	//std::cout << desc.c_str() << std::endl;
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
	std::streampos currentFileStreamLocation = stream.tellg();

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
		//std::cout << "Field value size is greater than header value field capacity. These bytes are pointers." << std::endl; //test
		isOffsetData = true;
	}

	std::streampos currentFileStreamLocation = stream.tellg();

	if (isOffsetData)
		stream.seekg(tag->offsetValue);

	if (tag->fieldTypeID == 3) //short
	{
		char buffer[2];
		for (unsigned long int i = 0; i < tag->count; i++) //all arithmatic uses of i bellow are always positive, safe to use unsigned integers.
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
		for (unsigned long int i = 0; i < tag->count; i++)
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
	
	stream.seekg(currentFileStreamLocation);
}

short int GetGeoKeyIntData(const GeoKey * geoKey, const short int * dataArray, int valueOrderInKey = 0)
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

	//std::cout << "GetGeoKeyIntData() returning: " << result << std::endl; //test
	return result;
}

double GetGeoKeyDoubleData(const GeoKey * geoKey, const double * dataArray, int valueOrderInKey = 0)
{
	double result;
	if (dataArray == NULL || geoKey->tiffTagLocation == 0) //This assumes that this function is called only when value is set in a key's valueOffset field. This assumption also includes the GeoTIFF spec that values stored in key offsetValue
	{						//are of type short (not used here, but can cause issues) and that the count = 1 (so we return a single value)
		result = geoKey->offsetValue;
	}
	else
	{
		//std::cout << "Getting a double value of order: " << geoKey->offsetValue + valueOrderInKey << std::endl; //test
		result = dataArray[geoKey->offsetValue + valueOrderInKey];
	}

	return result;
}

std::string ExtractAndMergeMultiASCIIValues(const GeoKey * geoKey, const char * dataArray)
{
	std::string result = "";

	//std::cout << "ExtractAndMergeMultiASCIIValues() recieved a geoKey with offset: " << geoKey->offsetValue << std::endl; //test

	for (unsigned int i = geoKey->offsetValue; i < geoKey->offsetValue + geoKey->count; i++)
	{
		//std::cout << "Getting a ascii value of order: " << i << std::endl; //test
		result += dataArray[i];
	}

	result += '\0';

	return result;
}

template <typename T>
void ProcessGeoKey(int rasterID, const GeoKey * geoKey, const T * dataArray = NULL)
{
	switch (geoKey->keyID)
	{
	case(1024):  //GTModelTypeGeoKey
		geoDetails[rasterID]->modelType = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(1025): //GTRasterTypeGeoKey
		geoDetails[rasterID]->rasterSpace = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(2048): //GeodeticCRSGeoKey
		geoDetails[rasterID]->geodeticCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(3072): //GeodeticCRSGeoKey
		geoDetails[rasterID]->projectedCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;
	case(4096): //VerticalCRSGeoKey
		geoDetails[rasterID]->verticalCRS = GetGeoKeyIntData(geoKey, (short int *)dataArray);
		break;

	case(1026): //GTCitationGeoKey - ASCII
		break;
	case(2049): //GeodeticCitationGeoKey - ASCII
		geoDetails[rasterID]->geodeticCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*)dataArray);
		break;
	case(3073): //ProjectedCitationGeoKey  - ASCII
		geoDetails[rasterID]->projectedCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*)dataArray);
		break;
	case(4097): //VerticalCitationGeoKey  - ASCII
		geoDetails[rasterID]->verticalCRSCitation = ExtractAndMergeMultiASCIIValues(geoKey, (char*) dataArray);
		break;

	case(2057): //EllipsoidSemiMajorAxisGeoKey - Double
		geoDetails[rasterID]->ellipsoidSemiMajorAxis = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;
	case(2058): //EllipsoidSemiMajorAxisGeoKey - Double
		geoDetails[rasterID]->ellipsoidSemiMinorAxis = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;
	case(2059): //EllipsoidInvFlatteningGeoKey - Double
		geoDetails[rasterID]->ellipsoidInvFlattening = GetGeoKeyDoubleData(geoKey, (double *)dataArray);
		break;

	default:
		break;
	}
}

void ProcessGeoKeyDirectory(int rasterID, const Tag * geoKeyDirectoryTag)
{
	std::cout << "Processing geoKeys." << std::endl;
	stream.seekg(geoKeyDirectoryTag->offsetValue, stream.beg);

	char word[2];
	unsigned int keyDirectoryVersion, keyRevision, minorRevision, numberOfKeys;

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

	for (unsigned long int i = 0; i < numberOfKeys; i++)
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
		geoKey.get()->count = BytesToInt16(word);
		//-----------------------------------------
		stream.read(word, sizeof(word));
		geoKey.get()->offsetValue = BytesToInt16(word);


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
			ProcessGeoKey(rasterID, geoKey.get(), _nullPtr);
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

void ProcessTag(int rasterID, const Tag * tag)
{
	switch (tag->tagID)
	{
	case (256): //width
		tiffDetails[rasterID]->width = GetFieldIntData(tag);
		break;
	case (257): //height
		tiffDetails[rasterID]->height = GetFieldIntData(tag);
		break;
	case (258): //bps
		tiffDetails[rasterID]->bitsPerSample = GetFieldIntData(tag);
		break;
	case (259): //compression
		tiffDetails[rasterID]->compression = GetFieldIntData(tag);
		break;
	case (273): //stripoffsets
	{
		//std::cout << "allocating tileStripOffset array of rows: " << tag->count << std::endl; //test
		tiffDetails[rasterID]->tileStripOffset = std::unique_ptr<long int>(new long int[tag->count]);
		tiffDetails[rasterID]->noOfTilesOrStrips = tag->count;
		GetFieldIntArrayData(tag, tiffDetails[rasterID]->tileStripOffset.get());
	}
		break;
	case (278): //rowsperstrip
		tiffDetails[rasterID]->bitmapFormat = BitmapFormat::strips;
		tiffDetails[rasterID]->rowsPerStrip = GetFieldIntData(tag);
		break;
	case (279): //stripbytecount
		tiffDetails[rasterID]->tileStripByteCount = GetFieldIntData(tag);
		break;
	case (325): //tilebytecount
		tiffDetails[rasterID]->bitmapFormat = BitmapFormat::tiles;
		tiffDetails[rasterID]->tileStripByteCount = GetFieldIntData(tag);
		break;
	case (323): //tilelength
		tiffDetails[rasterID]->tileHeight = GetFieldIntData(tag);
		break;
	case (324): //tileoffsets
	{
		tiffDetails[rasterID]->tileStripOffset = std::unique_ptr<long int>(new long int[tag->count]);
		tiffDetails[rasterID]->noOfTilesOrStrips = tag->count;
		GetFieldIntArrayData(tag, tiffDetails[rasterID]->tileStripOffset.get());
	}
		break;
	case (322): //tilewidth
		tiffDetails[rasterID]->tileWidth = GetFieldIntData(tag);
		break;
	case (277): //samplesperpixel
		tiffDetails[rasterID]->samplesPerPixel = GetFieldIntData(tag);
		break;
	case (338): //extrasampletype
		tiffDetails[rasterID]->extraSampleType = GetFieldIntData(tag);
		break;
	case (262):
		tiffDetails[rasterID]->photometricInterpretation = GetFieldIntData(tag);
		break;
	case (284):
		tiffDetails[rasterID]->planarConfiguration = GetFieldIntData(tag);
		break;
	case (339):
		tiffDetails[rasterID]->sampleFormat = GetFieldIntData(tag);
		break;
	case (34735): //GeoKeyDirectory
		ProcessGeoKeyDirectory(rasterID, tag);
		break;
	case (34736): //DoubleParamsGeoKey
		if (doubleParamsGeoKeys.size() > 0)
		{
			//extract the params stored in this tag.
			std::unique_ptr<double> doubleParamsData = std::unique_ptr<double>(new double[tag->count]);
			double buffer;
			//std::cout << "count of doubleParamsData: " << tag->count <<std::endl; //test

			stream.seekg(tag->offsetValue);
			for (unsigned long int i = 0; i < tag->count; i++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				doubleParamsData.get()[i] = buffer;
			}

			//Loop over keys stored in doubleParamsGeoKeys and process them.
			for (std::vector<GeoKey>::iterator it = doubleParamsGeoKeys.begin(); it < doubleParamsGeoKeys.end(); ++it)
			{
				//std::cout << "Attempting to process a Double geokey of offset: " << it->offsetValue << ", and count: " << it->count << std::endl; //test
				ProcessGeoKey(rasterID, &(*it), doubleParamsData.get());
			}
		}
		break;
	case (34737): //ASCIIParamsGeoKey
		if (asciiParamsGeoKeys.size() > 0)
		{
			//extract the params stored in this tag.
			std::unique_ptr<char> asciiParamsData = std::unique_ptr<char>(new char[tag->count]);
			//std::cout << "count of asciiParamsData: " << tag->count << std::endl;//test

			stream.seekg(tag->offsetValue);

			char buffer = ' ';
			for (unsigned long int i = 0; i < tag->count; i++)
			{

				stream.read(&buffer, sizeof(buffer));
				asciiParamsData.get()[i] = buffer;
			}

			//Loop over keys stored in asciiParamsGeoKeys and process them.
			for (std::vector<GeoKey>::iterator it = asciiParamsGeoKeys.begin(); it < asciiParamsGeoKeys.end(); ++it)
			{
				//std::cout << "Attempting to process an ASCII geokey of offset: " << it->offsetValue << ", and count: " << it->count << std::endl; //test
				ProcessGeoKey(rasterID, &(*it), asciiParamsData.get());
			}
		}
		break;
	case (33550): //ModelPixelScaleTag
	{
		if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::matrix)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelPixelScaleTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelPixelScaleTag" << std::endl;
			break;
		}
		else
			geoDetails[rasterID]->transformationMethod = RasterToModelTransformationMethod::tieAndScale;

		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 3; i++)
		{
			stream.read((char*)&buffer, sizeof(buffer));
			geoDetails[rasterID]->pixelScale[i] = buffer;
		}
	}
		break;
	case (33922): //ModelTiepointTag
	{
		if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::matrix)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelTiepointTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelTiepointTag" << std::endl;
			break;
		}
		else
			geoDetails[rasterID]->transformationMethod = RasterToModelTransformationMethod::tieAndScale;

		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				geoDetails[rasterID]->tiePoints[i][j] = buffer;
			}
		}
	}
		break;
	case (34264): //ModelTransformationTag
	{
		if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::tieAndScale)
		{
			std::cout << "WARNING! This GeoTIFF has both ModelTransformationTag and ModelTiepointTag or ModePixelScaleTag set. This is against GeoTIFF spec." << std::endl;
			std::cout << "Skipping processing ModelTransformationTag" << std::endl;
			break;
		}
		else
			geoDetails[rasterID]->transformationMethod = RasterToModelTransformationMethod::matrix;


		stream.seekg(tag->offsetValue);
		double buffer;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				stream.read((char*)&buffer, sizeof(buffer));
				geoDetails[rasterID]->modelTransformationMatrix[i][j] = buffer;
			}
		}
	}
		break;
	default:
		break;
	}
}

bool ParseStripOrTileData(int rasterID, int stripOrTileID)
{
	switch (tiffDetails[rasterID]->compression)
	{
	case (1): //no compression
		ParseUncompressedStripOrTileData(rasterID, stripOrTileID, bitMaps[rasterID]);
		break;

	case (32773): //PackBits (Macintosh RLE)
		ParsePackBitsStripOrTileData(rasterID, stripOrTileID, bitMaps[rasterID]);
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
		//ParseDeflateStripOrTileData(stripOrTileID, bitMap);
		std::cout << "ERROR! Unsupported compression algorithm - Deflate" << std::endl;
		return false;
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

void DisplayTIFFDetailsOnCLI(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No TIFF is loaded to memory. (Invalid ID)" << std::endl;
		return;
	}
	if (tiffDetails[rasterID] == NULL)
	{
		std::cout << "ERROR! No TIFF is loaded to memory. (Expired ID)" << std::endl;
		return;
	}

	std::cout << "=======================================================================================" << std::endl;
	std::cout << "\t\tTIFF Details" << std::endl;
	std::cout << "=======================================================================================" << std::endl;

	std::cout << "Dimensions :" << tiffDetails[rasterID]->width << " x " << tiffDetails[rasterID]->height << std::endl;
	std::cout << "Samples Per Pixel: " << tiffDetails[rasterID]->samplesPerPixel << std::endl;
	std::cout << "Bits Per Sample: " << tiffDetails[rasterID]->bitsPerSample << std::endl;
	std::cout << "Extra Samples Type: " << tiffDetails[rasterID]->extraSampleType << std::endl;
	std::cout << "Sample Format: " << tiffDetails[rasterID]->sampleFormat << std::endl;
	std::cout << "Compression: " << tiffDetails[rasterID]->compression << std::endl;

	std::cout << "Photmetric Interpretation: " << tiffDetails[rasterID]->photometricInterpretation << std::endl;
	std::cout << "Planar Configuration: " << tiffDetails[rasterID]->planarConfiguration << std::endl;



	switch (tiffDetails[rasterID]->bitmapFormat)
	{
	case BitmapFormat::strips:
		std::cout << "Bitmap format: " << "strips" << std::endl;
		std::cout << "No. of Strip:" << tiffDetails[rasterID]->noOfTilesOrStrips << std::endl;
		std::cout << "Strip Byte Count: " << tiffDetails[rasterID]->tileStripByteCount << std::endl;
		std::cout << "Rows per Strip: " << tiffDetails[rasterID]->rowsPerStrip << std::endl;
		std::cout << "No. of Pixels per Strip: " << tiffDetails[rasterID]->noOfPixelsPerTileStrip << std::endl;

	/*	std::cout << "--------------------" << std::endl;
		std::cout << "Strip offsets:" << std::endl;
		for (int i = 0; i < tiffDetails[rasterID]->noOfTilesOrStrips; i++)
			std::cout << "Offset " << i << ": " << tiffDetails[rasterID]->tileStripOffset.get()[i] << std::endl;
		std::cout << "--------------------" << std::endl;*/
		break;

	case BitmapFormat::tiles:
		std::cout << "Bitmap format: " << "tiles" << std::endl;
		std::cout << "No. of Tiles:" << tiffDetails[rasterID]->noOfTilesOrStrips << std::endl;
		std::cout << "Tile Byte Count: " << tiffDetails[rasterID]->tileStripByteCount << std::endl;
		std::cout << "Tile Height: " << tiffDetails[rasterID]->tileHeight << std::endl;
		std::cout << "Tile Width: " << tiffDetails[rasterID]->tileWidth << std::endl;
		std::cout << "No. of Pixels per Tile: " << tiffDetails[rasterID]->noOfPixelsPerTileStrip << std::endl;

	/*	std::cout << "--------------------" << std::endl;
		std::cout << "Tile offsets:" << std::endl;
		for (int i = 0; i < tiffDetails[rasterID]->noOfTilesOrStrips; i++)
			std::cout << "Offset " << i << ": " << tiffDetails[rasterID]->tileStripOffset.get()[i] << std::endl;
		std::cout << "--------------------" << std::endl;*/
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

void DisplayGeoTIFFDetailsOnCLI(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No TIFF is loaded to memory. (Invalid ID)" << std::endl;
		return;
	}
	if (geoDetails[rasterID] == NULL)
	{
		std::cout << "ERROR! No TIFF is loaded to memory. (Expired ID)" << std::endl;
		return;
	}

	std::cout << "=======================================================================================" << std::endl;
	std::cout << "\t\tGeoTIFF Details" << std::endl;
	std::cout << "=======================================================================================" << std::endl;

	std::cout << "Raster Space: " << geoDetails[rasterID]->rasterSpace << std::endl;
	std::cout << "Model Type: " << geoDetails[rasterID]->modelType << std::endl;
	std::cout << "Transformation Method: " << (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::matrix? "Matrix" : "Tie and Scale") << std::endl;


	if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::tieAndScale)
	{
		std::cout << "Tie Points" << std::endl;
		for (int i = 0; i < 2; i++)
			std::cout << "\t" << geoDetails[rasterID]->tiePoints[i][0] << ",  " << geoDetails[rasterID]->tiePoints[i][1] << ",  " << geoDetails[rasterID]->tiePoints[i][2] << std::endl;
		std::cout << "Pixel Scale: " << geoDetails[rasterID]->pixelScale[0] << " x " << geoDetails[rasterID]->pixelScale[1] << " x " << geoDetails[rasterID]->pixelScale[2] << std::endl;
	}
	else if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::matrix)
	{
		std::cout << "Raster to Mode Transformation Matrix:" << std::endl;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				std::cout << geoDetails[rasterID]->modelTransformationMatrix[i][j] << "\t";
			
			std::cout << std::endl;
		}
	}

	std::cout << "Projected CRS: " << geoDetails[rasterID]->projectedCRS << std::endl;
	std::cout << "Geodetic CRS: " << geoDetails[rasterID]->geodeticCRS << std::endl;
	std::cout << "Vertical CRS: " << geoDetails[rasterID]->verticalCRS << std::endl;

	std::cout << "GeoTIFF Citation: " << geoDetails[rasterID]->geotiffCitation.c_str() << std::endl;
	std::cout << "Geodetic CRS Citation: " << geoDetails[rasterID]->geodeticCRSCitation.c_str() << std::endl;
	std::cout << "Projected CRS Citation: " << geoDetails[rasterID]->projectedCRSCitation.c_str() << std::endl;
	std::cout << "Vertical CRS Citation: " << geoDetails[rasterID]->verticalCRSCitation.c_str() << std::endl;

	std::cout << "Ellipsoid: " << geoDetails[rasterID]->ellipsoid << std::endl;
	std::cout << "Ellispod Semi Major Axis: " << geoDetails[rasterID]->ellipsoidSemiMajorAxis << std::endl;
	std::cout << "Ellipsoid Semi Minor Axis: " << geoDetails[rasterID]->ellipsoidSemiMinorAxis << std::endl;
	std::cout << "Ellipsoid Inverse Flattening: " << geoDetails[rasterID]->ellipsoidInvFlattening << std::endl;

	std::cout << "Vertical Datum: " << geoDetails[rasterID]->verticalDatum << std::endl;

	std::cout << "Bounding Box:" << std::endl;
	std::cout << "\t" << geoDetails[rasterID]->cornerNW[0] << ", " << geoDetails[rasterID]->cornerNW[1] << "\t\t" << geoDetails[rasterID]->cornerNE[0] << ", " << geoDetails[rasterID]->cornerNE[1] << std::endl;
	std::cout << "\t" << geoDetails[rasterID]->cornerSW[0] << ", " << geoDetails[rasterID]->cornerSW[1] << "\t\t" << geoDetails[rasterID]->cornerSE[0] << ", " << geoDetails[rasterID]->cornerSE[1] << std::endl;

	std::cout << "=======================================================================================" << std::endl;
	std::cout << "=======================================================================================" << std::endl;
}

bool OpenTIFFFile(std::string filePath)
{
	stream.clear();
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
	//char byte[1];
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

bool ParseFirstIFDHeader(int rasterID)
{
	//char byte[1];
	char word[2];
	char dword[4];

	//seek to first IFD begining
	stream.seekg(firstIFDOffset, stream.beg);

	//check IFD header
	//std::cout << "Current file loc: " << stream.tellg() << "\t";
	stream.read(word, sizeof(word));
	short int noOfTagsInIFD = BytesToInt16(word);
	//std::cout << "Number of IFD enteries: " << noOfTagsInIFD << std::endl;

	std::streampos endOfLastTag = stream.tellg(); //Because ProcessTag() may modify the position in the stream and is called at end of loop, any further reading of tags wouldn't be correct.
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

		ProcessTag(rasterID, tag.get());
	}

	//Fill out our last remaining TIFFDetail, the Number of Pixels in each strip/tile.
	tiffDetails[rasterID]->noOfPixelsPerTileStrip = tiffDetails[rasterID]->tileStripByteCount / (tiffDetails[rasterID]->samplesPerPixel * tiffDetails[rasterID]->bitsPerSample / 8);

	if (viewTagsInCLI)
	{
		std::cout << "===================================================================" << std::endl;
		std::cout << "Finished processing tags" << std::endl;
		std::cout << "===================================================================" << std::endl;
	}

	return true;
}

bool AllocateBitmapMemory(int rasterID)
{
	Matrix_f64 * newBitmap;
	bitMaps.push_back(newBitmap);
	try
	{
		bitMaps[rasterID] = new Matrix_f64[tiffDetails[rasterID]->width];
		for (unsigned long int i = 0; i < tiffDetails[rasterID]->width; i++)
		{
			bitMaps[rasterID][i] = Matrix_f64(tiffDetails[rasterID]->height, tiffDetails[rasterID]->samplesPerPixel);
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

void DeallocateBitmapMemory(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
		return;
	
	if (bitMaps[rasterID] != NULL)
	{
		for (unsigned long int i = 0; i < tiffDetails[rasterID]->width; i++)
			(bitMaps[rasterID])[i].~Matrix_f64();
		
		delete[] bitMaps[rasterID];
		bitMaps[rasterID] = NULL;
	}
}

bool ComputeGeographicBoundingBox(int rasterID) //technically, we're only computing a 2D rect here...
{
	//Current implementation assumes tiepoints in image space is pixel 0,0.
	if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::tieAndScale)
	{
		geoDetails[rasterID]->cornerSW[0] = geoDetails[rasterID]->tiePoints[1][0];
		geoDetails[rasterID]->cornerSW[1] = geoDetails[rasterID]->tiePoints[1][1] - geoDetails[rasterID]->pixelScale[1] * tiffDetails[rasterID]->height;

		geoDetails[rasterID]->cornerNW[0] = geoDetails[rasterID]->tiePoints[1][0];
		geoDetails[rasterID]->cornerNW[1] = geoDetails[rasterID]->tiePoints[1][1];

		geoDetails[rasterID]->cornerNE[0] = geoDetails[rasterID]->tiePoints[1][0] + geoDetails[rasterID]->pixelScale[0] * tiffDetails[rasterID]->width;
		geoDetails[rasterID]->cornerNE[1] = geoDetails[rasterID]->tiePoints[1][1];

		geoDetails[rasterID]->cornerSE[0] = geoDetails[rasterID]->tiePoints[1][0] + geoDetails[rasterID]->pixelScale[0] * tiffDetails[rasterID]->width;
		geoDetails[rasterID]->cornerSE[1] = geoDetails[rasterID]->tiePoints[1][1]- geoDetails[rasterID]->pixelScale[1] * tiffDetails[rasterID]->height;

	}
	else if (geoDetails[rasterID]->transformationMethod == RasterToModelTransformationMethod::matrix)
	{
		std::cout << "ERROR! Cannot determine the GeoTIFF's bounding box, Matrix Raster-Model transformation method is not supported." << std::endl;
		return false;
	}
	else
	{
		std::cout << "ERROR! Cannot determine the GeoTIFF's bounding box, Raster-Model transformation method is not set." << std::endl;
		return false;
	}
	
	return true;
}

bool ParseFirstBitmap(int rasterID)
{
	if (tiffDetails[rasterID]->planarConfiguration != 1)
	{
		std::cout << "ERROR! This reader cannot parse non-chunky (TIFF6.0 Planar Configuration other than 1) TIFF files." << std::endl;
		return false;
	}
	else if (tiffDetails[rasterID]->bitsPerSample != 8 && tiffDetails[rasterID]->bitsPerSample != 16 && tiffDetails[rasterID]->bitsPerSample != 32)
	{
		std::cout << "ERROR! This reader can only parse 8, 16 and 32 bits-per-samples images." << std::endl;
		return false;
	}
	else
	{
		for (unsigned long int i = 0; i < tiffDetails[rasterID]->noOfTilesOrStrips; i++)
		{
			if (!ParseStripOrTileData(rasterID, i))
				return false;
		}
	}

	return true;
}

void DisplayBitmapOnCLI(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Invalid ID)" << std::endl;
		return;
	}
	if (bitMaps[rasterID] == NULL)
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Expired ID)" << std::endl;
		return;
	}

	for (unsigned long int i = 0; i < tiffDetails[rasterID]->height; i++)
	{
		for (unsigned long int j = 0; j < tiffDetails[rasterID]->width; j++)
		{
			for (unsigned long int k = 0; k < tiffDetails[rasterID]->samplesPerPixel; k++)
			{
				if (k > 0)
					std::cout << ",";
				std::cout << (bitMaps[rasterID])[j][i][k];
			}
			std::cout << "\t";
		}
		std::cout << "[ROW_ " << i << "_END]" << std::endl << std::endl;
	}
}

void PurgeAll()
{
	//for (auto it = bitMaps.begin(); it != bitMaps.end(); ++it)
	for (int i = 0; i < bitMaps.size(); i++)
	{
		if (bitMaps[i] != NULL)
			for (unsigned long int j = 0; j < tiffDetails[i]->width; j++)
				(bitMaps[i])[j].~Matrix_f64();
		delete[] bitMaps[i];
	}
	bitMaps.clear();

	for (int i = 0; i < geoDetails.size(); i++)
	{
		if (geoDetails[i] != NULL)
			delete geoDetails[i];
	}
	geoDetails.clear();

	for (int i = 0; i < tiffDetails.size(); i++)
	{
		if (tiffDetails[i] != NULL)
			delete tiffDetails[i];
	}
	tiffDetails.clear();


	intParamsGeoKeys.clear();
	doubleParamsGeoKeys.clear();
	asciiParamsGeoKeys.clear();
}

bool LoadGeoTIFFHeaders(int rasterID, const char * filePath, bool closeStreamAtEnd) //Loads only the headers of the file. Usefull in case we want to check the file's specs before loading its bitmap content
{
	std::string path(filePath);
	return LoadGeoTIFFHeaders(rasterID, path);
}

bool LoadGeoTIFFHeaders(int rasterID, const std::string &filePath, bool closeStreamAtEnd) //Loads only the headers of the file. Usefull in case we want to check the file's specs before loading its bitmap content
{
	if (!OpenTIFFFile(filePath))
		return false;

	if (!ParseTIFFHeader())
	{
		if (stream.is_open())
			stream.close();
		return false;
	}

	if (!ParseFirstIFDHeader(rasterID)) //Currently, no error checking is done in ParseFirstIFDHeader(), but future work should include some.
	{

		if (stream.is_open())
			stream.close();
		return false;
	}

	if (closeStreamAtEnd && stream.is_open())
		stream.close();


	return true;
}

bool LoadGeoTIFF(const char * filePath, int * outGeoTIFFID) //Load entire GeoTIFF file -including bitmap- to memory.
{
	std::string path(filePath);
	return LoadGeoTIFF(path, outGeoTIFFID);
}

bool LoadGeoTIFF(const std::string &filePath, int * outGeoTIFFID) //Load entire GeoTIFF file -including bitmap- to memory.
{
	//create new id
	GeoTIFFDetails * newGeoDetails = new GeoTIFFDetails();
	TIFFDetails * newTiffDetails = new TIFFDetails();

	geoDetails.push_back(newGeoDetails);
	tiffDetails.push_back(newTiffDetails);
	
	int newID = *outGeoTIFFID  = geoDetails.size() - 1;

	if (!LoadGeoTIFFHeaders(newID, filePath, false)) //file open-ability check is done inside LoadGeoTIFFHeaders().
		return false;

	if (!AllocateBitmapMemory(newID))
	{
		if (stream.is_open())
			stream.close();
		return false;
	}

	if (!ParseFirstBitmap(newID))
	{
		UnloadGeoTIFF(newID);
		return false;
	}

	stream.close();

	ComputeGeographicBoundingBox(newID);

	//Don't need geokeys anymore
	intParamsGeoKeys.clear();
	doubleParamsGeoKeys.clear();
	asciiParamsGeoKeys.clear();

	return true;
}

void UnloadGeoTIFF(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
		return;

	DeallocateBitmapMemory(rasterID);

	intParamsGeoKeys.clear();
	doubleParamsGeoKeys.clear();
	asciiParamsGeoKeys.clear();

	if (stream.is_open())
		stream.close();
	stream.clear();

	delete tiffDetails[rasterID];	
	delete geoDetails[rasterID];

	tiffDetails[rasterID] = NULL;
	geoDetails[rasterID] = NULL;
}

Matrix_f64 const * GetPointerToBitmap(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Invalid ID)" << std::endl;
		return NULL;
	}
	if (bitMaps[rasterID] == NULL)
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Expired ID)" << std::endl;
		return NULL;
	}

	return bitMaps[rasterID];
}

TIFFDetails const * GetPointerToTIFFDetails(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Invalid ID)" << std::endl;
		return NULL;
	}
	if (bitMaps[rasterID] == NULL)
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Expired ID)" << std::endl;
		return NULL;
	}

	return tiffDetails[rasterID];
}

GeoTIFFDetails const * GetPointerToGeoTIFFDetails(int rasterID)
{
	if (!IsLoadedRaster(rasterID))
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Invalid ID)" << std::endl;
		return NULL;
	}
	if (bitMaps[rasterID] == NULL)
	{
		std::cout << "ERROR! No Bitmap is loaded to memory. (Expired ID)" << std::endl;
		return NULL;
	}

	return geoDetails[rasterID];
}

double GetSample(int rasterID, unsigned long int x, unsigned long int y, unsigned int sampleOrder)
{
	if (rasterID >= bitMaps.size())
	{
		std::cout << "ERROR! No loaded raster with provided ID." << std::endl;
		return std::numeric_limits<float>().min();
	}
	if (x > tiffDetails[rasterID]->width || y > tiffDetails[rasterID]->height || sampleOrder > tiffDetails[rasterID]->samplesPerPixel)
	{
		std::cout << "ERROR! Attempting to get a sample outside boundary of bitmap." << std::endl;
		return std::numeric_limits<float>().min();
	}
	else
		return (bitMaps[rasterID])[x][y][sampleOrder];
}
