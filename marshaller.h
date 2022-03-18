/* 
 * File:   Marshaller.h
 * Author: dseres
 *
 * Created on April 29, 2016, 12:57 PM
 */

#ifndef MARSHALLERS_H
#define	MARSHALLERS_H

#include <type_traits>
#include <utility>
#include <functional>

#include "ei_functions.h"

namespace generic_erlang_driver {

    using namespace std;

    template<typename T, typename = enable_if_t< is_arithmetic<T>::value > >
    struct ArithmeticHelper {
        using ulong = unsigned long;
        using longlong = long long;
        using ulonglong = unsigned long long;

        static constexpr bool is_long_v = is_integral<T>::value && is_signed<T>::value && sizeof (T) <= sizeof (long);
        static constexpr bool is_ulong_v = is_integral<T>::value && is_unsigned<T>::value && sizeof (T) <= sizeof (long);
        static constexpr bool is_longlong_v = is_integral<T>::value && is_signed<T>::value && sizeof (T) > sizeof (long);
        static constexpr bool is_ulonglong_v = is_integral<T>::value && is_unsigned<T>::value && sizeof (T) > sizeof (long);
        static constexpr bool is_float_v = is_floating_point<T>::value;
        static constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        using conditional5_t =
        conditional_t< is_long_v, T1,
        conditional_t< is_ulong_v, T2,
        conditional_t< is_longlong_v, T3,
        conditional_t< is_ulonglong_v, T4,
        conditional_t< is_float_v, T5, T6 > > > > >;
        typedef integral_constant<nullptr_t, nullptr> null_integral;

        typedef conditional5_t<long, ulong, longlong, ulonglong, double, nullptr_t> ei_type;
        typedef void (*encode_function_ptr_t)(ei_x_buff *, ei_type);
        typedef ei_type(*decode_function_ptr_t)(char *, int *);
        typedef function< remove_pointer_t<encode_function_ptr_t> > encode_function_t;
        typedef function< remove_pointer_t<decode_function_ptr_t> > decode_function_t;

        static constexpr encode_function_t get_encode_function() {
            typedef integral_constant< void(*)(ei_x_buff *, long), &encode_long > encode_long_type;
            typedef integral_constant< void(*)(ei_x_buff *, ulong), &encode_ulong > encode_ulong_type;
            typedef integral_constant< void(*)(ei_x_buff *, longlong), &encode_longlong > encode_longlong_type;
            typedef integral_constant< void(*)(ei_x_buff *, ulonglong), &encode_ulonglong > encode_ulonglong_type;
            typedef integral_constant< void(*)(ei_x_buff *, double), &encode_double > encode_double_type;
            return conditional5_t<encode_long_type, encode_ulong_type, encode_longlong_type, encode_ulonglong_type, encode_double_type, null_integral>();
        }

        static constexpr decode_function_t get_decode_function() {
            typedef integral_constant< long(*)(char *, int *), &decode_long > decode_long_type;
            typedef integral_constant < ulong(*)(char *, int *), &decode_ulong > decode_ulong_type;
            typedef integral_constant < longlong(*)(char *, int *), &decode_longlong > decode_longlong_type;
            typedef integral_constant < ulonglong(*)(char *, int *), &decode_ulonglong > decode_ulonglong_type;
            typedef integral_constant< double(*)(char *, int *), &decode_double > decode_double_type;
            return conditional5_t<decode_long_type, decode_ulong_type, decode_longlong_type, decode_ulonglong_type, decode_double_type, null_integral>();
        }
    };

    template<typename T, typename = void>
    class MarshallerBase {
    public:

        void marshal(ei_x_buff *x, const T &t) {
            encode_binary(x, static_cast<const void *> (&t), sizeof (T));
        }

        T unmarshal(char *buff, int *index) {
            EiType type = decode_type(buff, index);
            if (type.size != sizeof (T)) {
                throw invalid_argument("Length of binary parameter is not same as length of type.");
            }
            long len = 0;
            T t;
            decode_binary(buff, index, static_cast<void *> (&t), &len);
            return t;
        }

        void free(T) {
        }
    };

    template<>
    class MarshallerBase<void, void> {
    public:

        void marshal(ei_x_buff *x) {
        }

        void unmarshal(char *buff, int *index) {
        }

        void free() {
        }
    };
    
    /* Arithmetic marshaller*/
    template<typename T>
    class MarshallerBase<T, enable_if_t< ArithmeticHelper<T>::is_arithmetic_v > > {
    public:

        void marshal(ei_x_buff *x, const T &t) {
            auto encode = ArithmeticHelper<T>::get_encode_function();
            encode(x, static_cast<typename ArithmeticHelper<T>::ei_type> (t));
        }

        T unmarshal(char *buff, int *index) {
            auto decode = ArithmeticHelper<T>::get_decode_function();
            typename ArithmeticHelper<T>::ei_type t = decode(buff, index);
            return static_cast<T> (t);
        }

        void free(T) {
        }
    };

    template<typename T>
    struct enable_char_pointer : enable_if< is_same<T, const char*>::value || is_same<T, char*>::value > {
    };

    /* Char* marshaller*/
    template<typename T>
    class MarshallerBase<T, typename enable_char_pointer<T>::type > {
    public:

        void marshal(ei_x_buff *x, const T &c) {
            encode_string(x, c);
        }

        T unmarshal(char *buff, int *index) {
            EiType type = decode_type(buff, index);
            char *str = new char[type.size + 1];
            //memset(str,0,type.size+1);
            decode_c_str(buff, index, str);
            return str;
        }

        void free(T c) {
            delete (char *) c;
        }
    };

    template <typename T>
    class Marshaller : public MarshallerBase<T> {
    };

};


#endif	/* MARSHALLERS_H */

