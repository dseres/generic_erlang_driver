/* 
 * File:   constant_marshaller.h
 * Author: dseres
 *
 * Created on June 8, 2016, 1:39 PM
 */

#ifndef CONSTANT_MARSHALLER_H
#define	CONSTANT_MARSHALLER_H

#include "marshaller.h"
#include <map>
#include <string>

namespace generic_erlang_driver {

    template<typename T>
    class ConstantMarshaller : public Marshaller<T> {
    public:
        void marshal(ei_x_buff *x, const T &t) {
            //std::cout<<"Marshaling  "<<t<<"..."<<std::endl;
            if (values.find(t) != values.end()) {
                //std::cout<<"Marshal unsigned int: "<<t<<" to "<< values[t]<<"."<<std::endl;
                //std::cout.flush();
                encode_atom(x, values[t]);
            } else {
                //std::cout<<"Marshal unsigned int: "<<t<<" to undefined."<<std::endl;
                //std::cout.flush();
                encode_atom(x, "undefined");
            }
        };

        T unmarshal(char *buff, int *index) {
            //std::cout<<"UnMarshaling atom ..."<<std::endl;
            //std::cout.flush();
            Marshaller<char*> str_marshaller;
            char *c = str_marshaller.unmarshal(buff, index);
            std::string key(c);
            //std::cout<<"UnMarshalled key:"<<key<<"."<<std::endl;
            //std::cout.flush();
            T t;
            decltype(values2.find(key)) it;
            if ((it = values2.find(key)) != values2.end())
                t = it->second;
            //std::cout<<"Value of parameter: "<<t<<std::endl;
            //std::cout<<"Freeing resources...."<<std::endl;
            //std::cout.flush();
            str_marshaller.free(c);
            //std::cout<<"Resources have been freed."<<std::endl;
            //std::cout.flush();
            return t;
        };

    protected:
        std::map<T, std::string> values;
        std::map<std::string, T> values2;
    };
};


#endif	/* CONSTANT_MARSHALLER_H */

