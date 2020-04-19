#pragma once
#include <iostream>
#include <fstream>
#include <memory>


#include "Array2D.h"
#include "GeoTIFF_Parser_Globals.h"
#include "Decompressor.h"


//variables
extern bool viewTagsInCLI;


bool LoadGeoTIFF(std::string filePath);