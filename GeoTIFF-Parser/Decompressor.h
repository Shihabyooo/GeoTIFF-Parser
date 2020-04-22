#pragma once
#include <fstream>
#include <iostream>

#include "GeoTIFF_Parser_Globals.h"

enum DeflateBlockType
{
	noCompression, fixedHuffman, dynamicHuffman, unused
};


void ParseUncompressedStripOrTileData(int stripOrTileID, Array2D * const _bitMap);

void ParseDeflateStripOrTileData(int stripOrTileID, Array2D * const _bitMap);