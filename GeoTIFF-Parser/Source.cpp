#include "GeoTIFF_Parser.h"


int main()
{
	
	//LoadGeoTIFF("dem_GCS.tif");
	LoadGeoTIFF("dem_GSC_uncomp.tif");
	//LoadGeoTIFF("testTIFF.tif");
	//LoadGeoTIFF("testTIFF_32.tif");
	
	std::cout << "FINISHED!" << std::endl; //test
	
	return 0;
}