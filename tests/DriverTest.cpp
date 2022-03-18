/*
 * File:   GenErlDrvTest.cpp
 * Author: dseres
 *
 * Created on May 25, 2016, 10:43:58 AM
 */

#include "DriverTest.h"
#include "../generic_erlang_driver.h"
#include "helpers.h"
#include <string>
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;
//using namespace generic_erlang_driver;

CPPUNIT_TEST_SUITE_REGISTRATION(DriverTest);

DriverTest::DriverTest() {
}

DriverTest::~DriverTest() {
}

void DriverTest::setUp() {
    erl_dyndriver_real_driver_init();
    //Driver::driver_instance->start((ErlDrvPort)stdout);
}

void DriverTest::tearDown() {
}

ei_x_buff * DriverTest::create_x_buff_with_functionname(const char * function_name){
    ei_x_buff *x = create_x_buff_with_version();
    encode_tuple_arity(x,2);
    encode_atom(x,function_name);
    return x;
}

ei_x_buff * DriverTest::invoke_function( char *buff){
    Driver *driver = Driver::driver_instance;
//    int index=0;
//    string function_code = driver->driver_pimpl->decode_function_code(buff,&index);
//    InvokerBase *invoker = driver->driver_pimpl->find_invoker(function_code);
//    driver->driver_pimpl->check_param_count(invoker, buff, &index);
//    cout.flush();
    ei_x_buff *ox = create_x_buff_with_version();
//    invoker->operator ()(buff+index, ox);
    return ox;
}

template <typename R, typename ...Ts>
void DriverTest::test_function(const char *function_name, R r, Ts ... args){
    try {
        ei_x_buff *x = create_x_buff_with_functionname(function_name);
        add_to_x_buff(x, args...);
        TestFunctionDriver *driver = static_cast<TestFunctionDriver*>(Driver::driver_instance);
        driver->output( to_cbuff(x), get_size(x) );
        ei_x_buff *output_buffer = driver->get_output_buffer(); 
        R return_value = get_return_value<R>(output_buffer);
        free_x_buff(x);
        free_x_buff(output_buffer);
        CPPUNIT_ASSERT( r == return_value );
    } catch ( Driver_Exception ex) {
        cout<<"Driver exception was caught. "<<ex.what()<<endl;
        CPPUNIT_ASSERT(false);
    }
}

template <typename ...Ts>
void DriverTest::test_void_type_function(const char *function_name, Ts ... args){
    try {
        ei_x_buff *x = create_x_buff_with_functionname(function_name);
        add_to_x_buff(x, args...);
        TestFunctionDriver *driver = static_cast<TestFunctionDriver*>(Driver::driver_instance);
        driver->output( to_cbuff(x), get_size(x) );
        ei_x_buff *output_buffer = driver->get_output_buffer(); 
        get_return_value<void>(output_buffer);
        free_x_buff(x);
        free_x_buff(output_buffer);
        CPPUNIT_ASSERT( true );
    } catch ( Driver_Exception ex) {
        cout<<"Driver exception was caught. "<<ex.what()<<endl;
        CPPUNIT_ASSERT(false);
    }
}


void DriverTest::test_function1() {
    test_function("testfun1",10, 1, 3.14, 'B', "Hello");
}

void DriverTest::test_function2() {
    test_function("testfun2", 123456789);
}

void DriverTest::test_function3() {
    const char *c1 = "Helló világ";
    char c2[128];
    strcpy_s(c2, sizeof(c2), c1);
    test_function("testfun3", (void*)0x11111, c1, c2);
}

void DriverTest::test_function4() {
    test_void_type_function("testfun4", numeric_limits<unsigned long long>::max());
}

void DriverTest::test_function5() {
    test_void_type_function("testfun5");
}

void DriverTest::test_function6() {
    test_function("testfun6",101,"constant1");
    test_function("testfun6",-1,"constant2");
}
