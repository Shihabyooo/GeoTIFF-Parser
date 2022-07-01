#include "GeoTIFF_Parser.h"


int main()
{
	//char test;
	//std::cout << "start" << std::endl; //test for debugging.
	//std::cin >> test;
	//std::cout << "starting" << std::endl;
	//LoadGeoTIFFHeaders("C:\\Users\\Shihabyooo\\source\\repos\\GeoTIFF-Parser\\x64\\Release\\TestData\\alos_UTM_nocomp_int16.tif");
	//end test

	int rasterID;

	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_int16.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int16.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float64.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_float32.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int32.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_float32.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_deflate_uint16.tif", &rasterID);
	//LoadGeoTIFF("TestData\\alos_UTM_deflate_float32.tif", &rasterID);
	//LoadGeoTIFF("TestData\\testTIFF.tif", &rasterID);
	//LoadGeoTIFF("TestData\\testTIFF_32.tif", &rasterID);
	//LoadGeoTIFF("wdem_uncomp.tif", &rasterID);
	//LoadGeoTIFF("wdem_packbits.tif", &rasterID);
	
	//DisplayTIFFDetailsOnCLI(rasterID);
	//DisplayGeoTIFFDetailsOnCLI(rasterID);
	//DisplayBitmapOnCLI(rasterID);


	//int x = 1000;
	//int y = 2000;
	////std::cout << "Height at coord: " << geoDetails.tiePoints[1][0] + x * geoDetails.pixelScale[0] << " x " << geoDetails.tiePoints[1][1] - y * geoDetails.pixelScale[1] << ": " << GetPointerToBitmap()[x].GetValue(y, 0) << std::endl;
	////std::cout << "Height at coord: " << geoDetails.tiePoints[1][0] + x * geoDetails.pixelScale[0] << " x " << geoDetails.tiePoints[1][1] - y * geoDetails.pixelScale[1] << ": " << GetSample(x, y, 0) << std::endl;

	//int secondRasterID;
	//LoadGeoTIFF("TestData\\alos_UTM_packbits_int16.tif", &secondRasterID);

	//DisplayTIFFDetailsOnCLI(secondRasterID);
	//DisplayGeoTIFFDetailsOnCLI(secondRasterID);
	//DisplayBitmapOnCLI(secondRasterID);

	//UnloadGeoTIFF(rasterID);
	//
	//int thirdRasterID;
	//LoadGeoTIFF("TestData\\alos_UTM_nocomp_float32.tif", &thirdRasterID);
	//DisplayTIFFDetailsOnCLI(thirdRasterID);
	//DisplayGeoTIFFDetailsOnCLI(thirdRasterID);
	//DisplayBitmapOnCLI(thirdRasterID);

	////UnloadGeoTIFF(rasterID);
	//UnloadGeoTIFF(secondRasterID);
	//UnloadGeoTIFF(thirdRasterID);

	int slopeID;
	//TODO there is a memory out of bound io with the raster bellow. Doesn't happen with the packbit version.
	//LoadGeoTIFF("C:\\Test\\Slope_Percent.tif", &slopeID);
	LoadGeoTIFF("C:\\Test\\Slope_packbit.tif", &slopeID);

	Matrix_f64 const * map = GetPointerToBitmap(slopeID);
	TIFFDetails const * details = GetPointerToTIFFDetails(slopeID);

	for (int i = 0; i < details->width; i++)
	{
		for (int j = 0; j < details->height; j++)
			std::cout << map[0][j][i] << " ";
	}
	std::cout << std::endl;

	UnloadGeoTIFF(slopeID);
	//UnloadGeoTIFF(rasterID);
	std::cout << "FINISHED!" << std::endl; //test
	return 0;
}