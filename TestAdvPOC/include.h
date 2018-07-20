typedef int long long int64_t;
typedef unsigned int long long uint64_t;

typedef int ADVRESULT;
typedef int bool;

#define true 1
#define false 0

extern ADVRESULT AdvNewFile(const char* fileName, bool overwriteExisting);
extern ADVRESULT AdvSetTicksTimingPrecision(int mainStreamAccuracy, int calibrationStreamAccuracy);
extern ADVRESULT AdvDefineExternalClockForMainStream(int64_t clockFrequency, int ticksTimingAccuracy);
extern ADVRESULT AdvDefineExternalClockForCalibrationStream(int64_t clockFrequency, int ticksTimingAccuracy);
extern ADVRESULT AdvAddMainStreamTag(const char* tagName, const char* tagValue);
extern ADVRESULT AdvAddCalibrationStreamTag(const char* tagName, const char* tagValue);
extern ADVRESULT AdvDefineImageSection(unsigned short width, unsigned short height, unsigned char dataBpp);
extern ADVRESULT AdvDefineImageLayout(unsigned char layoutId, const char* layoutType, const char* compression, unsigned char layoutBpp);
extern ADVRESULT AdvDefineStatusSection(int64_t utcTimestampAccuracyInNanoseconds);
extern ADVRESULT AdvDefineStatusSectionTag(const char* tagName, int tagType, unsigned int* addedTagId);
extern ADVRESULT AdvAddFileTag(const char* tagName, const char* tagValue);
extern ADVRESULT AdvAddUserTag(const char* tagName, const char* tagValue);
extern ADVRESULT AdvAddOrUpdateImageSectionTag(const char* tagName, const char* tagValue);
extern ADVRESULT AdvEndFile();
extern ADVRESULT AdvBeginFrameWithTicks(unsigned int streamId, int64_t startFrameTicks, int64_t endFrameTicks, int64_t elapsedTicksSinceFirstFrame, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds);
extern ADVRESULT AdvBeginFrame(unsigned int streamId, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds);
extern ADVRESULT AdvFrameAddImage(unsigned char layoutId, unsigned short* pixels, unsigned char pixelsBpp);
extern ADVRESULT AdvFrameAddImageBytes(unsigned char layoutId, unsigned char* pixels, unsigned char pixelsBpp);
extern ADVRESULT AdvFrameAddStatusTagUTF8String(unsigned int tagIndex, const char* tagValue);
extern ADVRESULT AdvFrameAddStatusTagUInt8(unsigned int tagIndex, unsigned char tagValue);
extern ADVRESULT AdvFrameAddStatusTag16(unsigned int tagIndex, unsigned short tagValue);
extern ADVRESULT AdvFrameAddStatusTagReal(unsigned int tagIndex, float tagValue);
extern ADVRESULT AdvFrameAddStatusTag32(unsigned int tagIndex, unsigned int tagValue);
extern ADVRESULT AdvFrameAddStatusTag64(unsigned int tagIndex, int64_t tagValue);
extern ADVRESULT AdvEndFrame();

extern int AdvGetLastSystemSpecificFileError();

extern void GetLibraryVersion(char* version);
extern void GetLibraryPlatformId( char* platform);
extern int GetLibraryBitness();

unsigned int MILLI_TO_NANO = 1000000;

enum Adv2TagType
{
	Int8 = 0,
	Int16 = 1,
	Int32 = 2,
	Long64 = 3,
	Real4 = 4,
	UTF8String = 5
};

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

#define S_OK 0
#define E_FAIL ((ADVRESULT)0x80004005L)
#define E_NOTIMPL ((ADVRESULT)0x80004001L)