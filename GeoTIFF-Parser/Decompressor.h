#pragma once
#include <fstream>
#include <iostream>

#include "GeoTIFF_Parser_Globals.h"

enum DeflateBlockType
{
	noCompression, fixedHuffman, dynamicHuffman, unused
};


void ParseUncompressedStripOrTileData(int stripOrTileID);

void ParseDeflateStripOrTileData(int stripOrTileID);