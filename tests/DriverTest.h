/*
 * File:   GenErlDrvTest.h
 * Author: dseres
 *
 * Created on May 25, 2016, 10:43:57 AM
 */

#ifndef GENERLDRVTEST_H
#define	GENERLDRVTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "TestFunctionsAndDriver.h"

using namespace generic_erlang_driver;

typedef struct ei_x_buff_TAG ei_x_buff;

class DriverTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(DriverTest);

    CPPUNIT_TEST(test_function1);
    CPPUNIT_TEST(test_function2);
    CPPUNIT_TEST(test_function3);
    CPPUNIT_TEST(test_function4);
    CPPUNIT_TEST(test_function5);
    CPPUNIT_TEST(test_function6);

    CPPUNIT_TEST_SUITE_END();

public:
    DriverTest();
    virtual ~DriverTest();
    void setUp();
    void tearDown();

private:

    /* Helper functions */
    ei_x_buff * create_x_buff_with_functionname(const char * function_name);
    ei_x_buff * invoke_function( char *buff);
    
    template <typename R, typename ...Ts>
    void test_function(const char *function_name, R r, Ts ... args);
    template <typename ...Ts>
    void test_void_type_function(const char *function_name, Ts ... args);

    /* Test methods */
    void test_function1();
    void test_function2();
    void test_function3();
    void test_function4();
    void test_function5();
    void test_function6();
};

        
#endif	/* GENERLDRVTEST_H */

