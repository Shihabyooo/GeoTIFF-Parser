#include "GeoTIFF_Parser.h"


int main()
{
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_int16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float64.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float32.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int32.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_float32.tif");
	LoadGeoTIFF("TestData\\alos_UTM_deflate_uint16.tif");
	//LoadGeoTIFF("TestData\\alos_UTM_deflate_float32.tif");
	//LoadGeoTIFF("TestData\\testTIFF.tif");
	//LoadGeoTIFF("TestData\\testTIFF_32.tif");
	

	DisplayBitmapOnCLI();
	UnloadGeoTIFF();
	std::cout << "FINISHED!" << std::endl; //test
	return 0;
}