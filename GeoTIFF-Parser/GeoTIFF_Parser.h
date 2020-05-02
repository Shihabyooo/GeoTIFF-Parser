//Currently only supports TIFF files with one image (IFD), as in this program will only parse the contents of the first image within the TIFF file, disregarding the rest.
//Support for compressed TIFF files is not yet implemented.
//Support for Tile-Bitmaped TIFF files is not yet implemented.



#pragma once
#include <iostream>
#include <fstream>
#include <memory>


#include "Array2D.h"
#include "GeoTIFF_Parser_Globals.h"
#include "Decompressor.h"


bool LoadGeoTIFF(std::string filePath);
void UnloadGeoTIFF();
const Array2D * GetPointerToBitmap();
double GetSample(int x, int y, int sampleOrder);

void DisplayTIFFDetailsOnCLI();
void DisplayGeoTIFFDetailsOnCLI();
void DisplayBitmapOnCLI();
