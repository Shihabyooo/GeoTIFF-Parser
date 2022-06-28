//Currently only supports TIFF files with one image (IFD), as in this program will only parse the contents of the first image within the TIFF file, disregarding the rest.
//Support for compressed TIFF files is not yet implemented.
//Support for Tile-Bitmaped TIFF files is not yet implemented.

#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include "GeoTIFF_Parser_Globals.h"

bool LoadGeoTIFFHeaders(int rasterID, const char * filePath, bool closeStreamAtEnd = true);
bool LoadGeoTIFFHeaders(int rasterID, const std::string &filePath, bool closeStreamAtEnd = true);
bool LoadGeoTIFF(const char * filePath, int * outGeoTIFFID);
bool LoadGeoTIFF(const std::string &filePath, int * outGeoTIFFID);
void UnloadGeoTIFF(int rasterID);
Matrix_f32 const * GetPointerToBitmap(int rasterID);
TIFFDetails const * GetPointerToTIFFDetails(int rasterID);
GeoTIFFDetails const * GetPointerToGeoTIFFDetails(int rasterID);

double GetSample(int rasterID, unsigned long int x, unsigned long int y, unsigned int sampleOrder);

void DisplayTIFFDetailsOnCLI(int rasterID);
void DisplayGeoTIFFDetailsOnCLI(int rasterID);
void DisplayBitmapOnCLI(int rasterID);

void PurgeAll(); //nukes all loaded memory and resets all ids. Don't see it ever having practical use (given how large vectors can be).