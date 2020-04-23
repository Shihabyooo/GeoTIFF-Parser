#include "GeoTIFF_Parser.h"


int main()
{
	
	//LoadGeoTIFF("dem_GCS.tif");
	//LoadGeoTIFF("dem_GSC_packbits.tif");
	//LoadGeoTIFF("alos_UTM_nocomp_int16.tif");
	LoadGeoTIFF("alos_UTM_packbits_int16.tif");
	//LoadGeoTIFF("dem_GSC_uncomp.tif");
	//LoadGeoTIFF("dem_UTM_uncomp.tif");
	//LoadGeoTIFF("testTIFF.tif");
	//LoadGeoTIFF("testTIFF_32.tif");
	
	std::cout << "FINISHED!" << std::endl; //test
	
	return 0;
}