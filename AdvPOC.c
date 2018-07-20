#include "AdvPOC.h"
#include "cross_platform.h"


char* g_CurrentAdvFile;
FILE* g_AdvFile;
bool g_FileStarted;

ADVRESULT AdvNewFile(const char* fileName, bool overwriteExisting)
{
	if (!fileName)
		return E_FAIL;
	
	if (g_CurrentAdvFile)
	{
		free(g_CurrentAdvFile);
		g_CurrentAdvFile = NULL;
	}
	
	if (g_AdvFile != 0)
	{
		advfclose(g_AdvFile);
		g_AdvFile = 0;
	}
	
	g_FileStarted = false;
	
	ADVRESULT rv = CheckFileName(fileName, !overwriteExisting);
	if (rv != S_OK)
		return rv;

	int len = (int)strlen(fileName);
	if (len > 0)
	{
		g_CurrentAdvFile = (char*)malloc(len + 1);
		strncpy_s(g_CurrentAdvFile, len + 1, fileName, len + 1);
		
		g_AdvFile = advfopen(fileName, "wb");
		if (g_AdvFile == 0) return E_ADV_IO_ERROR;
		
		m_ImageSectionSet = false;
		
		m_FrameBytes = NULL;
		// TODO:
		//ImageSection = nullptr;
		//StatusSection = nullptr;
		//m_Index = nullptr;

		m_NumberOfMainFrames = 0;
		m_NumberOfCalibrationFrames = 0;
		m_UsesExternalMainStreamClock = false;
		m_UsesExternalCalibrationStreamClock = false;
		TotalNumberOfMainFrames = 0;
		TotalNumberOfCalibrationFrames = 0;

		m_ImageAdded = false;
		m_FrameStarted = false;
		m_LastSystemSpecificFileError = 0;
		m_FileDefinitionMode = true;		
	}

	return S_OK;
}

ADVRESULT AdvSetTicksTimingPrecision(int mainStreamAccuracy, int calibrationStreamAccuracy)
{	
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;
		
	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	m_MainStreamTickAccuracy = mainStreamAccuracy;
	m_CalibrationStreamTickAccuracy = calibrationStreamAccuracy;

	return S_OK;
}

ADVRESULT AdvDefineExternalClockForMainStream(int64_t clockFrequency, int ticksTimingAccuracy)
{
	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	m_UsesExternalMainStreamClock = true;
	m_MainStreamClockFrequency = clockFrequency;
	m_MainStreamTickAccuracy = ticksTimingAccuracy;

	return S_OK;
}

ADVRESULT AdvDefineExternalClockForCalibrationStream(int64_t clockFrequency, int ticksTimingAccuracy)
{
	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	m_UsesExternalCalibrationStreamClock = true;
	m_CalibrationStreamClockFrequency = clockFrequency;
	m_CalibrationStreamTickAccuracy = ticksTimingAccuracy;

	return S_OK;
}

ADVRESULT AdvAddMainStreamTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;
		
	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

    ADVRESULT rv = S_OK;
	MAP_ADD_STR_STR(tagName, tagValue, m_MainStreamTags, rv);
	return rv;
}

ADVRESULT AdvAddCalibrationStreamTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;
		
	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;
	
	ADVRESULT rv = S_OK;	
	MAP_ADD_STR_STR(tagName, tagValue, m_CalibrationStreamTags, rv);
	return rv;
}

ADVRESULT AdvDefineImageSection(unsigned short width, unsigned short height, unsigned char dataBpp)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	m_Width = width;
	m_Height = height;
	m_DataBpp = dataBpp;
	
	m_PreviousLayoutId = UNINITIALIZED_LAYOUT_ID;
	m_NumFramesInThisLayoutId = 0;
	ByteOrder = LittleEndian;
	UsesCRC = false;
	MaxPixelValue = 0;
	m_RGBorBGR = false;
	m_SectionDefinitionMode = true;
	m_MaxImageLayoutFrameBufferSize = -1;

	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	if (m_ImageSectionSet)
		return E_ADV_IMAGE_SECTION_ALREADY_DEFINED;

	m_ImageSectionSet = true;

	char convStr [10];
	_snprintf_s(convStr, 10, "%d", m_Width);
	
	ADVRESULT rv = S_OK;
	MAP_ADD_STR_STR("WIDTH", convStr, m_FileTags, rv);
	
	_snprintf_s(convStr, 10, "%d", m_Height);
	MAP_ADD_STR_STR("HEIGHT", convStr, m_FileTags, rv);
	
	_snprintf_s(convStr, 10, "%d", m_DataBpp);
	MAP_ADD_STR_STR("BITPIX", convStr, m_FileTags, rv);

	return S_OK;
}

ADVRESULT AdvDefineImageLayout(unsigned char layoutId, const char* layoutType, const char* compression, unsigned char layoutBpp)
{
	return E_NOTIMPL;
}

ADVRESULT AdvDefineStatusSection(int64_t utcTimestampAccuracyInNanoseconds)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	MaxFrameBufferSize = 0;
	UtcTimestampAccuracyInNanoseconds = utcTimestampAccuracyInNanoseconds;

    utarray_new(m_TagDefinitionNames, &ut_str_icd);

	m_FrameStatusLoaded = false;
	m_SectionDefinitionMode = true;
	
	return S_OK;
}

void AdvUpdateMaxBuffSize(int tagType, int direction)
{
	switch(tagType)
	{
		case Int8:
			MaxFrameBufferSize+=1*direction;
			break;
			
		case Int16:
			MaxFrameBufferSize+=2*direction;
			break;

		case Int32:
			MaxFrameBufferSize+=4*direction;
			break;
			
		case Long64:
			MaxFrameBufferSize+=8*direction;
			break;			
			
		case Real4:
			MaxFrameBufferSize+=4*direction;
			break;	
			
		case UTF8String:
			MaxFrameBufferSize+=0x10001*direction;
			break;
	}	
}

ADVRESULT AdvDefineStatusSectionTag(const char* tagName, int tagType, unsigned int* addedTagId)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_TagDefinitionNames)
		return E_ADV_STATUS_SECTION_UNDEFINED;

	if (!m_SectionDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	
	ADVRESULT rv = S_OK;
	
	char **p = NULL;
	int index = 0;
	bool found = false;
	while ( (p=(char**)utarray_next(m_TagDefinitionNames,p))) {
       if (strcmp(*p, tagName) == 0) 
	   {
		   found = true;
		   break;
	   }
	   index++;
    }
	 
	if (found)
	{
		// Update existing
		struct mapCharInt *s;
		HASH_FIND_STR(m_TagDefinition, tagName, s);
		AdvUpdateMaxBuffSize(s->value, -1);
		s->value = tagType;
		AdvUpdateMaxBuffSize(tagType, 1);
		rv = S_ADV_TAG_REPLACED;
	}
	else
	{
		// Add new 
	   struct mapCharInt *s;
	   s = (struct mapCharInt *)malloc(sizeof *s); 
	   strcpy(s->key, tagName);
	   s->value = tagType;
	   HASH_ADD_STR(m_TagDefinition, key, s );
	   utarray_push_back(m_TagDefinitionNames, &tagName);
	   AdvUpdateMaxBuffSize(tagType, 1);
	}  

    *addedTagId = index;
	return S_OK;
}

ADVRESULT AdvAddFileTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

    ADVRESULT rv = S_OK;
    MAP_ADD_STR_STR(tagName, tagValue, m_FileTags, rv);
	return rv;
}

ADVRESULT AdvAddUserTag(const char* tagName, const char* tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvAddOrUpdateImageSectionTag(const char* tagName, const char* tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFile_EndFile()
{
	if (g_FileStarted == false)
		return E_ADV_FILE_NOT_OPEN;

    /* TODO
	int64_t indexTableOffset;
	advfgetpos64(m_AdvFile, &indexTableOffset);
	
	// TODO:
	//if (nullptr != m_Index)
	//	m_Index->WriteIndex(m_Adv2File);
		
	int64_t userMetaTableOffset;
	advfgetpos64(m_AdvFile, &userMetaTableOffset);

	advfseek(m_AdvFile, m_MainFrameCountPosition, SEEK_SET);
	advfwrite(&m_MainFrameNo, 4, 1, m_AdvFile);

	advfseek(m_AdvFile, m_CalibrationFrameCountPosition, SEEK_SET);
	advfwrite(&m_CalibrationFrameNo, 4, 1, m_AdvFile);

	advfseek(m_AdvFile, 9, SEEK_SET);
	advfwrite(&indexTableOffset, 8, 1, m_AdvFile);
	advfseek(m_AdvFile, 0x19, SEEK_SET);	
	advfwrite(&userMetaTableOffset, 8, 1, m_AdvFile);
		
	// Write the metadata table
	advfseek(m_AdvFile, 0, SEEK_END);

	unsigned int userTagsCount = (unsigned int)m_UserMetadataTags.size();
	advfwrite(&userTagsCount, 4, 1, m_AdvFile);
	
	map<string, string>::iterator curr = m_UserMetadataTags.begin();
	while (curr != m_UserMetadataTags.end()) 
	{
		char* userTagName = const_cast<char*>(curr->first.c_str());	
		WriteUTF8String(m_AdvFile, userTagName);
		
		char* userTagValue = const_cast<char*>(curr->second.c_str());	
		WriteUTF8String(m_AdvFile, userTagValue);
		
		curr++;
	}
		
	advfflush(m_AdvFile);
	*/
	
	advfclose(g_AdvFile);
	
	g_AdvFile = 0;

	return S_OK;	
}

ADVRESULT AdvEndFile()
{
	ADVRESULT rv = S_OK;

	if (g_AdvFile != 0)
		rv = AdvFile_EndFile();
	else
		rv = E_ADV_NOFILE;
	
	if (g_CurrentAdvFile)
	{
		free(g_CurrentAdvFile);
		g_CurrentAdvFile = NULL;
	}
	
	// File
	MAP_FREE_STR_STR(m_FileTags);
	MAP_FREE_STR_STR(m_ImageTags);
	MAP_FREE_STR_STR(m_UserMetadataTags);
	MAP_FREE_STR_STR(m_MainStreamTags);
	MAP_FREE_STR_STR(m_CalibrationStreamTags);
	
	// Status section
	utarray_free(m_TagDefinitionNames);
	MAP_FREE_STR_INT(m_TagDefinition);
	MAP_FREE_INT_STR(m_FrameStatusTags);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt8);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt16);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt32);
	MAP_FREE_INT_INT64(m_FrameStatusTagsUInt64);
	MAP_FREE_INT_FLOAT(m_FrameStatusTagsReal);
	
	g_FileStarted = false;
	return rv;
}

ADVRESULT AdvBeginFrameWithTicks(unsigned int streamId, int64_t startFrameTicks, int64_t endFrameTicks, int64_t elapsedTicksSinceFirstFrame, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds)
{
	return E_NOTIMPL;
}

ADVRESULT AdvBeginFrame(unsigned int streamId, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddImage(unsigned char layoutId, unsigned short* pixels, unsigned char pixelsBpp)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddImageBytes(unsigned char layoutId, unsigned char* pixels, unsigned char pixelsBpp)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddStatusTagUTF8String(unsigned int tagIndex, const char* tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddStatusTagUInt8(unsigned int tagIndex, unsigned char tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddStatusTag16(unsigned int tagIndex, unsigned short tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddStatusTagReal(unsigned int tagIndex, float tagValue)
{
	return E_NOTIMPL;
	
}

ADVRESULT AdvFrameAddStatusTag32(unsigned int tagIndex, unsigned int tagValue)
{
	return E_NOTIMPL;
}

ADVRESULT AdvFrameAddStatusTag64(unsigned int tagIndex, int64_t tagValue)
{
	return E_NOTIMPL;	
}

ADVRESULT AdvEndFrame()
{
	return E_NOTIMPL;
}

int AdvGetLastSystemSpecificFileError()
{
	
}

void GetLibraryVersion(char* version)
{
	
}

void GetLibraryPlatformId( char* platform)
{
	
}

int GetLibraryBitness()
{
	
}