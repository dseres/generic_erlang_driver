/* 
 * File:   helpers.h
 * Author: dseres
 *
 * Created on July 6, 2016, 9:42 AM
 */

#ifndef HELPERS_H
#define	HELPERS_H

#include "../marshaller.h"
#include <iostream>

using namespace generic_erlang_driver;
using namespace std;

/* Helper functions */
template <typename T, typename ...Ts>
void add_to_x_buff_helper(ei_x_buff *x, T t, Ts... args) {
    Marshaller<T> marshaller;
    marshaller.marshal(x, t);
    add_to_x_buff_helper(x, args...);
};

void add_to_x_buff_helper(ei_x_buff *x);

template <typename ...Ts>
void add_to_x_buff(ei_x_buff *x, Ts... args) {
    if ( sizeof...(Ts) > 1)
        encode_tuple_arity(x, sizeof...(Ts));
    add_to_x_buff_helper(x, args...);
};
    

template <typename R>
R get_return_value(ei_x_buff *ox) {
    int index = 0;
    decode_version(to_cbuff(ox), &index);
    Marshaller<R> marshaller;
    R ret = marshaller.unmarshal(to_cbuff(ox), &index);
    cout << "Return value : " << ret << endl;
    return ret;
};

template <>
void get_return_value<void>(ei_x_buff *ox);

#endif	/* HELPERS_H */
