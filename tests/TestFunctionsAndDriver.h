/* 
 * File:   TestDrviver.h
 * Author: dseres
 *
 * Created on June 8, 2016, 12:55 PM
 */

#ifndef TESTDRVIVER_H
#define	TESTDRVIVER_H

#include "../driver.h"
#include "../generic_erlang_driver.h"
#include <string>

using std::string;

int testfun1(int i, double d, char c, const char *s);
int testfun2();
void * testfun3(const char *c1, char *c2);
void testfun4(unsigned long long l);
void testfun5();
int testfun6(int i);

template<typename T>
class TestConstantMarshaller;

template<>
class TestConstantMarshaller<int>: public generic_erlang_driver::ConstantMarshaller<int>{
public: 
    TestConstantMarshaller() {
        values[101] = string("constant1");
        values[-1] = string("constant2");
        for (auto pair : values)
            values2[pair.second] = pair.first;
    };
};

class TestFunctionDriver: public generic_erlang_driver::Driver{
public:
    TestFunctionDriver(char *driver_name);
    virtual ~TestFunctionDriver();
    ei_x_buff *get_output_buffer(){return output_buffer;};
private:
    ei_x_buff *output_buffer;
};

#endif	/* TESTDRVIVER_H */

