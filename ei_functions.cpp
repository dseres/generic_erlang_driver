/* 
 * File:   ei_functions.cpp
 * Author: dseres
 * 
 * Created on May 24, 2016, 1:46 PM
 */

#include "ei_functions.h"

#include <ei.h>
#include <typeinfo>

namespace generic_erlang_driver {

    ei_x_buff *create_x_buff() {
        return new ei_x_buff();
    };

    ei_x_buff *create_x_buff_with_version() {
        ei_x_buff *x = create_x_buff();
        ei_x_new_with_version(x);
        return x;
    };

    void free_x_buff(ei_x_buff *x) {
        ei_x_free(x);
        delete x;
    };

    char *to_cbuff(ei_x_buff *x) {
        return x->buff;
    }

    int get_size(ei_x_buff *x) {
        return x->index;
    }

    bool EiType::is_atom() {
        return type == ERL_ATOM_EXT;
    };

    bool EiType::is_string() {
        return type == ERL_STRING_EXT || type == ERL_LIST_EXT;
    };

    int decode_version(char *buff, int *index) {
        int version;
        if (ei_decode_version(buff, index, &version) != 0) {
            throw Driver_Exception("Decoding of version info was failed.");;
        }
        return version;
    }

    int decode_tuple_arity(char *buff, int *index) {
        int arity;
        if (ei_decode_tuple_header(buff, index, &arity) != 0) {
            throw Driver_Exception("Decoding of tuple header was failed.");;
        }
        return arity;
    }

    void encode_tuple_arity(ei_x_buff *x, long n){
        if ( ei_x_encode_tuple_header(x,n) != 0 )
            throw Driver_Exception("Encoding of tuple header was failed.");
    }
    
    EiType decode_type(char *buff, int *index) {
        EiType type;
        if (ei_get_type(buff, index, &type.type, &type.size) != 0)
            throw Driver_Exception("Decoding of type info was failed.");
        return type;
    }

    void decode_c_str(char *buff, int *index, char *str) {
        EiType type = decode_type(buff, index);
        int ret;
        if (type.is_atom())
            ret = ei_decode_atom(buff, index, str);
        else if (type.is_string())
            ret = ei_decode_string(buff, index, str);
        else
            throw Driver_Exception("Decoding of string parameter was failed. Type of parameter is neither atom or string.");
        if (ret != 0) {
            throw Driver_Exception("Decoding of string parameter was failed.");
        }
    }

    std::string decode_string(char *buff, int *index) {
        EiType type = decode_type(buff, index);
        char c_str[type.size + 1];
        decode_c_str(buff, index, c_str);
        return std::string(c_str);
    }

    void encode_atom(ei_x_buff *x, const char* s){
        if ( ei_x_encode_atom(x,s) != 0)
            throw Driver_Exception("Encoding of atom parameter was failed.");
    }
    void encode_atom(ei_x_buff *x, std::string s){
        if ( ei_x_encode_atom(x,s.c_str()) != 0)
            throw Driver_Exception("Encoding of atom parameter was failed.");
    }
    void encode_string(ei_x_buff *x, const char* s){
        if ( ei_x_encode_string(x,s) != 0)
            throw Driver_Exception("Encoding of string parameter was failed.");
    }
    void encode_string(ei_x_buff *x, std::string s){
        if ( ei_x_encode_string(x,s.c_str()) != 0)
            throw Driver_Exception("Encoding of string parameter was failed.");
    }

//    int ei_x_encode_atom(ei_x_buff* x, const char* s)    
//    int ei_decode_atom(const char *buf, int *index, char *p);
//    int ei_x_encode_string(ei_x_buff* x, const char* s)
//    int ei_decode_string(const char *buf, int *index, char *p);
    
    void decode_binary(char *buff, int *index, void *p, long *lenp){
            if (ei_decode_binary(buff, index, p, lenp) != 0) {
                throw Driver_Exception("Decoding of binary parameter was failed.");
            }
    }
    
    void encode_binary(ei_x_buff* x, const void* p, int len){
            if (ei_x_encode_binary(x, p, len) != 0) {
                throw Driver_Exception("Encoding of binary parameter was failed.");
            }
    }
    
    using ulong = unsigned long;
    using longlong = long long;
    using ulonglong = unsigned long long;

    template<typename T, int (*fun)(const char *, int *, T*) >
    T decode_impl(char *buff, int *index) {
        T t;
        int rc = fun(buff, index, &t);
        if (rc != 0)
            throw Driver_Exception(std::string("Decoding arithmetic type (typeid=") + typeid (T).name() + ") was failed.");
        return t;
    };

#define DECODE_IMPL(Type) \
    Type decode_##Type(char *buff, int *index) { \
        return decode_impl<Type, &ei_decode_##Type>(buff, index); \
    }

    DECODE_IMPL(long);
    DECODE_IMPL(ulong);
    DECODE_IMPL(longlong);
    DECODE_IMPL(ulonglong);
    DECODE_IMPL(double);

    template<typename T, int (*fun)(ei_x_buff*, T) >
    void encode_impl(ei_x_buff *x, T t) {
        int rc = fun(x, t);
        if (rc != 0)
            throw Driver_Exception(std::string("Encoding arithmetic type (typeid=") + typeid (T).name() + ") was failed.");
    };

#define ENCODE_IMPL(Type) \
    void encode_##Type(ei_x_buff *buff, Type v) { \
        encode_impl<Type, &ei_x_encode_##Type>(buff, v); \
    }

    ENCODE_IMPL(long);
    ENCODE_IMPL(ulong);
    ENCODE_IMPL(longlong);
    ENCODE_IMPL(ulonglong);
    ENCODE_IMPL(double);
}