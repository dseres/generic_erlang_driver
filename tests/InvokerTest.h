/* 
 * File:   InvokerTest.h
 * Author: dseres
 *
 * Created on July 5, 2016, 3:21 PM
 */

#ifndef INVOKERTEST_H
#define	INVOKERTEST_H


#include <cppunit/extensions/HelperMacros.h>

#include "TestFunctionsAndDriver.h"

using generic_erlang_driver::InvokerBase;


class InvokerTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(InvokerTest);

    CPPUNIT_TEST(test_function1);
    CPPUNIT_TEST(test_function2);
    CPPUNIT_TEST(test_function3);
    CPPUNIT_TEST(test_function4);
    CPPUNIT_TEST(test_function5);
    CPPUNIT_TEST(test_function6);

    CPPUNIT_TEST_SUITE_END();

public:
    InvokerTest();
    virtual ~InvokerTest();
    void setUp();
    void tearDown();

private:

    /* Helper functions */
    template <typename R, typename ...Ts>
    void invoke_function(InvokerBase *invoker, R r, Ts ... args);
    template <typename ...Ts>
    void invoke_void_function(InvokerBase *invoker, Ts ... args);
    
    /* Test methods */
    void test_function1();
    void test_function2();
    void test_function3();
    void test_function4();
    void test_function5();
    void test_function6();
};

#endif	/* INVOKERTEST_H */

