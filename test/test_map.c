#include "utest.h"

#include "../src/server/map.h"

struct Fixture {
    Map * map;
};

UTEST_F_SETUP(Fixture){
    utest_fixture->map = mapCreate();
}

UTEST_F_TEARDOWN(Fixture){
    mapDestroy(utest_fixture->map);
}

UTEST_F(Fixture, checkNonExistent){
    ASSERT_EQ(NULL, mapGet(utest_fixture->map, "nonexisting"));
}

UTEST_F(Fixture, checkExisting){
    mapPut(utest_fixture->map, "test_key", "test_value");

    ASSERT_NE(NULL, mapGet(utest_fixture->map, "test_key"));
    ASSERT_EQ(0, strcmp("test_value", mapGet(utest_fixture->map, "test_key")));
}

UTEST_F(Fixture, checkExistingMultipleTimes){
    mapPut(utest_fixture->map, "test_key", "test_value");

    for (int i = 0; i < 1000; ++i) {
        ASSERT_NE(NULL, mapGet(utest_fixture->map, "test_key"));
        ASSERT_EQ(0, strcmp("test_value", mapGet(utest_fixture->map, "test_key")));
    }
}

UTEST_F(Fixture, checkOverride){
    mapPut(utest_fixture->map, "test_key", "test_value");

    ASSERT_NE(NULL, mapGet(utest_fixture->map, "test_key"));
    ASSERT_EQ(0, strcmp("test_value", mapGet(utest_fixture->map, "test_key")));

    mapPut(utest_fixture->map, "test_key", "new_test_value");
    ASSERT_NE(NULL, mapGet(utest_fixture->map, "test_key"));
    ASSERT_EQ(0, strcmp("new_test_value", mapGet(utest_fixture->map, "test_key")));
}
