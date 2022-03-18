/* 
 * File:   InvokerTest.cpp
 * Author: dseres
 * 
 * Created on July 5, 2016, 3:21 PM
 */

#include "InvokerTest.h"
#include "../generic_erlang_driver.h"
#include "helpers.h"
#include <string>
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;
using namespace generic_erlang_driver;

CPPUNIT_TEST_SUITE_REGISTRATION(InvokerTest);

InvokerTest::InvokerTest() {
}

InvokerTest::~InvokerTest() {
}

void InvokerTest::setUp() {
}

void InvokerTest::tearDown() {
}

template <typename R, typename ...Ts>
void InvokerTest::invoke_function(InvokerBase *invoker, R r, Ts ... args){
    try {
        ei_x_buff *ix = create_x_buff();
        add_to_x_buff_helper(ix, args...);
        ei_x_buff *ox = create_x_buff_with_version();
        (*invoker)( to_cbuff(ix), ox);
        R ret = get_return_value<R>(ox);
        CPPUNIT_ASSERT( ret == r ); 
        free_x_buff(ix);
        free_x_buff(ox);
    } catch ( Driver_Exception ex) {
        cout<<"Driver exception was caught. "<<ex.what()<<endl;
        CPPUNIT_ASSERT(false);
    }
}

template <typename ...Ts>
void InvokerTest::invoke_void_function(InvokerBase *invoker, Ts ... args){
    try {
        ei_x_buff *ix = create_x_buff();
        add_to_x_buff(ix, args...);
        ei_x_buff *ox = create_x_buff_with_version();
        (*invoker)( to_cbuff(ix), ox);
        get_return_value<void>(ox);
        free_x_buff(ix);
        free_x_buff(ox);
        CPPUNIT_ASSERT( true );
    } catch ( Driver_Exception ex) {
        cout<<"Driver exception was caught. "<<ex.what()<<endl;
        CPPUNIT_ASSERT(false);
    }
}

void InvokerTest::test_function1() {
    InvokerBase *invoker = create_invoker(testfun1);
    invoke_function(invoker,10, 1, 3.14, 'B', "Hello");
}

void InvokerTest::test_function2() {
    InvokerBase *invoker = create_invoker(testfun2, Marshaller<int>());
    invoke_function(invoker, 123456789);
}

void InvokerTest::test_function3() {
    InvokerBase *invoker = create_invoker(testfun3, Marshaller<void *>(), make_tuple(Marshaller<const char*>(), Marshaller<char*>()));
    const char *c1 = "Helló világ";
    char c2[128];
    strcpy_s(c2, sizeof(c2), c1);
    invoke_function(invoker, (void*)0x11111, c1, c2);
}

void InvokerTest::test_function4() {
    InvokerBase *invoker = create_invoker(testfun4);
    invoke_void_function(invoker, numeric_limits<unsigned long long>::max());
}

void InvokerTest::test_function5() {
    InvokerBase *invoker = create_invoker(testfun5);
    invoke_void_function(invoker);
}

void InvokerTest::test_function6() {
    InvokerBase *invoker = create_invoker(testfun6, Marshaller<int>(), make_tuple(TestConstantMarshaller<int>()));
    invoke_function(invoker,101,"constant1");
    invoke_function(invoker,-1,"constant2");
}
