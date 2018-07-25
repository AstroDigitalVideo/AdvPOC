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

	AdvEndFile();
}

CuSuite* TestBasicAdvGetSuite() {
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestAdvAddFileTag);
	SUITE_ADD_TEST(suite, TestAdvAddMainStreamTag);
	SUITE_ADD_TEST(suite, TestAdvAddCalibrationStreamTag);
	SUITE_ADD_TEST(suite, TestAdvDefineStatusSectionTag);
	SUITE_ADD_TEST(suite, TestAdvAddImageSectionTag);
	return suite;
}