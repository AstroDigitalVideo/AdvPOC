/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ADV_POC
#define ADV_POC

#include "cross_platform.h"

// https://troydhanson.github.io/uthash/
#include "uthash.h" 
#include "utarray.h"

#define CORE_VERSION "2.0f"

extern char* g_CurrentAdvFile;
extern FILE* g_AdvFile;
extern bool g_FileStarted;

enum GetByteOperation
{
	None = 0,
	ConvertTo12BitPacked = 1,
	ConvertTo8BitBytesLooseHighByte = 2
};

enum Adv2TagType
{
	Int8 = 0,
	Int16 = 1,
	Int32 = 2,
	Long64 = 3,
	Real4 = 4,
	UTF8String = 5
};

struct AdvFileInfo
{
   int Width;
   int Height;
   int CountMaintFrames;
   int CountCalibrationFrames;
   int DataBpp;
   int MaxPixelValue;	   
   int64_t MainClockFrequency;
   int MainStreamAccuracy;
   int64_t CalibrationClockFrequency;
   int CalibrationStreamAccuracy;
   unsigned char MainStreamTagsCount;
   unsigned char CalibrationStreamTagsCount;
   unsigned char SystemMetadataTagsCount;
   unsigned char UserMetadataTagsCount;
   int64_t UtcTimestampAccuracyInNanoseconds;
   bool IsColourImage;
   int ImageLayoutsCount;
   int StatusTagsCount;
   int ImageSectionTagsCount;
   int ErrorStatusTagId;
};

struct AdvFrameInfo
{	
	unsigned int StartTicksLo;
	unsigned int StartTicksHi;
	unsigned int EndTicksLo;
	unsigned int EndTicksHi;
	
	unsigned int UtcTimestampLo;
	unsigned int UtcTimestampHi;
	unsigned int Exposure;

	float Gamma;
	float Gain;
	float Shutter;
	float Offset;

	unsigned char GPSTrackedSattelites;
	unsigned char GPSAlmanacStatus;
	unsigned char GPSFixStatus;
	char GPSAlmanacOffset;

	unsigned int VideoCameraFrameIdLo;
	unsigned int VideoCameraFrameIdHi;
	unsigned int HardwareTimerFrameIdLo;
	unsigned int HardwareTimerFrameIdHi;

	unsigned int SystemTimestampLo;
	unsigned int SystemTimestampHi;

	unsigned int ImageLayoutId;
	unsigned int RawDataBlockSize;
};

struct AdvImageLayoutInfo
{
	int ImageLayoutId;
	int ImageLayoutTagsCount;
	char ImageLayoutBpp;
	bool IsFullImageRaw;
	bool Is12BitImagePacked;
	bool Is8BitColourImage;
};

struct AdvIndexEntry
{
	int64_t ElapsedTicks;
	int64_t FrameOffset;
	unsigned int  BytesCount;
};

enum ImageByteOrder
{
	BigEndian = 0,
	LittleEndian = 1
};

int64_t m_MainStreamClockFrequency;
unsigned int m_MainStreamTickAccuracy;
int64_t m_CalibrationStreamClockFrequency;
unsigned int m_CalibrationStreamTickAccuracy;
bool m_UsesExternalMainStreamClock;
bool m_UsesExternalCalibrationStreamClock;

int m_NumberOfMainFrames;
int m_NumberOfCalibrationFrames;

bool m_ImageAdded;
bool m_FrameStarted;
int m_LastSystemSpecificFileError;
bool m_FileDefinitionMode;

// File
int64_t m_NewFrameOffset;
unsigned int m_FrameNo;

unsigned char *m_FrameBytes;
unsigned int m_FrameBufferIndex; 
unsigned int m_ElapedTime;

#define MAX_MAP_KEY 64
#define MAX_MAP_VALUE 256

struct mapCharChar {
    char key[MAX_MAP_KEY];
    char value[MAX_MAP_VALUE];
    UT_hash_handle hh;
};

struct mapCharInt {
    char key[MAX_MAP_KEY];
    int value;
    UT_hash_handle hh;
};

struct mapIntChar {
    int key;
    char value[MAX_MAP_VALUE];
    UT_hash_handle hh;
};

struct mapIntInt {
    int key;
    int value;
    UT_hash_handle hh;
};

struct mapIntInt64 {
    int key;
    int64_t value;
    UT_hash_handle hh;
};

struct mapIntFloat {
    int key;
    float value;
    UT_hash_handle hh;
};

#define MAP_ADD_STR_STR(pair_key,pair_value,dict,rv)                    \
do {                                                                    \
    if (NULL == pair_key)                                               \
    {                                                                   \
		rv = E_ADV_FILE_HASH_KEY_NULL;                                  \
	}                                                                   \
	else if (strlen(pair_key) > MAX_MAP_KEY)                            \
	{                                                                   \
	    rv = E_ADV_FILE_HASH_KEY_TOO_LONG;	                            \
	}                                                                   \
	else if (NULL != pair_value && strlen(pair_value) > MAX_MAP_VALUE)  \
    {                                                                   \
	    rv = E_ADV_FILE_HASH_VALUE_TOO_LONG;                            \
	}                                                                   \
	else                                                                \
	{                                                                   \
		rv = S_OK;                                                      \
		struct mapCharChar *s;                                          \
		HASH_FIND_STR(dict, pair_key, s);                               \
		if (s==NULL) {                                                  \
		  s = (struct mapCharChar *)malloc(sizeof *s);                  \
		  strcpy(s->key, pair_key);                                     \
		  HASH_ADD_STR(dict, key, s );                                  \
		}                                                               \
        else                                                            \
		{                                                               \
			rv = S_ADV_TAG_REPLACED;                                    \
		}                                                               \
		strcpy(s->value, pair_value);                                   \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_STR_STR(dict)                                          \
do {                                                                    \
	struct mapCharChar *current_item, *tmp;                             \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_INT_INT(dict)                                          \
do {                                                                    \
	struct mapIntInt *current_item, *tmp;                               \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_INT_STR(dict)                                          \
do {                                                                    \
	struct mapIntChar *current_item, *tmp;                              \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_STR_INT(dict)                                          \
do {                                                                    \
	struct mapCharInt *current_item, *tmp;                              \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_INT_INT64(dict)                                        \
do {                                                                    \
	struct mapIntInt64 *current_item, *tmp;                             \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

#define MAP_FREE_INT_FLOAT(dict)                                        \
do {                                                                    \
	struct mapIntFloat *current_item, *tmp;                             \
	HASH_ITER(hh, dict, current_item, tmp) {                            \
	HASH_DEL(dict,current_item);                                        \
	   free(current_item);                                              \
	}                                                                   \
} while (0)                                                             \

struct mapCharChar *m_FileTags = NULL;
struct mapCharChar *m_UserMetadataTags = NULL;
struct mapCharChar *m_MainStreamTags = NULL;
struct mapCharChar *m_CalibrationStreamTags = NULL;

// File
int TotalNumberOfMainFrames;
int TotalNumberOfCalibrationFrames;
unsigned char *m_FrameBytes;
		
// Image Section
#define UNINITIALIZED_LAYOUT_ID 0	
unsigned char m_PreviousLayoutId;
unsigned int m_NumFramesInThisLayoutId;
bool m_ImageSectionSet;

struct mapCharChar *m_ImageTags = NULL;

// TODO:
//map<unsigned char, Adv2ImageLayout*> m_ImageLayouts;

bool m_RGBorBGR;
bool m_SectionDefinitionMode;
int m_MaxImageLayoutFrameBufferSize;
		
unsigned int m_Width;
unsigned int m_Height;
unsigned char m_DataBpp;

enum ImageByteOrder ByteOrder;
bool UsesCRC;
int MaxPixelValue;
bool IsColourImage;
char ImageBayerPattern[128];

// Status Section
int MaxFrameBufferSize;
int64_t UtcTimestampAccuracyInNanoseconds;
int64_t m_UtcStartTimeNanosecondsSinceAdvZeroEpoch;
unsigned int m_UtcExposureNanoseconds;
bool m_FrameStatusLoaded;
bool m_SectionDefinitionMode;

UT_array *m_TagDefinitionNames;

struct mapCharInt *m_TagDefinition = NULL;

struct mapIntChar *m_FrameStatusTags = NULL;
struct mapIntInt *m_FrameStatusTagsUInt8 = NULL;
struct mapIntInt *m_FrameStatusTagsUInt16 = NULL;
struct mapIntInt *m_FrameStatusTagsUInt32 = NULL;
struct mapIntInt64 *m_FrameStatusTagsUInt64 = NULL;
struct mapIntFloat *m_FrameStatusTagsReal = NULL;

// Helper functions
ADVRESULT AdvFile_EndFile();

// Exported functions

DLL_PUBLIC ADVRESULT AdvNewFile(const char* fileName, bool overwriteExisting);
DLL_PUBLIC ADVRESULT AdvSetTicksTimingPrecision(int mainStreamAccuracy, int calibrationStreamAccuracy);
DLL_PUBLIC ADVRESULT AdvDefineExternalClockForMainStream(int64_t clockFrequency, int ticksTimingAccuracy);
DLL_PUBLIC ADVRESULT AdvDefineExternalClockForCalibrationStream(int64_t clockFrequency, int ticksTimingAccuracy);
DLL_PUBLIC ADVRESULT AdvAddMainStreamTag(const char* tagName, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvAddCalibrationStreamTag(const char* tagName, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvDefineImageSection(unsigned short width, unsigned short height, unsigned char dataBpp);
DLL_PUBLIC ADVRESULT AdvDefineImageLayout(unsigned char layoutId, const char* layoutType, const char* compression, unsigned char layoutBpp);
DLL_PUBLIC ADVRESULT AdvDefineStatusSection(int64_t utcTimestampAccuracyInNanoseconds);
DLL_PUBLIC ADVRESULT AdvDefineStatusSectionTag(const char* tagName, int tagType, unsigned int* addedTagId);
DLL_PUBLIC ADVRESULT AdvAddFileTag(const char* tagName, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvAddUserTag(const char* tagName, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvAddOrUpdateImageSectionTag(const char* tagName, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvEndFile();
DLL_PUBLIC ADVRESULT AdvBeginFrameWithTicks(unsigned int streamId, int64_t startFrameTicks, int64_t endFrameTicks, int64_t elapsedTicksSinceFirstFrame, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds);
DLL_PUBLIC ADVRESULT AdvBeginFrame(unsigned int streamId, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds);
DLL_PUBLIC ADVRESULT AdvFrameAddImage(unsigned char layoutId, unsigned short* pixels, unsigned char pixelsBpp);
DLL_PUBLIC ADVRESULT AdvFrameAddImageBytes(unsigned char layoutId, unsigned char* pixels, unsigned char pixelsBpp);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTagUTF8String(unsigned int tagIndex, const char* tagValue);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTagUInt8(unsigned int tagIndex, unsigned char tagValue);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTag16(unsigned int tagIndex, unsigned short tagValue);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTagReal(unsigned int tagIndex, float tagValue);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTag32(unsigned int tagIndex, unsigned int tagValue);
DLL_PUBLIC ADVRESULT AdvFrameAddStatusTag64(unsigned int tagIndex, int64_t tagValue);
DLL_PUBLIC ADVRESULT AdvEndFrame();

DLL_PUBLIC int AdvGetLastSystemSpecificFileError();

DLL_PUBLIC void GetLibraryVersion(char* version);
DLL_PUBLIC void GetLibraryPlatformId( char* platform);
DLL_PUBLIC int GetLibraryBitness();

#endif