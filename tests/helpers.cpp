#include "helpers.h"

void add_to_x_buff_helper(ei_x_buff *x){
}

template <>
void get_return_value<void>(ei_x_buff *ox) {
    int index = 0;
    decode_version(to_cbuff(ox), &index);
};
