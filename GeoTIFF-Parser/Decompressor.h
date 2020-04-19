#pragma once
#include <fstream>
#include <iostream>

#include "GeoTIFF_Parser_Globals.h"


void ParseUncompressedStripOrTileData(int stripOrTileID);

void ParseDeflateStripOrTileData(int stripOrTileID);