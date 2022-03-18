/* 
 * File:   MarshallerTest.h
 * Author: dseres
 *
 * Created on June 6, 2016, 9:09 AM
 */

#ifndef MARSHALLERTEST_H
#define	MARSHALLERTEST_H

#include <cppunit/extensions/HelperMacros.h>

typedef struct ei_x_buff_TAG ei_x_buff;

class MarshallerTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(MarshallerTest);

    CPPUNIT_TEST(test_encoder_functions);
    CPPUNIT_TEST(test_marshallers);

    CPPUNIT_TEST_SUITE_END();

public:
    MarshallerTest();
    virtual ~MarshallerTest();
    void setUp();
    void tearDown();

private:
    ei_x_buff * encode_data();
    void decode_data(char *buff);
    void test_encoder_functions();

    template<typename T, T v>
    void test_integral_marshaller();
    void test_float_marshaller();
    void test_str_marshaller();
    void test_marshallers();
    
};


#endif	/* MARSHALLERTEST_H */

