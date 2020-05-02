#include "GeoTIFF_Parser.h"


int main()
{
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_int16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float64.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float32.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int32.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_float32.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_deflate_uint16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_deflate_float32.tif");
	//LoadGeoTIFF("TestData\\testTIFF.tif");
	//LoadGeoTIFF("TestData\\testTIFF_32.tif");
	//LoadGeoTIFF("wdem_uncomp.tif");
	LoadGeoTIFF("wdem_packbits.tif");
	
	DisplayGeoTIFFDetailsOnCLI();
	//DisplayBitmapOnCLI();



	int x = 1000;
	int y = 2000;
	//std::cout << "Height at coord: " << geoDetails.tiePoints[1][0] + x * geoDetails.pixelScale[0] << " x " << geoDetails.tiePoints[1][1] - y * geoDetails.pixelScale[1] << ": " << GetPointerToBitmap()[x].GetValue(y, 0) << std::endl;
	std::cout << "Height at coord: " << geoDetails.tiePoints[1][0] + x * geoDetails.pixelScale[0] << " x " << geoDetails.tiePoints[1][1] - y * geoDetails.pixelScale[1] << ": " << GetSample(x, y, 0) << std::endl;



	UnloadGeoTIFF();
	std::cout << "FINISHED!" << std::endl; //test
	return 0;
}