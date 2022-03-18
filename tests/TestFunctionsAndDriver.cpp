/* 
 * File:   TestDrviver.cpp
 * Author: dseres
 * 
 * Created on June 8, 2016, 12:55 PM
 */

#include "TestFunctionsAndDriver.h"
#include <iostream>
#include <cstring>
#include "../generic_erlang_driver.h"

using std::cout;
using std::endl;



int testfun1(int i, double d, char c, const char *s) {
    cout << "Test function 1st called with " << i << ", " << d << ", " << c << ", " << s << "." << endl;
    return i + static_cast<int> (d) + (c - 'A') + strlen(s);
};

int testfun2() {
    cout << "Test function 2th called." << endl;
    return 123456789;
};

void * testfun3(const char *c1, char *c2) {
    cout << "Test function 3th called." << endl;
    return (void *) 0x11111;
};

void testfun4(unsigned long long l) {
    cout << "Test function 4th called with parameter " << l << "." << endl;
};

void testfun5() {
    cout << "Test function 5th called." << endl;
};

int testfun6(int i) {
    cout << "Test function 6th called with parameter " << i << "." << endl;
    return i;
};

using namespace generic_erlang_driver;

TestFunctionDriver::TestFunctionDriver(char *driver_name) : generic_erlang_driver::Driver(driver_name) {
    this->add_test_invoker( "testfun1", create_invoker(testfun1), &output_buffer);
    this->add_test_invoker( "testfun2", create_invoker(testfun2, Marshaller<int>()), &output_buffer);
    this->add_test_invoker( "testfun3", create_invoker(testfun3, Marshaller<void *>(), make_tuple(Marshaller<const char*>(), Marshaller<char*>())), &output_buffer);
    this->add_test_invoker( "testfun4", create_invoker(testfun4), &output_buffer);
    this->add_test_invoker( "testfun5", create_invoker(testfun5), &output_buffer);
    this->add_test_invoker( "testfun6", create_invoker(testfun6, Marshaller<int>(), make_tuple(TestConstantMarshaller<int>())), &output_buffer);
};

TestFunctionDriver::~TestFunctionDriver()  = default;

TestFunctionDriver testfunction_driver((char *)"test driver");
Driver *Driver::driver_instance = &testfunction_driver;
//Driver *Driver::driver_instance = new TestFunctionDriver((char*)"test driver");

