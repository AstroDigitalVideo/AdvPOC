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
		if (g_AdvFile == 0) {
			m_LastSystemSpecificFileError = errno;
			return E_ADV_IO_ERROR;
		}

		m_ImageSectionSet = false;
		
		m_FrameBytes = NULL;

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
		m_ImageSectionSet = false;
		m_StatusSectionSet = false;
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

    if (NULL == tagValue)
    {
        return E_ADV_FILE_HASH_KEY_NULL;
	}
    
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
	
    if (NULL == tagValue)
    {
        return E_ADV_FILE_HASH_KEY_NULL;
	}
    
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
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;
		
	if (!m_ImageSectionSet)
		return E_ADV_IMAGE_SECTION_UNDEFINED;
		
	if (!m_SectionDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

    struct mapIntImageLayout *s;
	int key = layoutId;
    HASH_FIND_INT(m_ImageLayouts, &key, s);
	
	if (s)
		return E_ADV_IMAGE_LAYOUT_ALREADY_DEFINED;

	if (layoutType == NULL)
		return E_ADV_INVALID_IMAGE_LAYOUT_TYPE;

	if (strcmp(layoutType, "FULL-IMAGE-RAW") != 0 &&
		strcmp(layoutType, "12BIT-IMAGE-PACKED") != 0 &&
		strcmp(layoutType, "8BIT-COLOR-IMAGE") != 0)
	{
		return E_ADV_INVALID_IMAGE_LAYOUT_TYPE;
	}

	if (compression == NULL)
		return E_ADV_INVALID_IMAGE_LAYOUT_COMPRESSION;

	if (strcmp(compression, "UNCOMPRESSED") != 0 &&
		strcmp(compression, "LAGARITH16") != 0 &&
		strcmp(compression, "QUICKLZ") != 0)
	{
		return E_ADV_INVALID_IMAGE_LAYOUT_COMPRESSION;
	}

	if (layoutBpp == 0 || layoutBpp > 32)
		return E_ADV_INVALID_IMAGE_LAYOUT_BPP;

    ADVRESULT rv = S_OK;
    MAP_ADD_INT_IMG_LAYOUT(key, layoutType, compression, layoutBpp, m_ImageLayouts, rv);
	
	return rv;
}

ADVRESULT AdvDefineStatusSection(int64_t utcTimestampAccuracyInNanoseconds)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	StatusSectionMaxFrameBufferSize = 0;
	UtcTimestampAccuracyInNanoseconds = utcTimestampAccuracyInNanoseconds;

    utarray_new(m_TagDefinitionNames, &ut_str_icd);

	m_FrameStatusLoaded = false;
	m_SectionDefinitionMode = true;
	m_StatusSectionSet = true;
	
	return S_OK;
}

void AdvUpdateMaxBuffSize(int tagType, int direction)
{
	switch(tagType)
	{
		case Int8:
			StatusSectionMaxFrameBufferSize+=1*direction;
			break;
			
		case Int16:
			StatusSectionMaxFrameBufferSize+=2*direction;
			break;

		case Int32:
			StatusSectionMaxFrameBufferSize+=4*direction;
			break;
			
		case Long64:
			StatusSectionMaxFrameBufferSize+=8*direction;
			break;
			
		case Real4:
			StatusSectionMaxFrameBufferSize+=4*direction;
			break;	
			
		case UTF8String:
			StatusSectionMaxFrameBufferSize+=0x10001*direction;
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
	return rv;
}

ADVRESULT AdvAddFileTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

    if (NULL == tagValue)
    {
        return E_ADV_FILE_HASH_KEY_NULL;
	}

	ADVRESULT rv = S_OK;
	MAP_ADD_STR_STR(tagName, tagValue, m_FileTags, rv);
	return rv;
}

ADVRESULT AdvAddUserTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_FileDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

    if (NULL == tagValue)
    {
        return E_ADV_FILE_HASH_KEY_NULL;
	}

	ADVRESULT rv = S_OK;
	MAP_ADD_STR_STR(tagName, tagValue, m_UserMetadataTags, rv);
	return rv;
}

ADVRESULT AdvAddImageSectionTag(const char* tagName, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_ImageSectionSet)
		return E_ADV_IMAGE_SECTION_UNDEFINED;
	
	if (!m_SectionDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	if (strcmp("IMAGE-BYTE-ORDER", tagName) == 0)
	{
		ByteOrder = LittleEndian;

		if (strcmp("BIG-ENDIAN", tagValue) == 0)
			ByteOrder = BigEndian;
	}

	if (strcmp("SECTION-DATA-REDUNDANCY-CHECK", tagName) == 0)
	{
		UsesCRC = strcmp("CRC32", tagValue) == 0;
	}

	if (strcmp("IMAGE-MAX-PIXEL-VALUE", tagName) == 0 && tagValue != NULL)
	{
		MaxPixelValue = atoi(tagValue);
	}

	if (strcmp("IMAGE-BAYER-PATTERN", tagName) == 0 && tagValue != NULL)
	{
		m_RGBorBGR = strcmp("RGB", tagValue) == 0 || strcmp("BGR", tagValue) == 0;
		IsColourImage = strcmp("MONOCHROME", tagValue) != 0;
		strcpy_s(ImageBayerPattern, strlen(tagValue) + 1, tagValue);
	}

	ADVRESULT rv = S_OK;
	MAP_ADD_STR_STR(tagName, tagValue, m_ImageTags, rv);
	return rv;
}

ADVRESULT AdvFile_EndFile()
{
	if (!g_FileStarted)
		return E_ADV_FILE_NOT_OPEN;

    /* TODO
	int64_t indexTableOffset;
	advfgetpos64(m_AdvFile, &indexTableOffset);
	
	// TODO:
	//if (nullptr != m_Index)
	//	m_Index->WriteIndex(g_AdvFile);
		
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
    {
		rv = AdvFile_EndFile();

        if (g_AdvFile != 0)
        {
            advfclose(g_AdvFile);
            g_AdvFile = 0;
        }
    }
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
    if (m_StatusSectionSet) utarray_free(m_TagDefinitionNames);
	MAP_FREE_STR_INT(m_TagDefinition);
	MAP_FREE_INT_STR(m_FrameStatusTags);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt8);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt16);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt32);
	MAP_FREE_INT_INT64(m_FrameStatusTagsUInt64);
	MAP_FREE_INT_FLOAT(m_FrameStatusTagsReal);
	
	// Image section
	MAP_FREE_INT_IMG_LAYOUT(m_ImageLayouts);

	if (g_FileStarted)
	{
		utarray_free(m_MainIndex);
		utarray_free(m_CalibrationIndex);
	}
	
	g_FileStarted = false;
	
	return rv;
}

void WriteImageSectionHeader(FILE* pFile)
{
	unsigned char buffChar;
	
	buffChar = 2;
	advfwrite(&buffChar, 1, 1, pFile); /* Version */

	
	advfwrite(&m_Width, 4, 1, pFile);
	advfwrite(&m_Height, 4, 1, pFile);
	advfwrite(&m_DataBpp, 1, 1, pFile);
	
	buffChar = (unsigned char)HASH_COUNT(m_ImageLayouts);
	advfwrite(&buffChar, 1, 1, pFile);
	
	struct mapIntImageLayout *currIml;

    for(currIml=m_ImageLayouts; currIml != NULL; currIml=currIml->hh.next) 
	{
		char layoutId = currIml->key;	
		advfwrite(&layoutId, 1, 1, pFile);

		buffChar = 2;
		advfwrite(&buffChar, 1, 1, pFile); /* Version */

		buffChar = currIml->layoutBpp;
		advfwrite(&buffChar, 1, 1, pFile);

		buffChar = 2;
		advfwrite(&buffChar, 1, 1, pFile);
		
		WriteUTF8String(pFile, "DATA-LAYOUT");
		WriteUTF8String(pFile, currIml->layoutType);
		
		WriteUTF8String(pFile, "SECTION-DATA-COMPRESSION");
		WriteUTF8String(pFile, currIml->compression);
    }
		
	buffChar = (unsigned char)HASH_COUNT(m_ImageTags);
	advfwrite(&buffChar, 1, 1, pFile);

	struct mapCharChar *curr;

	for(curr=m_ImageTags; curr != NULL; curr=curr->hh.next) {
		WriteUTF8String(pFile, curr->key);
		WriteUTF8String(pFile, curr->value);
	}		
}

void WriteStatusSectionHeader(FILE* pFile)
{
	unsigned char buffChar;
	
	buffChar = 2;
	advfwrite(&buffChar, 1, 1, pFile); /* Version */
	
	advfwrite(&UtcTimestampAccuracyInNanoseconds, 8, 1, pFile);

	buffChar = (unsigned char)utarray_len(m_TagDefinitionNames);
	advfwrite(&buffChar, 1, 1, pFile);
	int tagCount = buffChar;
	int tagIndex;
	for(tagIndex = 0; tagIndex<tagCount; tagIndex++)
	{
		char** tagName = (char**)utarray_eltptr(m_TagDefinitionNames, tagIndex);
		
		WriteUTF8String(pFile, *tagName);
		
		struct mapCharInt *currDef;

		HASH_FIND_STR( m_TagDefinition, *tagName, currDef );

		buffChar = (unsigned char)(currDef->value);
		advfwrite(&buffChar, 1, 1, pFile);
	}

	m_SectionDefinitionMode = false;	
}

unsigned char CURRENT_DATAFORMAT_VERSION = 2;

ADVRESULT AdvBeginFile()
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;
	
	if (!m_ImageSectionSet)
		return E_ADV_IMAGE_SECTION_UNDEFINED;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
			
	if (HASH_COUNT(m_ImageLayouts) == 0)
		return E_ADV_IMAGE_LAYOUTS_UNDEFINED;		

	unsigned int buffInt;
	int64_t buffLong;
	unsigned char buffChar;
	
	buffInt = 0x46545346;
	advfwrite(&buffInt, 4, 1, g_AdvFile);
	advfwrite(&CURRENT_DATAFORMAT_VERSION, 1, 1, g_AdvFile);

	buffInt = 0;
	buffLong = 0;
	advfwrite(&buffInt, 4, 1, g_AdvFile); // 0x00000000 (Reserved)
	advfwrite(&buffLong, 8, 1, g_AdvFile); // Offset of index table (will be saved later) 
	advfwrite(&buffLong, 8, 1, g_AdvFile); // Offset of system metadata table (will be saved later) 
	advfwrite(&buffLong, 8, 1, g_AdvFile); // Offset of user metadata table (will be saved later) 

	buffChar = (unsigned char)2;
	advfwrite(&buffChar, 1, 1, g_AdvFile); // Number of streams (main and calibration) 
	
	int64_t streamHeaderOffsetPositions[2];
	int64_t streamHeaderOffsets[2];

	int64_t internalFrequency = advgetclockresolution();
	if (!m_UsesExternalMainStreamClock)
	{
		m_MainStreamClockFrequency = internalFrequency;
		m_MainStreamTickAccuracy = 0; // Unknown accuracy as it is 'automatically' timestamped at frame save time
	}

	WriteUTF8String(g_AdvFile, "MAIN");
	advfgetpos64(g_AdvFile, &m_MainFrameCountPosition);
	buffInt = 0;
	advfwrite(&buffInt, 4, 1, g_AdvFile); // Number of frames saved in the Main stream
	buffLong = m_MainStreamClockFrequency;
	advfwrite(&buffLong, 8, 1, g_AdvFile);
	buffInt = m_MainStreamTickAccuracy;
	advfwrite(&buffInt, 4, 1, g_AdvFile);
	advfgetpos64(g_AdvFile, &streamHeaderOffsetPositions[0]);
	buffLong = 0;
	advfwrite(&buffLong, 8, 1, g_AdvFile); // Offset of main stream metadata table (will be saved later) 

	if (!m_UsesExternalCalibrationStreamClock)
	{
		m_CalibrationStreamClockFrequency = internalFrequency;
		m_CalibrationStreamTickAccuracy = 0; // Unknown accuracy as it is 'automatically' timestamped at frame save time
	}

	WriteUTF8String(g_AdvFile, "CALIBRATION");
	advfgetpos64(g_AdvFile, &m_CalibrationFrameCountPosition);
	buffInt = 0;
	advfwrite(&buffInt, 4, 1, g_AdvFile); // Number of frames saved in the Calibration stream
	buffLong = m_CalibrationStreamClockFrequency;
	advfwrite(&buffLong, 8, 1, g_AdvFile);
	buffInt = m_CalibrationStreamTickAccuracy;
	advfwrite(&buffInt, 4, 1, g_AdvFile);
	advfgetpos64(g_AdvFile, &streamHeaderOffsetPositions[1]);
	buffLong = 0;
	advfwrite(&buffLong, 8, 1, g_AdvFile); // Offset of Calibration stream metadata table (will be saved later) 

	buffChar = (unsigned char)2;
	advfwrite(&buffChar, 1, 1, g_AdvFile); // Number of sections (image and status) 

	int64_t sectionHeaderOffsetPositions[2];
	
	WriteUTF8String(g_AdvFile, "IMAGE");
	advfgetpos64(g_AdvFile, &sectionHeaderOffsetPositions[0]);
	buffLong = 0;
	advfwrite(&buffLong, 8, 1, g_AdvFile);
	
	WriteUTF8String(g_AdvFile, "STATUS");
	advfgetpos64(g_AdvFile, &sectionHeaderOffsetPositions[1]);
	buffLong = 0;
	advfwrite(&buffLong, 8, 1, g_AdvFile);
	
	// Write main stream metadata table
	unsigned char mainTagsCount = (unsigned char)HASH_COUNT(m_MainStreamTags);
	advfgetpos64(g_AdvFile, &streamHeaderOffsets[0]);
	
	advfwrite(&mainTagsCount, 1, 1, g_AdvFile);
	
	struct mapCharChar *curr;

    for(curr=m_MainStreamTags; curr != NULL; curr=curr->hh.next) {
		WriteUTF8String(g_AdvFile, curr->key);
		WriteUTF8String(g_AdvFile, curr->value);
    }

	// Write calibration stream metadata table
	unsigned char calibrationTagsCount = (unsigned char)HASH_COUNT(m_CalibrationStreamTags);
	advfgetpos64(g_AdvFile, &streamHeaderOffsets[1]);
	
	advfwrite(&calibrationTagsCount, 1, 1, g_AdvFile);

	for(curr=m_CalibrationStreamTags; curr != NULL; curr=curr->hh.next) {
		WriteUTF8String(g_AdvFile, curr->key);
		WriteUTF8String(g_AdvFile, curr->value);
    }

	advfsetpos64(g_AdvFile, &streamHeaderOffsetPositions[0]);
	advfwrite(&streamHeaderOffsets[0], 8, 1, g_AdvFile);
	
	advfsetpos64(g_AdvFile, &streamHeaderOffsetPositions[1]);
	advfwrite(&streamHeaderOffsets[1], 8, 1, g_AdvFile);

	advfseek(g_AdvFile, 0, SEEK_END);
	
	// Write section headers
	int64_t sectionHeaderOffsets[2];
	advfgetpos64(g_AdvFile, &sectionHeaderOffsets[0]);
	WriteImageSectionHeader(g_AdvFile);
	
	advfgetpos64(g_AdvFile, &sectionHeaderOffsets[1]);
	WriteStatusSectionHeader(g_AdvFile);

	// Write section headers positions
	advfsetpos64(g_AdvFile, &sectionHeaderOffsetPositions[0]);
	advfwrite(&sectionHeaderOffsets[0], 8, 1, g_AdvFile);
	advfsetpos64(g_AdvFile, &sectionHeaderOffsetPositions[1]);
	advfwrite(&sectionHeaderOffsets[1], 8, 1, g_AdvFile);
	
	advfseek(g_AdvFile, 0, SEEK_END);

	// Write system metadata table
	int64_t systemMetadataTablePosition;
	advfgetpos64(g_AdvFile, &systemMetadataTablePosition);
	
	unsigned int fileTagsCount = (unsigned int)HASH_COUNT(m_FileTags);
	advfwrite(&fileTagsCount, 4, 1, g_AdvFile);
	
	for(curr=m_FileTags; curr != NULL; curr=curr->hh.next) {
		WriteUTF8String(g_AdvFile, curr->key);
		WriteUTF8String(g_AdvFile, curr->value);
    }
	
	// Write system metadata table position to the file header
	advfseek(g_AdvFile, 0x11, SEEK_SET);
	advfwrite(&systemMetadataTablePosition, 8, 1, g_AdvFile);
	
	advfseek(g_AdvFile, 0, SEEK_END);
	
	utarray_new(m_MainIndex, &ut_str_icd);
	utarray_new(m_CalibrationIndex, &ut_str_icd);
	
	advfflush(g_AdvFile);
		
	m_MainFrameNo = 0;
	m_CalibrationFrameNo = 0;
    m_FileDefinitionMode = false;
	
	return S_OK;
}

int GetLayoutMaxFrameBufferSize(struct mapIntImageLayout *curr)
{
	if (curr->layoutBpp == 8)
	{
		return m_Width * m_Height + 1 + 4 + 16;
	}
	else if (curr->layoutBpp == 12)
	{
		return (m_Width * m_Height * 3 / 2) + 1 + 4 + 2 * ((m_Width * m_Height) % 2) + 16;
	}
	else if (curr->layoutBpp == 16)
	{
		return (m_Width * m_Height * 2) + 1 + 4 + 16;
	}
	else 
		return m_Width * m_Height * 4 + 1 + 4 + 16;	
}

ADVRESULT ImageSectionMaxFrameBufferSize(int* maxImageBuffer)
{
	if (m_SectionDefinitionMode)
		return E_ADV_CHANGE_NOT_ALLOWED_RIGHT_NOW;

	// Max frame buffer size is the max frame buffer size of the largest image layout
	if (m_MaxImageLayoutFrameBufferSize == -1)
	{
		struct mapIntImageLayout *curr;

		for(curr=m_ImageLayouts; curr != NULL; curr=curr->hh.next) {
			int maxBuffSize = GetLayoutMaxFrameBufferSize(curr);
				
			if (m_MaxImageLayoutFrameBufferSize < maxBuffSize) 
				m_MaxImageLayoutFrameBufferSize = maxBuffSize;
		}			
	}
		
	*maxImageBuffer = m_MaxImageLayoutFrameBufferSize;
	return S_OK;	
}

int IndexGetFramesCount(unsigned int streamId)
{
	if (streamId == 0)
		return (unsigned int)utarray_len(m_MainIndex);
	else
		return (unsigned int)utarray_len(m_CalibrationIndex);
}

ADVRESULT ImageSectionBeginFrame()
{
	if (HASH_COUNT(m_ImageLayouts) == 0)
		return E_ADV_IMAGE_LAYOUTS_UNDEFINED;

	m_SectionDefinitionMode = false;

	return S_OK;
}

ADVRESULT StatusSectionBeginFrame(int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds)
{
	MAP_FREE_INT_STR(m_FrameStatusTags);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt8);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt16);
	MAP_FREE_INT_INT(m_FrameStatusTagsUInt32);
	MAP_FREE_INT_INT64(m_FrameStatusTagsUInt64);
	MAP_FREE_INT_FLOAT(m_FrameStatusTagsReal);	

	m_UtcStartTimeNanosecondsSinceAdvZeroEpoch = utcStartTimeNanosecondsSinceAdvZeroEpoch;
	m_UtcExposureNanoseconds = utcExposureNanoseconds;

	m_SectionDefinitionMode = false;

	return S_OK;	
}

ADVRESULT AdvBeginFrame(unsigned int streamId, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds)
{
	ADVRESULT rv = S_OK;
	
	if (!g_FileStarted)
	{
		rv = AdvBeginFile();
		if (rv == S_OK)
		{
			g_FileStarted = true;
		}
		else
		{
			g_FileStarted = false;
			return rv;
		}
	}

	if (streamId != 0 && streamId != 1)
		return E_ADV_INVALID_STREAM_ID;

	int64_t endFrameTicks = advgetclockticks();
	int64_t startFrameTicks = endFrameTicks;
	int64_t elapsedTicksSinceFirstFrame = 0;
	if (IndexGetFramesCount(streamId) > 0)
	{
		startFrameTicks = m_PrevFrameInStreamTicks[streamId];
		elapsedTicksSinceFirstFrame = endFrameTicks - m_FirstFrameInStreamTicks[streamId];
	}
	
	return AdvBeginFrameWithTicks(streamId, startFrameTicks, endFrameTicks, elapsedTicksSinceFirstFrame, utcStartTimeNanosecondsSinceAdvZeroEpoch, utcExposureNanoseconds);
}

ADVRESULT AdvBeginFrameWithTicks(unsigned int streamId, int64_t startFrameTicks, int64_t endFrameTicks, int64_t elapsedTicksSinceFirstFrame, int64_t utcStartTimeNanosecondsSinceAdvZeroEpoch, unsigned int utcExposureNanoseconds)
{
	ADVRESULT rv = S_OK;
	
	if (!g_FileStarted)
	{
		rv = AdvBeginFile();
		if (rv == S_OK)
		{
			g_FileStarted = true;
		}
		else
		{
			g_FileStarted = false;
			return rv;
		}
	}

	if (streamId != 0 && streamId != 1)
		return E_ADV_INVALID_STREAM_ID;
	
	if (IndexGetFramesCount(streamId) == 0)
	{
		// First frame in stream
		m_FirstFrameInStreamTicks[streamId] = startFrameTicks;
	}

	m_PrevFrameInStreamTicks[streamId] = endFrameTicks;
	
	advfgetpos64(g_AdvFile, &m_NewFrameOffset);

	m_FrameBufferIndex = 0;
	m_CurrentStreamId = streamId;

	m_CurrentFrameElapsedTicks = elapsedTicksSinceFirstFrame;
		
	if (m_FrameBytes == NULL)
	{
		int imageBufferSize = 0;
		rv = ImageSectionMaxFrameBufferSize(&imageBufferSize);
		if (!SUCCEEDED(rv))
			return rv;

		int maxUncompressedBufferSize = 
			4 + // frame start magic
			8 + // timestamp
			4 + // exposure			
			4 + 4 + // the length of each of the 2 sections 
			StatusSectionMaxFrameBufferSize +
			imageBufferSize + 
			100; // Just in case
		
		m_FrameBytes = (char*)malloc(maxUncompressedBufferSize * sizeof(unsigned char));
	};		
	
	m_FrameBytes[0] = streamId;

	// Add the start timestamp
	m_FrameBytes[1] = (unsigned char)(startFrameTicks & 0xFF);
	m_FrameBytes[2] = (unsigned char)((startFrameTicks >> 8) & 0xFF);
	m_FrameBytes[3] = (unsigned char)((startFrameTicks >> 16) & 0xFF);
	m_FrameBytes[4] = (unsigned char)((startFrameTicks >> 24) & 0xFF);
	m_FrameBytes[5] = (unsigned char)((startFrameTicks >> 32) & 0xFF);
	m_FrameBytes[6] = (unsigned char)((startFrameTicks >> 40) & 0xFF);
	m_FrameBytes[7] = (unsigned char)((startFrameTicks >> 48) & 0xFF);
	m_FrameBytes[8] = (unsigned char)((startFrameTicks >> 56) & 0xFF);
	
	// Add the end timestamp
	m_FrameBytes[9] = (unsigned char)(endFrameTicks & 0xFF);
	m_FrameBytes[10] = (unsigned char)((endFrameTicks >> 8) & 0xFF);
	m_FrameBytes[11] = (unsigned char)((endFrameTicks >> 16) & 0xFF);
	m_FrameBytes[12] = (unsigned char)((endFrameTicks >> 24) & 0xFF);
	m_FrameBytes[13] = (unsigned char)((endFrameTicks >> 32) & 0xFF);
	m_FrameBytes[14] = (unsigned char)((endFrameTicks >> 40) & 0xFF);
	m_FrameBytes[15] = (unsigned char)((endFrameTicks >> 48) & 0xFF);
	m_FrameBytes[16] = (unsigned char)((endFrameTicks >> 56) & 0xFF);
	
	m_FrameBufferIndex = 17;
	
	rv = ImageSectionBeginFrame();	
	if (rv != S_OK) return rv;

	rv = StatusSectionBeginFrame(utcStartTimeNanosecondsSinceAdvZeroEpoch, utcExposureNanoseconds);
	if (rv != S_OK) return rv;

	m_FrameStarted = true;
	return S_OK;
}

bool Is12BitImagePackedImageLayout(struct mapIntImageLayout *imageLayout)
{
	return 0 == strcmp(imageLayout->layoutType, "12BIT-IMAGE-PACKED");
}

bool IsFullImageRawImageLayout(struct mapIntImageLayout *imageLayout)
{
	return 0 == strcmp(imageLayout->layoutType, "FULL-IMAGE-RAW");
}

unsigned char* ImageSectionGetDataBytes(unsigned char layoutId, unsigned short* pixels, int* imageBytesCount, unsigned char pixelsBpp,enum GetByteOperation operation)
{
	// TODO:
}

unsigned char* StatusSectionGetDataBytes(unsigned int * statusBytesCount)
{
	// TODO:
}

ADVRESULT AddFrameImageInternal(unsigned char layoutId, unsigned short* pixels, unsigned char pixelsBpp, enum GetByteOperation operation)
{
	unsigned int imageBytesCount = 0;	
	unsigned char *imageBytes = ImageSectionGetDataBytes(layoutId, pixels, &imageBytesCount, pixelsBpp, operation);
	
	int imageSectionBytesCount = imageBytesCount + 2; // +1 byte for the layout id and +1 byte for the byteMode (See few lines below)
	
	m_FrameBytes[m_FrameBufferIndex] = imageSectionBytesCount & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 1] = (imageSectionBytesCount >> 8) & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 2] = (imageSectionBytesCount >> 16) & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 3] = (imageSectionBytesCount >> 24) & 0xFF;
	m_FrameBufferIndex+=4;
	
	// It is faster to write the layoutId and byteMode directly here
	m_FrameBytes[m_FrameBufferIndex] = layoutId;
	m_FrameBytes[m_FrameBufferIndex + 1] = 0; // byteMode of Normal (reserved for future use of differential coding)
	m_FrameBufferIndex+=2;
		
	memcpy(&m_FrameBytes[m_FrameBufferIndex], &imageBytes[0], imageBytesCount);
	m_FrameBufferIndex+= imageBytesCount;
		
	unsigned int statusBytesCount = 0;
	unsigned char *statusBytes = StatusSectionGetDataBytes(&statusBytesCount);
	
	m_FrameBytes[m_FrameBufferIndex] = statusBytesCount & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 1] = (statusBytesCount >> 8) & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 2] = (statusBytesCount >> 16) & 0xFF;
	m_FrameBytes[m_FrameBufferIndex + 3] = (statusBytesCount >> 24) & 0xFF;
	m_FrameBufferIndex+=4;
	
	if (statusBytesCount > 0)
	{
		memcpy(&m_FrameBytes[m_FrameBufferIndex], &statusBytes[0], statusBytesCount);
		m_FrameBufferIndex+=statusBytesCount;

		free(statusBytes);
	}
	
	m_ImageAdded = true;	
}

/* Assumed pixel format by AdvCore when this method is called

    |    Layout Type    |  ImageLayout.Bpp |  Assumed Pixel Format                                         |
    |  FULL-IMAGE-RAW   |    16, 12, 8     | 16-bit data (1 short per pixel)                               |
    |12BIT-IMAGE-PACKED |    12            | 16-bit data (1 short per pixel) will be packed when storing   |
    
	All other combinations which are not listed above are invalid.
*/
ADVRESULT AdvFrameAddImage(unsigned char layoutId, unsigned short* pixels, unsigned char pixelsBpp)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_ImageSectionSet)
		return E_ADV_IMAGE_SECTION_UNDEFINED;

	if (!m_FrameStarted)
		return E_ADV_FRAME_NOT_STARTED;
		
	HASH_FIND_INT(m_ImageLayouts, &layoutId, m_CurrentImageLayout);
	if (!m_CurrentImageLayout)
	{
		return E_ADV_INVALID_IMAGE_LAYOUT_ID;
		
	}
	if (Is12BitImagePackedImageLayout(m_CurrentImageLayout) && m_DataBpp == 12)
	{
		AddFrameImageInternal(layoutId, pixels, pixelsBpp, ConvertTo12BitPacked);
		return S_OK;
	}
	else if (IsFullImageRawImageLayout(m_CurrentImageLayout) && m_DataBpp == 8)
	{
		AddFrameImageInternal(layoutId, pixels, pixelsBpp, ConvertTo8BitBytesLooseHighByte);
		return S_OK;
	}
	else if (IsFullImageRawImageLayout(m_CurrentImageLayout))
	{
		AddFrameImageInternal(layoutId, pixels, pixelsBpp, None);
		return S_OK;
	}

	return E_FAIL;
}

ADVRESULT AdvFrameAddImageBytes(unsigned char layoutId, unsigned char* pixels, unsigned char pixelsBpp)
{
	return E_NOTIMPL;
}

ADVRESULT VaidateStatusTagId(unsigned int tagIndex, int expectedTagType)
{
	if (tagIndex < 0 || tagIndex >= utarray_len(m_TagDefinitionNames))
		return E_ADV_INVALID_STATUS_TAG_ID;

	char** tagName = (char**)utarray_eltptr(m_TagDefinitionNames, tagIndex);
	
	struct mapCharInt *tag;

    HASH_FIND_STR( m_TagDefinition, *tagName, tag);
	
	if (!tag)
		return E_ADV_INVALID_STATUS_TAG_ID;
	
	if (tag->value != expectedTagType)
		return E_ADV_INVALID_STATUS_TAG_TYPE;

	return S_OK;
}

ADVRESULT AdvFrameAddStatusTagUTF8String(unsigned int tagIndex, const char* tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntChar *tagVal;
	HASH_FIND_INT(m_FrameStatusTags, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, UTF8String);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntChar *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTags, key, tagVal);
	}
	return rv;
}

ADVRESULT AdvFrameAddStatusTagUInt8(unsigned int tagIndex, unsigned char tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntInt *tagVal;
	HASH_FIND_INT(m_FrameStatusTagsUInt8, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, Int8);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntInt *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTagsUInt8, key, tagVal);
	}
	return rv;		
}

ADVRESULT AdvFrameAddStatusTag16(unsigned int tagIndex, unsigned short tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntInt *tagVal;
	HASH_FIND_INT(m_FrameStatusTagsUInt16, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, Int16);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntInt *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTagsUInt16, key, tagVal);
	}
	return rv;	
}

ADVRESULT AdvFrameAddStatusTagReal(unsigned int tagIndex, float tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntFloat *tagVal;
	HASH_FIND_INT(m_FrameStatusTagsReal, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, Real4);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntFloat *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTagsReal, key, tagVal);
	}
	return rv;	
}

ADVRESULT AdvFrameAddStatusTag32(unsigned int tagIndex, unsigned int tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntInt *tagVal;
	HASH_FIND_INT(m_FrameStatusTagsUInt32, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, Int32);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntInt *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTagsUInt32, key, tagVal);
	}
	return rv;
}

ADVRESULT AdvFrameAddStatusTag64(unsigned int tagIndex, int64_t tagValue)
{
	if (g_AdvFile == 0)
		return E_ADV_NOFILE;

	if (!m_StatusSectionSet)
		return E_ADV_STATUS_SECTION_UNDEFINED;
	
	struct mapIntInt64 *tagVal;
	HASH_FIND_INT(m_FrameStatusTagsUInt64, &tagIndex, tagVal);	
	
	if (tagVal)
		return E_ADV_STATUS_ENTRY_ALREADY_ADDED;
	
	ADVRESULT rv = VaidateStatusTagId(tagIndex, Long64);
	if (rv == S_OK)
	{
		int key = tagIndex;
		tagVal = (struct mapIntInt64 *)malloc(sizeof *tagVal);
		tagVal->key = key;
		HASH_ADD_INT(m_FrameStatusTagsUInt64, key, tagVal);
	}
	return rv;
}

ADVRESULT AdvEndFrame()
{
	return E_NOTIMPL;
}

int AdvGetLastSystemSpecificFileError()
{
	return m_LastSystemSpecificFileError;
}

void GetLibraryVersion(char* version)
{
	strcpy_s(version, strlen(CORE_VERSION) + 1, CORE_VERSION);
}

void GetLibraryPlatformId(char* platform)
{
#define PLATFORM_WIN_MSVC_32 "MS VC++, x86, Windows"
#define PLATFORM_WIN_MSVC_64 "MS VC++, AMD64, Windows"
#define PLATFORM_WIN_GNU_32 "GNU GCC/G++, x86, Windows"
#define PLATFORM_WIN_GNU_64 "GNU GCC/G++, AMD64, Windows"
#define PLATFORM_LINUX_GNU "GNU GCC/G++, Linux"
#define PLATFORM_OSX_GNU "GNU GCC/G++, OSX"
#define PLATFORM_UNKNOWN "Unknown"

#ifdef MSVC
	#if INTPTR_MAX == INT32_MAX
		strcpy_s(platform, strlen(PLATFORM_WIN_MSVC_32) + 1, PLATFORM_WIN_MSVC_32);
	#elif INTPTR_MAX == INT64_MAX
		strcpy_s(platform, strlen(PLATFORM_WIN_MSVC_64) + 1, PLATFORM_WIN_MSVC_64);
	#endif
#elif __linux__
	strcpy_s(platform, strlen(PLATFORM_LINUX_GNU) + 1, PLATFORM_LINUX_GNU);
#elif __APPLE__
	strcpy_s(platform, strlen(PLATFORM_OSX_GNU) + 1, PLATFORM_OSX_GNU);
#elif __GNUC__ || __GNUG__
	#if __x86_64__ || __ppc64__ || _WIN64
		strcpy_s(platform, strlen(PLATFORM_WIN_GNU_64) + 1, PLATFORM_WIN_GNU_64);
	#else
		strcpy_s(platform, strlen(PLATFORM_WIN_GNU_32) + 1, PLATFORM_WIN_GNU_32);
	#endif	
#else
	strcpy_s(platform, strlen(PLATFORM_UNKNOWN) + 1, PLATFORM_UNKNOWN);
#endif
}

int GetLibraryBitness()
{
	#if __GNUC__
	
		#if defined(_WIN64)
			// Windows compilation with GCC
			return 64;
		#elif defined(_WIN32)
			// Windows compilation with GCC
			return 32;
		#endif
		
		// Linux/OSX Compilation
		
		// All modern 64-bit Unix systems use LP64. MacOS X and Linux are both modern 64-bit systems.
		//	Type           ILP64   LP64   LLP64
		//  char              8      8       8
		//  short            16     16      16
		//  int              64     32      32
		//  long             64     64      32
		//  long long        64     64      64
		//  pointer          64     64      64
		//------------------------------------
		// On a Unix system (gcc/g++ compiler) the bitness can be determined by the size of 'long'
		return sizeof(long) * 8;
		
	#endif
	#if _WIN32 || _WIN64

		#if defined(_WIN64)
			return 64;  // 64-bit programs run only on Win64
			
		#elif defined(_WIN32)
			//// 32-bit programs run on both 32-bit and 64-bit Windows so must sniff
			//BOOL f64 = FALSE;
			//if (IsWow64Process(GetCurrentProcess(), &f64) && f64)
			//	return 64;
			//else
			//	return 32;

			// We only care if the binary is 32 or 64 bit, so ignore the IsWow64Process thing
			return 32;
		#else
			return 16; // Win64 does not support Win16
		#endif
	#endif
}