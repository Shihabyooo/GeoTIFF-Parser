#pragma once
#include <fstream>
#include <iostream>

#include "GeoTIFF_Parser_Globals.h"

enum class DeflateBlockType
{
	noCompression, fixedHuffman, dynamicHuffman, unused
};

void ParseUncompressedStripOrTileData(int rasterID, int stripOrTileID, Matrix_f32 * const _bitMap);

void ParseDeflateStripOrTileData(int rasterID, int stripOrTileID, Matrix_f32 * const _bitMap);

void ParsePackBitsStripOrTileData(int rasterID, int stripOrTileID, Matrix_f32 * const _bitMap);
