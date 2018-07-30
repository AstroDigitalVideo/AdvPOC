#include "include.h"
#include "CuTest.h"

void TestAdvAddFileTag(CuTest *tc)
{
    ADVRESULT rv = AdvAddFileTag("FSTF-TYPE", "ADV");
    CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
    
    rv = AdvNewFile("test.adv", true);
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddFileTag("FSTF-TYPE", "ADV");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddFileTag("ADV-VERSION", NULL);
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_NULL, rv);
    
    rv = AdvAddFileTag("12345678901234567890123456789012345678901234567890123456789012345", "Test");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_TOO_LONG, rv);

    rv = AdvAddFileTag("ADV-VERSION", "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_VALUE_TOO_LONG, rv);

    rv = AdvAddFileTag("ADV-VERSION", "1");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddFileTag("ADV-VERSION", "2");
    CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);

    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);
}

void TestAdvAddUserTag(CuTest *tc)
{
    ADVRESULT rv = AdvAddUserTag("MY-INFO", "ABC");
    CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
    
    rv = AdvNewFile("test.adv", true);
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddUserTag("MY-INFO", "ABC");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddUserTag("MY-INFO2", NULL);
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_NULL, rv);
    
    rv = AdvAddUserTag("12345678901234567890123456789012345678901234567890123456789012345", "Test");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_TOO_LONG, rv);

    rv = AdvAddUserTag("ADV-VERSION", "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_VALUE_TOO_LONG, rv);

    rv = AdvAddUserTag("MY-INFO2", "1");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddUserTag("MY-INFO2", "2");
    CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);

    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);
}

void TestAdvAddMainStreamTag(CuTest *tc)
{
    ADVRESULT rv = AdvAddMainStreamTag("Name1", "Христо");
    CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
    
    rv = AdvNewFile("test.adv", true);
    CuAssertIntEquals(tc, S_OK, rv);

    rv = AdvAddMainStreamTag("Name1", NULL);
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_NULL, rv);
    
    rv = AdvAddMainStreamTag("12345678901234567890123456789012345678901234567890123456789012345", "Test");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_TOO_LONG, rv);

    rv = AdvAddMainStreamTag("Name1", "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_VALUE_TOO_LONG, rv);

    rv = AdvAddMainStreamTag("Name1", "Христо");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddMainStreamTag("Name1", "Павлов");
    CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);

    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);
}

void TestAdvAddCalibrationStreamTag(CuTest *tc)
{
    ADVRESULT rv = AdvAddCalibrationStreamTag("Name1", "Frédéric");
    CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
    
    rv = AdvNewFile("test.adv", true);
    CuAssertIntEquals(tc, S_OK, rv);

    rv = AdvAddCalibrationStreamTag("Name1", NULL);
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_NULL, rv);
    
    rv = AdvAddCalibrationStreamTag("12345678901234567890123456789012345678901234567890123456789012345", "Test");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_KEY_TOO_LONG, rv);

    rv = AdvAddCalibrationStreamTag("Name1", "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
    CuAssertIntEquals(tc, E_ADV_FILE_HASH_VALUE_TOO_LONG, rv);

    rv = AdvAddCalibrationStreamTag("Name1", "Frédéric");
    CuAssertIntEquals(tc, S_OK, rv);
    
    rv = AdvAddCalibrationStreamTag("Name1", "值1");
    CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);

    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);
}

void TestAdvDefineStatusSectionTag(CuTest *tc)
{
	unsigned int tagId;
	ADVRESULT rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
    
    rv = AdvNewFile("test.adv", true);
    CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);

	rv = AdvDefineStatusSectionTag("Tag2", UTF8String, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvDefineStatusSectionTag("Tag1", Real4, &tagId);
	CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);
	CuAssertIntEquals(tc, 0, tagId);

    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);	
}

void TestAdvAddImageSectionTag(CuTest *tc)
{
	ADVRESULT rv = AdvAddImageSectionTag("IMAGE-BYTE-ORDER", "LITTLE-ENDIAN");
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);

	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvAddImageSectionTag("IMAGE-BYTE-ORDER", "LITTLE-ENDIAN");
	CuAssertIntEquals(tc, E_ADV_IMAGE_SECTION_UNDEFINED, rv);
	
	rv = AdvDefineImageSection(800, 600, 16);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvAddImageSectionTag("IMAGE-BYTE-ORDER", "LITTLE-ENDIAN");
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvAddImageSectionTag("IMAGE-BITPIX", "16");
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvAddImageSectionTag("IMAGE-BYTE-ORDER", "BIG-ENDIAN");
	CuAssertIntEquals(tc, S_ADV_TAG_REPLACED, rv);
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvDefineImageLayouts(CuTest *tc)
{
	ADVRESULT rv = AdvDefineImageLayout(0, "FULL-IMAGE-RAW", "UNCOMPRESSED", 16);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvDefineImageLayout(0, "FULL-IMAGE-RAW", "UNCOMPRESSED", 16);
	CuAssertIntEquals(tc, E_ADV_IMAGE_SECTION_UNDEFINED, rv);
	
	rv = AdvDefineImageSection(800, 600, 16);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvDefineImageLayout(0, NULL, NULL, 16);
	CuAssertIntEquals(tc, E_ADV_INVALID_IMAGE_LAYOUT_TYPE, rv);
	
	rv = AdvDefineImageLayout(0, "FULL-IMAGE-RAW", "UNCOMPRESSED", 16);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvDefineImageLayout(0, "BLAH", "BLAH", 32);
	CuAssertIntEquals(tc, E_ADV_IMAGE_LAYOUT_ALREADY_DEFINED, rv);	
	
	rv = AdvDefineImageLayout(1, "BLAH", "UNCOMPRESSED", 32);
	CuAssertIntEquals(tc, E_ADV_INVALID_IMAGE_LAYOUT_TYPE, rv);		

	rv = AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "BLAH", 32);
	CuAssertIntEquals(tc, E_ADV_INVALID_IMAGE_LAYOUT_COMPRESSION, rv);
	
	rv = AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "UNCOMPRESSED", 0);
	CuAssertIntEquals(tc, E_ADV_INVALID_IMAGE_LAYOUT_BPP, rv);

	rv = AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "UNCOMPRESSED", 64);
	CuAssertIntEquals(tc, E_ADV_INVALID_IMAGE_LAYOUT_BPP, rv);

	rv = AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "LAGARITH16", 16);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvDefineImageLayout(2, "FULL-IMAGE-RAW", "QUICKLZ", 8);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvDefineImageLayout(3, "12BIT-IMAGE-PACKED", "UNCOMPRESSED", 8);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvDefineImageLayout(4, "8BIT-COLOR-IMAGE", "UNCOMPRESSED", 32);
	CuAssertIntEquals(tc, S_OK, rv);
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);			
}

void TestAdvFrameAddStatusTag16(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTag16(0, 12);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTag16(0, 12);
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTag16(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTag16(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", Int16, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTag16(1, 12);
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTag16(1, 13);
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFrameAddStatusTag8(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTagUInt8(0, 12);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTagUInt8(0, 12);
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTagUInt8(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTagUInt8(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", Int8, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTagUInt8(1, 12);
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTagUInt8(1, 13);
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFrameAddStatusTag32(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTag32(0, 12);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTag32(0, 12);
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTag32(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int16, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTag32(0, 12);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTag32(1, 12);
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTag32(1, 13);
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFrameAddStatusTag64(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTag64(0, 0x12345678ABCD);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTag64(0, 0x12345678ABCD);
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTag64(0, 0x12345678ABCD);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTag64(0, 0x12345678ABCD);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", Long64, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTag64(1, 0x12345678ABCD);
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTag64(1, 0x12345678ABFF);
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFrameAddStatusTagReal(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTagReal(0, -12.346);
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTagReal(0, -12.346);
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTagReal(0, -12.346);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTagReal(0, -12.346);
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", Real4, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTagReal(1, -12.346);
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTagReal(1, 104523.12);
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFrameAddStatusTagUTF8String(CuTest *tc)
{
	ADVRESULT rv = AdvFrameAddStatusTagUTF8String(0, "AlaBala");
	CuAssertIntEquals(tc, E_ADV_NOFILE, rv);
	
	rv = AdvNewFile("test.adv", true);
	CuAssertIntEquals(tc, S_OK, rv);
	
	rv = AdvFrameAddStatusTagUTF8String(0, "AlaBala");
	CuAssertIntEquals(tc, E_ADV_STATUS_SECTION_UNDEFINED, rv);	
	
	rv = AdvDefineStatusSection(1 * MILLI_TO_NANO);
	CuAssertIntEquals(tc, S_OK, rv);

	rv = AdvFrameAddStatusTagUTF8String(0, "AlaBala");
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_ID, rv);	
		
	unsigned int tagId;
	rv = AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 0, tagId);
	
	rv = AdvFrameAddStatusTagUTF8String(0, "AlaBala");
	CuAssertIntEquals(tc, E_ADV_INVALID_STATUS_TAG_TYPE, rv);	
	
	rv = AdvDefineStatusSectionTag("Tag2", UTF8String, &tagId);
    CuAssertIntEquals(tc, S_OK, rv);
	CuAssertIntEquals(tc, 1, tagId);
	
	rv = AdvFrameAddStatusTagUTF8String(1, "AlaBala");
	CuAssertIntEquals(tc, S_OK, rv);	
	
	rv = AdvFrameAddStatusTagUTF8String(1, "Gérard");
	CuAssertIntEquals(tc, E_ADV_STATUS_ENTRY_ALREADY_ADDED, rv);	
	
    rv = AdvEndFile();
	CuAssertIntEquals(tc, E_ADV_FILE_NOT_OPEN, rv);		
}

void TestAdvFile(CuTest *tc)
{
	AdvNewFile("test.adv", true);
	
	//AdvDefineExternalClockForMainStream(m_MainClockFrequency, m_MainTicksTimingAccuracy);
	
	AdvAddFileTag("FSTF-TYPE", "ADV");
	AdvAddFileTag("ADV-VERSION", "1");
	AdvAddFileTag("ADV-VERSION", "2");
	AdvAddFileTag("RECORDER-SOFTWARE", "AdvPOC");
	char ver[10];
	GetLibraryVersion(ver);
	AdvAddFileTag("RECORDER-SOFTWARE-VERSION", "1.0");
	AdvAddFileTag("ADVLIB-VERSION", ver);
	
	AdvAddMainStreamTag("EXAMPLE1", "1234");
	AdvAddCalibrationStreamTag("CAL1", "ASDFG");
	AdvAddUserTag("UserStuff", "0%k#$");
	
	AdvDefineImageSection(800, 600, 16);
	AdvDefineStatusSection(1 * MILLI_TO_NANO);
	
	AdvAddImageSectionTag("IMAGE-BYTE-ORDER", "LITTLE-ENDIAN");
	AdvAddImageSectionTag("IMAGE-BITPIX", "16");
	AdvAddImageSectionTag("IMAGE-MAX-PIXEL-VALUE", "1024");

	AdvDefineImageLayout(0, "FULL-IMAGE-RAW", "UNCOMPRESSED", 16);
	AdvDefineImageLayout(1, "FULL-IMAGE-RAW", "LAGARITH16", 16);
	AdvDefineImageLayout(2, "FULL-IMAGE-RAW", "QUICKLZ", 16);
	AdvDefineImageLayout(3, "FULL-IMAGE-RAW", "UNCOMPRESSED", 12);
	AdvDefineImageLayout(4, "FULL-IMAGE-RAW", "UNCOMPRESSED", 8);
	AdvDefineImageLayout(5, "FULL-IMAGE-RAW", "QUICKLZ", 8);
	
	unsigned int tagId;
	AdvDefineStatusSectionTag("Tag1", Int32, &tagId);
	AdvDefineStatusSectionTag("Tag2", Real4, &tagId);
	AdvDefineStatusSectionTag("Tag2", UTF8String, &tagId);
	
	// TODO: Test overflow in and UInt8, Int16 status values, making sure they are saved correctly

	AdvEndFile();
	
	// TODO: Assert that everything has been cleared up 
}

CuSuite* TestBasicAdvGetSuite() {
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestAdvAddFileTag);
	SUITE_ADD_TEST(suite, TestAdvAddUserTag);
	SUITE_ADD_TEST(suite, TestAdvAddMainStreamTag);
	SUITE_ADD_TEST(suite, TestAdvAddCalibrationStreamTag);
	SUITE_ADD_TEST(suite, TestAdvDefineStatusSectionTag);
	SUITE_ADD_TEST(suite, TestAdvAddImageSectionTag);
	SUITE_ADD_TEST(suite, TestAdvDefineImageLayouts);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTag8);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTag16);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTag32);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTag64);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTagReal);
	SUITE_ADD_TEST(suite, TestAdvFrameAddStatusTagUTF8String);
	return suite;
}