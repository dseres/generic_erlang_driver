/* 
 * File:   MarshallerTest.cpp
 * Author: dseres
 * 
 * Created on June 6, 2016, 9:10 AM
 */

#include "MarshallerTest.h"

#include "../generic_erlang_driver.h"
#include <string>
#include <iostream>
#include <cstring>
#include <limits>


using std::string;
using std::cout;
using std::endl;
using std::make_tuple;
using std::numeric_limits;
using namespace generic_erlang_driver;


CPPUNIT_TEST_SUITE_REGISTRATION(MarshallerTest);

MarshallerTest::MarshallerTest() {
}

MarshallerTest::~MarshallerTest() {
}

void MarshallerTest::setUp() {
}

void MarshallerTest::tearDown() {
}

ei_x_buff * MarshallerTest::encode_data(){
        ei_x_buff *x = create_x_buff_with_version();
        encode_tuple_arity(x, 2);
        encode_atom(x, "testfun1");
        encode_tuple_arity(x, 4);
        encode_long(x, 1);
        encode_double(x, 3.14);
        encode_long(x, 'B');
        encode_string(x, "Hello");
        return x;
}

void MarshallerTest::decode_data(char *buff){
        int index = 0;
        int version = decode_version(buff, &index);
        cout<<"Version = "<< version <<endl;
        int arity = decode_tuple_arity(buff, &index);
        cout<<"Arity = "<< arity <<endl;
        CPPUNIT_ASSERT(arity == 2);
        string function_code = decode_string(buff, &index);
        cout<<"Function code = "<< function_code <<endl;
        CPPUNIT_ASSERT(function_code == "testfun1");
        arity = decode_tuple_arity(buff, &index);
        cout<<"Arity = "<< arity <<endl;
        CPPUNIT_ASSERT(arity == 4);
        long l=decode_long(buff,&index);
        cout<<"Long value= "<< l <<endl;
        CPPUNIT_ASSERT(l == 1);
        double d=decode_double(buff,&index);
        cout<<"Double value= "<< d <<endl;
        CPPUNIT_ASSERT(d == 3.14);
        char c=decode_long(buff,&index);
        cout<<"char value= "<< c <<endl;
        CPPUNIT_ASSERT(c == 'B');
        EiType type = decode_type(buff, &index);
        cout<<"Str size = "<< type.size <<endl;
        char str[type.size +1];
        decode_c_str(buff, &index, str);
        cout<<"Str value = "<< str <<endl;
        CPPUNIT_ASSERT( strcmp(str, "Hello") == 0);
}

void MarshallerTest::test_encoder_functions() {
    try {
        ei_x_buff *x = encode_data();
        decode_data(to_cbuff(x));
        free_x_buff(x);
        CPPUNIT_ASSERT(true);
    } catch (Driver_Exception ex) {
        CPPUNIT_FAIL( string("DriverException was raised : ") + ex.what());
    }
}
    
template<typename T, T v>
void MarshallerTest::test_integral_marshaller(){
    ei_x_buff *x = create_x_buff_with_version();
    Marshaller<T> m;
    m.marshal(x, v);
    char *buff = to_cbuff(x);
    int index = 0;
    decode_version(buff, &index);
    T value = m.unmarshal(buff, &index);
    CPPUNIT_ASSERT(value == v);
}

void MarshallerTest::test_float_marshaller(){
    ei_x_buff *x = create_x_buff_with_version();
    Marshaller<double> m;
    m.marshal(x, 3.14);
    m.marshal(x, -2365.4789);
    char *buff = to_cbuff(x);
    int index = 0;
    decode_version(buff, &index);
    double value1 = m.unmarshal(buff, &index);
    double value2 = m.unmarshal(buff, &index);
    CPPUNIT_ASSERT(3.14 == value1);
    CPPUNIT_ASSERT(-2365.4789 == value2);
}

void MarshallerTest::test_str_marshaller(){
    ei_x_buff *x = create_x_buff_with_version();
    Marshaller<char *> m1;
    Marshaller<const char *> m2;
    m1.marshal(x, (char *)"Test1");
    m2.marshal(x, (const char *)"Test2");
    char *buff = to_cbuff(x);
    int index = 0;
    decode_version(buff, &index);
    char *c1 = m1.unmarshal(buff, &index);
    const char *c2 = m2.unmarshal(buff, &index);
    CPPUNIT_ASSERT( strcmp(c1, "Test1") ==0);
    CPPUNIT_ASSERT( strcmp(c2, "Test2") ==0);
    m1.free(c1);
    m2.free(c2);
}

void MarshallerTest::test_marshallers(){
    Marshaller<void> mv;
    test_integral_marshaller<char, 'a'>();
    test_integral_marshaller<unsigned char, 'Z'>();
    test_integral_marshaller<short, -32767 >();
    test_integral_marshaller<unsigned short, 65535 >();
    test_integral_marshaller<int, -2147483647 >();
    test_integral_marshaller<unsigned int, 4294967295  >();
    test_integral_marshaller<long, -2147483647 >();
    test_integral_marshaller<unsigned long, 4294967295  >();
    test_integral_marshaller<long long, -9223372036854775807LL>();
    test_integral_marshaller<unsigned long long, 18446744073709551615ULL>();
    test_float_marshaller();
    test_str_marshaller();
}
