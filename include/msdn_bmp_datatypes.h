// BMP-related data types

#pragma once
#include <cstdint>

/**
 * Common Data Types
 *
 * The data types in this section are essentially aliases for C/C++
 * primitive data types.
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/cc230309.aspx.
 * See http://en.wikipedia.org/wiki/Stdint.h for more on stdint.h.
 */
using BYTE = std::uint8_t;
using DWORD = std::uint32_t;
using LONG = std::int32_t;
using WORD = std::uint16_t;

/**
 * BITMAPFILEHEADER
 *
 * The BITMAPFILEHEADER structure contains information about the type, size,
 * and layout of a file that contains a DIB [device-independent bitmap].
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx.
 */
#pragma pack(push, 1)
struct BITMAPFILEHEADER
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2; 
    DWORD  bfOffBits; // Where actual data begins
};
#pragma pack(pop)

/**
 * BITMAPINFOHEADER
 *
 * The BITMAPINFOHEADER structure contains information about the
 * dimensions and color format of a DIB [device-independent bitmap].
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx.
 */
#pragma pack(push, 1)
struct BITMAPINFOHEADER
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
};
#pragma pack(pop)

/**
 * RGBTRIPLE
 *
 * This structure describes a color consisting of relative intensities of
 * red, green, and blue.
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/aa922590.aspx.
 */
#pragma pack(push, 1)
struct RGBTRIPLE
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
};
#pragma pack(pop)

/**
 * RGBQUAD
 *
 * This structure describes a color table entry for BMP files (used in 1, 4, or 8-bit BMPs).
 * Each entry contains blue, green, red intensities, and a reserved byte.
 *
 * Adapted from https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-rgbquad
 */
#pragma pack(push, 1)
struct RGBQUAD
{
    BYTE  rgbBlue;
    BYTE  rgbGreen;
    BYTE  rgbRed;
    BYTE  rgbReserved;
};
#pragma pack(pop)