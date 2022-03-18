// ocilib_drv.cpp : Defines the exported functions for the DLL application.
//
#include "driver.h"
#include <sstream>
#include <iostream>
#include "invoker.h"
#include "marshaller.h"
#include "ei_functions.h"
#include "ei.h"
//#include "tests/InvokerTest.h"

extern "C" {
    #include <erl_driver.h>
}

using namespace std;
using namespace generic_erlang_driver;

/* *****************************************************************************
********************************  Wrappers *************************************
*******************************************************************************/

class InvokerWrapper  {
public:
    InvokerWrapper(InvokerBase *invoker, ErlDrvPort *port): invoker(invoker), port(port) {};
    virtual ~InvokerWrapper() = default;
    int get_size() { return invoker->get_size(); };
    virtual void operator()(const char *input_buffer) =0; 
protected:
    unique_ptr<InvokerBase> invoker;
    ErlDrvPort *port;
};    

class AsynchInvokerWrapper : public InvokerWrapper {
public:
    AsynchInvokerWrapper(InvokerBase *invoker, ErlDrvPort *port): InvokerWrapper(invoker,port) {};
    virtual ~AsynchInvokerWrapper() = default;
    virtual void operator()(const char *input_buffer) override;
    void do_async();
    void do_ready();
    void do_free();
protected:
    ei_x_buff *output_buffer;
    const char *input_buffer;
};    

extern "C" void async_callback(void* async_data);
extern "C" void free_callback(void* async_data);

void AsynchInvokerWrapper::operator()(const char *input_buffer) {
        this->input_buffer = input_buffer;
        this->output_buffer = create_x_buff_with_version();
        driver_async(*port, NULL, async_callback, this, free_callback);
}

void AsynchInvokerWrapper::do_async(){
        (*invoker)(input_buffer, output_buffer);
}

void AsynchInvokerWrapper::do_ready() {
    driver_output(*port, output_buffer->buff, output_buffer->index);
    free_x_buff(output_buffer);
}

void AsynchInvokerWrapper::do_free() {
    free_x_buff(output_buffer);
}

class SynchInvokerWrapper : public InvokerWrapper {
public:
    SynchInvokerWrapper(InvokerBase *invoker, ErlDrvPort *port): InvokerWrapper(invoker, port) {};
    virtual ~SynchInvokerWrapper() = default;
    virtual void operator()(const char *input_buffer) override;
};    
        
void SynchInvokerWrapper::operator()(const char *input_buffer) {
        ei_x_buff *output_buffer = create_x_buff_with_version();
        (*invoker)(input_buffer, output_buffer);
        driver_output(*port, output_buffer->buff, output_buffer->index);
        free_x_buff(output_buffer);
}

class TestInvokerWrapper : public InvokerWrapper {
public:
    TestInvokerWrapper(InvokerBase *invoker, ei_x_buff **output_buffer): InvokerWrapper(invoker, nullptr), output_buffer(output_buffer) {};
    virtual ~TestInvokerWrapper() = default;
    virtual void operator()(const char *input_buffer) override;
private:
    ei_x_buff **output_buffer;
};    
        
void TestInvokerWrapper::operator()(const char *input_buffer) {
    *output_buffer = create_x_buff_with_version();
    (*invoker)(input_buffer, *output_buffer);
}

/* *****************************************************************************
************************ C driver block ****************************************
*******************************************************************************/

extern "C" {
    ErlDrvData drv_start(ErlDrvPort port, char *buff) {
        Driver::driver_instance->start(port);
        return (ErlDrvData) Driver::driver_instance;
    }

    void drv_stop(ErlDrvData drv_data) {
        Driver *driver = reinterpret_cast<Driver*>(drv_data);
        driver->stop();
    }

    void drv_output(ErlDrvData drv_data, char *buff, ErlDrvSizeT bufflen) {
        Driver *driver = reinterpret_cast<Driver*>(drv_data);
        driver->output(buff, bufflen);
    }
    
    void ready_async(ErlDrvData drv_data, ErlDrvThreadData async_data)
    {
        Driver *driver = reinterpret_cast<Driver*>(drv_data);
        AsynchInvokerWrapper *async_wrapper = reinterpret_cast<AsynchInvokerWrapper*>(async_data);
        async_wrapper->do_ready();
    }

    void async_callback(void* async_data) {
        AsynchInvokerWrapper *wrapper = reinterpret_cast<AsynchInvokerWrapper*> (async_data);
        wrapper->do_async();
    }

    void free_callback(void* async_data){
        AsynchInvokerWrapper *wrapper = reinterpret_cast<AsynchInvokerWrapper*> (async_data);
        wrapper->do_free();
    }
    
    DRIVER_INIT("generic driver")                                     
    {                                                           
        return Driver::driver_instance->get_entry();
    };                                                          
}

/* *****************************************************************************
************************** Driver implementation *******************************
*******************************************************************************/

/* Private implementation of Driver*/
class Driver::Driver_PImpl {
public:
    Driver_PImpl(const string &drv_name);
    ~Driver_PImpl();
    
private:    
    //map<string, InvokerWrapper* > invoker_wrappers;
    map<string, unique_ptr<InvokerWrapper> > invoker_wrappers;
    char driver_name[1024];
    ErlDrvEntry driver_entry;
    ErlDrvPort port;
    
    /* Helper functions */
    void output_impl(char *buff);
    string decode_function_code(char *buff, int *index);
    InvokerWrapper *find_wrapper(const string &function_code);
    void check_param_count(InvokerWrapper *wrapper, char *buff, int *index);
    void call_invoker(InvokerWrapper *wrapper, char *buff);
    void output_exception(const Driver_Exception &ex);

    friend class Driver;
    friend class ::DriverTest;
};

Driver::Driver(const string &drv_name) {
    driver_pimpl = make_unique<Driver_PImpl>(drv_name);
};

Driver::~Driver() = default;

ErlDrvEntry *Driver::get_entry(){
    return &(driver_pimpl->driver_entry);
};

void Driver::start(ErlDrvPort port) {
    set_port_control_flags(port, PORT_CONTROL_FLAG_BINARY);
    driver_pimpl->port = port;
}

void Driver::stop() {
}

void Driver::output(char *buff, unsigned long bufflen) {
    try {
        (*driver_pimpl).output_impl(buff);
    } catch (Driver_Exception ex) {
        driver_pimpl->output_exception(ex);
    }
}

Driver::Driver_PImpl::Driver_PImpl(const string &drv_name) {
    strcpy_s(driver_name, sizeof(driver_name), drv_name.c_str());
    driver_entry = ErlDrvEntry{
        NULL, /* F_PTR init, called when driver is loaded */
        drv_start, /* L_PTR start, called when port is opened */
        drv_stop, /* F_PTR stop, called when port is closed */
        drv_output, /* F_PTR output, called when erlang has sent */
        NULL, /* F_PTR ready_input, called when input descriptor ready */
        NULL, /* F_PTR ready_output, called when output descriptor ready */
        driver_name, /* char *driver_name, the argument to open_port */
        NULL, /* F_PTR finish, called when unloaded */
        NULL, /* void *handle, Reserved by VM */
        NULL, /* F_PTR control, port_command callback */
        NULL, /* F_PTR timeout, reserved */
        NULL, /* F_PTR outputv, reserved */
        ready_async, /* F_PTR ready_async, only for async drivers */
        NULL, /* F_PTR flush, called when port is about 
				       to be closed, but there is data in driver 
				       queue */
        NULL, /* F_PTR call, much like control, sync call
				      to driver */
        NULL, /* F_PTR event, called when an event selected 
				      by driver_event() occurs. */
        (int) ERL_DRV_EXTENDED_MARKER, /* int extended marker, Should always be 
				                   set to indicate driver versioning */
        ERL_DRV_EXTENDED_MAJOR_VERSION, /* int major_version, should always be 
				                       set to this value */
        ERL_DRV_EXTENDED_MINOR_VERSION, /* int minor_version, should always be 
				                       set to this value */
        0, /* int driver_flags, see documentation */
        NULL, /* void *handle2, reserved for VM use */
        NULL, /* F_PTR process_exit, called when a monitored process dies */
        NULL /* F_PTR stop_select, called to close an event object */
    };
};

Driver::Driver_PImpl::~Driver_PImpl() = default;

void Driver::Driver_PImpl::output_impl(char *buff) {
    int index = 0;
    string function_code = decode_function_code(buff, &index);
    InvokerWrapper *wrapper = find_wrapper(function_code);
    check_param_count(wrapper, buff, &index);
    call_invoker(wrapper, buff + index);
}

string Driver::Driver_PImpl::decode_function_code(char *buff, int *index){
    int version = decode_version(buff, index);
    int arity = decode_tuple_arity(buff, index);
    string function_code = decode_string(buff, index);
    cout << "Version: " << version << ", Arity: " << arity << ", Function code: " << function_code;
    return function_code;
}

InvokerWrapper *Driver::Driver_PImpl::find_wrapper(const string &function_code){
    auto it = invoker_wrappers.find(function_code);
    if ( it == invoker_wrappers.end()) {
        throw Driver_Exception( string("There is no function : ") + function_code + "." );
    }
    InvokerWrapper *wrapper = it->second.get();
    return wrapper;
}

void Driver::Driver_PImpl::check_param_count(InvokerWrapper *wrapper, char *buff, int *index){
    if (wrapper->get_size() > 1) {
        int arity = decode_tuple_arity(buff, index);
        if (arity != wrapper->get_size()) {
            ostringstream msg;
            msg<<"Arity ("<<arity<<") of parameters doesn't match to function parameters ("
                    << (wrapper->get_size()) << ").";
            throw Driver_Exception( msg.str() );
        }
    }
    cout << ", Parameter count: " << wrapper->get_size()
            << ", Buffer index: " << *index
            << "." << endl;
}
    
void Driver::Driver_PImpl::call_invoker(InvokerWrapper *wrapper, char *buff){
        (*wrapper)(buff);
}

void Driver::Driver_PImpl::output_exception(const Driver_Exception &ex) {
    ei_x_buff *x = create_x_buff_with_version();
    encode_tuple_arity(x, 2);
    encode_atom(x, "error");
    encode_tuple_arity(x, 2);
    encode_atom(x, "general_erlang_driver_error");
    encode_string(x, ex.what());
    driver_output(port, x->buff, x->index);
    free_x_buff(x);
}

void Driver::add_invoker(string function_name, InvokerBase *invoker){
    add_synch_invoker(function_name, invoker);
}

void Driver::add_synch_invoker(string function_name, InvokerBase *invoker){
    driver_pimpl->invoker_wrappers[function_name] = unique_ptr<InvokerWrapper>(new SynchInvokerWrapper(invoker, &(driver_pimpl->port)));
}

void Driver::add_asynch_invoker(string function_name, InvokerBase *invoker){
    driver_pimpl->invoker_wrappers[function_name] = unique_ptr<InvokerWrapper>(new AsynchInvokerWrapper(invoker, &(driver_pimpl->port)));
}

void Driver::add_test_invoker(string function_name, InvokerBase *invoker, ei_x_buff **output_buffer){
    driver_pimpl->invoker_wrappers[function_name] = unique_ptr<InvokerWrapper>(new TestInvokerWrapper(invoker, output_buffer));
}
