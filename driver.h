/* 
 * File:   ocilib_drv_impl.h
 * Author: dseres
 *
 * Created on March 10, 2016, 10:30 AM
 */

#ifndef OCILIB_DRV_H
#define	OCILIB_DRV_H

#include <map>
#include <string>
#include <memory>

extern "C" {
    typedef struct erl_drv_entry ErlDrvEntry;
    typedef struct _erl_drv_port* ErlDrvPort;
    ErlDrvEntry *erl_dyndriver_real_driver_init(void);
}

typedef struct ei_x_buff_TAG ei_x_buff;

class DriverTest;

namespace generic_erlang_driver {

    using std::string;
    using std::map;
    using std::unique_ptr;

    class InvokerBase;

    class Driver {
    public:
        static Driver *driver_instance;
    
        Driver(const string &drv_name);
        ~Driver();

        ErlDrvEntry *get_entry();
        /* Driver api functions*/
        void start(ErlDrvPort port);
        void stop();
        void output(char *buff, unsigned long bufflen);

    protected:
        void add_invoker(string function_name, InvokerBase *invoker);
        void add_synch_invoker(string function_name, InvokerBase *invoker);
        void add_asynch_invoker(string function_name, InvokerBase *invoker);
        void add_test_invoker(string function_name, InvokerBase *invoker, ei_x_buff **output_buffer);

    private:
        class Driver_PImpl;
        unique_ptr<Driver_PImpl> driver_pimpl;
        
        friend class ::DriverTest;
    };
    
};

#endif	/* OCILIB_DRV_H */

