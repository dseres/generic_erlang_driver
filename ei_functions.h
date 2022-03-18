/* 
 * File:   ei_functions.h
 * Author: dseres
 *
 * Created on May 24, 2016, 1:46 PM
 */

#ifndef EI_FUNCTIONS_H
#define	EI_FUNCTIONS_H

#include <string>
#include <stdexcept>


typedef struct ei_x_buff_TAG ei_x_buff;

namespace generic_erlang_driver {

    class Driver_Exception: public std::invalid_argument {
    public:
        explicit Driver_Exception(const std::string& arg): invalid_argument(arg){};
        explicit Driver_Exception(const char* arg): invalid_argument(arg){};
    };
    
    struct EiType {
        int type = 0, size = 0;
        bool is_atom();
        bool is_string();
    };

    ei_x_buff * create_x_buff();
    ei_x_buff * create_x_buff_with_version();
    void free_x_buff(ei_x_buff *x);
    char *to_cbuff(ei_x_buff *x);
    int get_size(ei_x_buff *x);
    int decode_version(char *buff, int *index);
    EiType decode_type(char *buff, int *index);

    int decode_tuple_arity(char *buff, int *index);
    void encode_tuple_arity(ei_x_buff *x, long n);

    void decode_c_str(char *buff, int *index, char *str);
    std::string decode_string(char *buff, int *index);
    void encode_atom(ei_x_buff *x, std::string s);
    void encode_atom(ei_x_buff *x, const char* s);
    void encode_string(ei_x_buff *x, std::string s);
    void encode_string(ei_x_buff *x, const char* s);

    void decode_binary(char *buff, int *index, void *p, long *lenp);
    void encode_binary(ei_x_buff* x, const void* p, int len);
    
    long decode_long(char *buff, int *index);
    unsigned long decode_ulong(char *buff, int *index);
    long long decode_longlong(char *buff, int *index);
    unsigned long long decode_ulonglong(char *buff, int *index);
    double decode_double(char *buff, int *index);

    void encode_long(ei_x_buff *x, long v);
    void encode_ulong(ei_x_buff *x, unsigned long v);
    void encode_longlong(ei_x_buff *x, long long v);
    void encode_ulonglong(ei_x_buff *x, unsigned long long v);
    void encode_double(ei_x_buff *x, double v);

};
#endif	/* EI_FUNCTIONS_H */

