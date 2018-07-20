#include <stdio.h>

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

#include "..\uthash.h"

enum Adv2TagType
{
	Int8 = 0,
	Int16 = 1,
	Int32 = 2,
	Long64 = 3,
	Real4 = 4,
	UTF8String = 5
};

struct my_struct {
    int id;                    /* key */
    char name[10];
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct mapCharChar {
    char key[64];
    char value[264];
    UT_hash_handle hh;  /* makes this structure hashable */
};

struct my_struct *users = NULL;
struct mapCharChar *tags = NULL;

void add_user(int user_id, char *name) {
    struct my_struct *s;

    s = malloc(sizeof(struct my_struct));
    s->id = user_id;
    strcpy(s->name, name);
    HASH_ADD_INT( users, id, s );  /* id: name of key field */
}

void add_update_user(int user_id, char *name) {
    struct my_struct *s;
    HASH_FIND_INT(users, &user_id, s);  /* id already in the hash? */
    if (s==NULL) {
      s = (struct my_struct *)malloc(sizeof *s);
      s->id = user_id;
      HASH_ADD_INT( users, id, s );  /* id: name of key field */
    }
    strcpy(s->name, name);
}

void add_update_tag(char *key, char *value) {
    struct mapCharChar *s;
    HASH_FIND_STR(tags, key, s);  /* id already in the hash? */
    if (s==NULL) {
      s = (struct mapCharChar *)malloc(sizeof *s);
      strcpy(s->key, key);
      HASH_ADD_STR( tags, key, s );  /* id: name of key field */
    }
    strcpy(s->value, value);
}

void TestIntCharMap()
{
	add_user(1, "user1");
	add_update_user(1, "user2");
	
	struct my_struct *user;

    int user_id = 1;
    HASH_FIND_INT( users, &user_id, user);  /* s: output pointer */
	
    HASH_DEL(users, user);  /* user: pointer to deletee */
    free(user);             /* optional; it's up to you! */
}

void TestCharCharMap()
{
	add_update_tag("123", "user1");
	add_update_tag("123", "user2");
	
	struct mapCharChar *tag;

    char *tag_id = "123";
    HASH_FIND_STR( tags, tag_id, tag);  /* s: output pointer */
	
	struct mapCharChar *current_tag, *tmp;

	HASH_ITER(hh, tags, current_tag, tmp) {
	HASH_DEL(tags,current_tag);  /* delete; users advances to next */
	   free(current_tag);            /* optional- if you want to free  */
	}
	
    //HASH_DEL(tags, tag);  /* user: pointer to deletee */
    //free(tag);             /* optional; it's up to you! */
	
	HASH_FIND_STR( tags, tag_id, tag);  /* s: output pointer */
}

#include "..\utarray.h" // https://troydhanson.github.io/utarray/

void TestArray()
{
	UT_array *strs;
	char *s, **p;

	utarray_new(strs,&ut_str_icd);

	s = "hello"; utarray_push_back(strs, &s);
	s = "world"; utarray_push_back(strs, &s);
	p = NULL;
	while ( (p=(char**)utarray_next(strs,p))) {
		printf("%s\n",*p);
	}

	utarray_free(strs);	
}

int main(int argc, char **argv)
{
	//TestCharCharMap();
	//TestArray();
	
	// TODO: Use CuTest framework to write unit tests
	AdvNewFile("C:\\Work\\ADV Version 2\\AdvPOC\\Debug\\test.adv", true);
	AdvAddFileTag("FSTF-TYPE", "ADV");
	AdvAddFileTag("ADV-VERSION", "1");
	AdvAddFileTag("ADV-VERSION", "2");
	AdvDefineImageSection(800, 600, 16);
	AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "UNCOMPRESSED", 16);
	AdvDefineStatusSection(1 * MILLI_TO_NANO /* 1ms */);
	unsigned int tagId;
	AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
	AdvDefineStatusSectionTag("Tag2", Real4, &tagId);
	AdvDefineStatusSectionTag("Tag2", UTF8String, &tagId);
	
	AdvEndFile();
	
	printf("hello world\n");
	return 0;
}
