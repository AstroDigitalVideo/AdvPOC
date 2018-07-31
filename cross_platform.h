/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ADV_CROSS_PLATFORM
#define ADV_CROSS_PLATFORM

#define _FILE_OFFSET_BITS  64
#include "stdio.h"

typedef int long long int64_t;
typedef unsigned int long long uint64_t;

typedef int ADVRESULT;
typedef int bool;

#define true 1
#define false 0
#define E_ADV_NOFILE ((ADVRESULT)0x81000001L)
#define E_ADV_IO_ERROR ((ADVRESULT)0x81000002L)

#define E_ADV_STATUS_ENTRY_ALREADY_ADDED ((ADVRESULT)0x81001001L)
#define E_ADV_INVALID_STATUS_TAG_ID ((ADVRESULT)0x81001002L)
#define E_ADV_INVALID_STATUS_TAG_TYPE ((ADVRESULT)0x81001003L)
#define E_ADV_STATUS_TAG_NOT_FOUND_IN_FRAME ((ADVRESULT)0x81001004L)
#define E_ADV_FRAME_STATUS_NOT_LOADED ((ADVRESULT)0x81001005L)
#define E_ADV_FRAME_NOT_STARTED ((ADVRESULT)0x81001006L)
#define E_ADV_IMAGE_NOT_ADDED_TO_FRAME ((ADVRESULT)0x81001007L)
#define E_ADV_INVALID_STREAM_ID ((ADVRESULT)0x81001008L)
#define E_ADV_IMAGE_SECTION_UNDEFINED ((ADVRESULT)0x81001009L)
#define E_ADV_STATUS_SECTION_UNDEFINED ((ADVRESULT)0x8100100AL)
#define E_ADV_IMAGE_LAYOUTS_UNDEFINED ((ADVRESULT)0x8100100BL)
#define E_ADV_INVALID_IMAGE_LAYOUT_ID ((ADVRESULT)0x8100100CL)
#define E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW ((ADVRESULT)0x8100100DL)
#define E_ADV_IMAGE_SECTION_ALREADY_DEFINED ((ADVRESULT)0x8100100EL)
#define E_ADV_STATUS_SECTION_ALREADY_DEFINED ((ADVRESULT)0x8100100FL)
#define E_ADV_IMAGE_LAYOUT_ALREADY_DEFINED ((ADVRESULT)0x81001010L)
#define E_ADV_INVALID_IMAGE_LAYOUT_TYPE ((ADVRESULT)0x81001011L)
#define E_ADV_INVALID_IMAGE_LAYOUT_COMPRESSION ((ADVRESULT)0x81001012L)
#define E_ADV_INVALID_IMAGE_LAYOUT_BPP ((ADVRESULT)0x81001013L)
#define E_ADV_FRAME_MISSING_FROM_INDEX ((ADVRESULT)0x81001014L)
#define E_ADV_FRAME_CORRUPTED ((ADVRESULT)0x81001015L)
#define E_ADV_FILE_NOT_OPEN ((ADVRESULT)0x81001016L)

#define E_ADV_FILE_HASH_KEY_NULL ((ADVRESULT)0x81010001L)
#define E_ADV_FILE_HASH_KEY_TOO_LONG ((ADVRESULT)0x81010002L)
#define E_ADV_FILE_HASH_VALUE_TOO_LONG ((ADVRESULT)0x81010003L)

#define S_ADV_TAG_REPLACED ((ADVRESULT)0x71000001L)

#ifndef _WIN32
#define S_OK 0
#define E_FAIL ((ADVRESULT)0x80004005L)
#define E_NOTIMPL ((ADVRESULT)0x80004001L)
#else
#include "windows.h"
#endif

#ifndef _WIN32

typedef int BOOL;
typedef int LONG;
typedef unsigned short int WORD;
typedef unsigned char BYTE;
typedef unsigned int DWORD;

#define BI_RGB 0

typedef void* HBITMAP;

#pragma pack(1)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  int  biWidth;
  int  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  int  biXPelsPerMeter;
  int  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack()

#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack()

#endif

#if __linux__
#include <time.h>
#endif

#if __GNUC__
#define strcpy_s(x, y, z) strcpy(x, z)
#define strncpy_s(x, y, z, t) strcpy(x, z)
#define _snprintf_s(x, y, z, t) snprintf(x, y, z, t);
void fopen_s(FILE **f, const char *name, const char *mode);
#endif

#if _MSC_VER
#define snprintf _snprintf
#define MSVC
// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <SDKDDKVer.h>

#endif

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
 /*
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
   */
    #define DLL_PUBLIC __declspec(dllexport)
    #define DLL_LOCAL   
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC __declspec(dllexport)
    #define DLL_LOCAL
  #endif
#endif

#if __GNUC__
void fopen_s(FILE **f, const char *name, const char *mode) {
    *f = fopen(name, mode);
}
#endif

int advfclose(FILE* file)
{
	fclose(file);
	return 0;
}

FILE* advfopen(const char* fileName, const char* modes)
{
	FILE* file;
	fopen_s(&file, fileName, modes);
	return file;
}

size_t advfwrite(const void* pData, size_t size, size_t count, FILE* file)
{
	size_t written = fwrite(pData, size, count, file);
	return written;
}

void advfread(void* pData, size_t size, size_t count, FILE* file)
{
	fread(pData, size, count, file);
}

void advfgetpos64(FILE* file, int64_t* pos)
{	
#ifdef MSVC
	*pos = _ftelli64(file);
#elif _WIN32 || _WIN64
	int rv = fgetpos(file, (fpos_t*)(pos));
#elif __linux__
	int rv = fgetpos64(file, (fpos64_t*)(pos));
#else
	#error Platform not supported
#endif
}

int advfsetposorg64(FILE* file, const int64_t* pos, int origin)
{
#ifdef MSVC
	int rv = _fseeki64(file, *pos, origin);
#elif __linux__
	int rv = fseeko64(file, (__off64_t)(*pos), origin);
#elif _WIN32
	int rv = fseeko64(file, *pos, origin);
#elif __APPLE__
	int rv = fseeko(file, (off_t)*pos, origin);
#else
	#error Platform not supported
#endif
	
	return rv;
}

int advfsetpos64(FILE* file, const int64_t* pos)
{	
#ifdef MSVC
	int rv = _fseeki64(file, *pos, SEEK_SET);
#elif __linux__
	int rv = fsetpos64(file, (fpos64_t*)pos);
#elif _WIN32
	int rv = fseeko64(file, *pos, SEEK_SET);
#elif __APPLE__
	int rv = fseeko(file, (off_t )*pos, SEEK_SET);
#else
	#error Platform not supported
#endif
		
	return rv;
}

int advfseek(FILE* file, int64_t off, int whence)
{
	int rv = advfsetposorg64(file, &off, whence);
	return rv;
}

int advfflush(FILE* file)
{
	int rv = fflush(file);
	return rv;
}

void WriteUTF8String(FILE* pFile, const char* str)
{
	unsigned short len;
	len = strlen(str);
	
	advfwrite(&len, 2, 1, pFile);
	advfwrite(&str[0], len, 1, pFile);
}

int64_t advgetclockresolution()
{
	int64_t rv = 0;
#ifdef MSVC
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	rv = li.QuadPart;
#elif __linux__
	rv = 1E+09; // On Linux time is returned in nanoseconds so frequency is 1E+09 Hz
#elif _WIN32
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	rv = li.QuadPart;
#elif __APPLE__
	rv = 0;
#else
	#error Platform not supported
#endif
	return rv;
}

int64_t advgetclockticks()
{
	int64_t rv = 0;
#ifdef MSVC
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	rv = li.QuadPart;
#elif __linux__
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	rv = spec.tv_sec * 1E+09 + spec.tv_nsec;
#elif _WIN32
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	rv = li.QuadPart;
#elif __APPLE__
	rv = 0;
#else
	#error Platform not supported
#endif
	return rv;
}

ADVRESULT CheckFileName(const char* fileName, bool failIfExists)
{
	ADVRESULT rv = S_OK;
#ifdef MSVC
	// TODO: 
#elif __linux__
	// TODO: 
#elif _WIN32
	// TODO: 
#elif __APPLE__
	// TODO: 
#else
	#error Platform not supported
#endif
	return rv;
}

#endif