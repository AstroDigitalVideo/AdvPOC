#include "..\uthash.h"
#include "..\utarray.h"
#include "cutest.h"

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

struct mapIntImageLayout {
    int key;
    char layoutType[265];
	char compression[265];
	unsigned char layoutBpp;
    UT_hash_handle hh;
};

struct my_struct *users = NULL;
struct mapCharChar *tags = NULL;
struct mapIntImageLayout *layouts = NULL;

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

void add_update_image_layout(int key, char *layoutType, char *compression, unsigned char layoutBpp) {
    struct mapIntImageLayout *s;
    HASH_FIND_INT(layouts, &key, s);  /* id already in the hash? */
    if (s==NULL) {
      s = (struct mapIntImageLayout *)malloc(sizeof *s);
      s->key = key;

      HASH_ADD_INT( layouts, key, s );  /* id: name of key field */
    }
    strcpy(s->layoutType, layoutType);
    strcpy(s->compression, compression);
	s->layoutBpp = layoutBpp;
}

void TestIntCharMap(CuTest *tc)
{
    add_user(1, "user1");

    struct my_struct *user;

    int user_id = 1;
    HASH_FIND_INT( users, &user_id, user);

    CuAssertStrEquals(tc, "user1", user->name);

    add_update_user(1, "user2");
    CuAssertStrEquals(tc, "user2", user->name);

    HASH_DEL(users, user);
    free(user);
    
    HASH_FIND_INT( users, &user_id, user);
    CuAssertPtrEquals(tc, NULL, user);
}

void TestCharCharMap(CuTest *tc)
{
	add_update_tag("123", "user1");
    
	struct mapCharChar *tag;

    char *tag_id = "123";
    HASH_FIND_STR( tags, tag_id, tag);

    CuAssertStrEquals(tc, "user1", tag->value);

    add_update_tag("123", "user2");

    HASH_FIND_STR( tags, tag_id, tag);

    CuAssertStrEquals(tc, "user2", tag->value);

	struct mapCharChar *current_tag, *tmp;

	HASH_ITER(hh, tags, current_tag, tmp) {
	HASH_DEL(tags,current_tag);  /* delete; users advances to next */
	   free(current_tag);            /* optional- if you want to free  */
	}
    
	HASH_FIND_STR( tags, tag_id, tag);
    
    CuAssertPtrEquals(tc, NULL, tag);
}

void TestArray(CuTest *tc)
{
	UT_array *strs;
	char *s, **p;

	utarray_new(strs,&ut_str_icd);

	s = "hello"; utarray_push_back(strs, &s);
	s = "world"; utarray_push_back(strs, &s);
	p = NULL;

	p=(char**)utarray_next(strs,p);
    CuAssertStrEquals(tc, "hello", *p);

	p=(char**)utarray_next(strs,p);
    CuAssertStrEquals(tc, "world", *p);
    
	utarray_free(strs);
}

void TestIntImageLayoutMap(CuTest *tc)
{
	add_update_image_layout(1, "type1", "compress2", 16);
    
    struct mapIntImageLayout *layout;

    int layout_id = 1;
    HASH_FIND_INT( layouts, &layout_id, layout);

    CuAssertStrEquals(tc, "type1", layout->layoutType);
	CuAssertStrEquals(tc, "compress2", layout->compression);
	CuAssertIntEquals(tc, 16, layout->layoutBpp);

    add_update_image_layout(1, "type2", "compress5", 32);
    CuAssertStrEquals(tc, "type2", layout->layoutType);
	CuAssertStrEquals(tc, "compress5", layout->compression);
	CuAssertIntEquals(tc, 32, layout->layoutBpp);
	
    HASH_DEL(layouts, layout);
    free(layout);
    
    HASH_FIND_INT( layouts, &layout_id, layout);
    CuAssertPtrEquals(tc, NULL, layout);
}

CuSuite* TestUTGetSuite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestArray);
    SUITE_ADD_TEST(suite, TestCharCharMap);
    SUITE_ADD_TEST(suite, TestIntCharMap);
	SUITE_ADD_TEST(suite, TestIntImageLayoutMap);
    return suite;
}