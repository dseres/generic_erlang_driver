/* 
 * File:   Invoker.h
 * Author: dseres
 *
 * Created on March 10, 2016, 4:48 PM
 */

#ifndef INVOKER_H
#define	INVOKER_H

#include <tuple>
#include <functional>

#include "marshaller.h"


namespace generic_erlang_driver {

    using std::size_t;
    using std::tuple; 
    using std::function;
    using std::index_sequence;
    using std::index_sequence_for;
    using std::enable_if_t;
    using std::is_same;


//    // helper function to print a tuple of any size
//    template<class Tuple, std::size_t N>
//    struct TuplePrinter {
//
//        static void print(const Tuple& t) {
//            TuplePrinter<Tuple, N - 1 > ::print(t);
//            std::cout << ", " << std::get < N - 1 > (t);
//        }
//    };
//
//    template<class Tuple>
//    struct TuplePrinter<Tuple, 1> {
//
//        static void print(const Tuple& t) {
//            std::cout << std::get<0>(t);
//        }
//    };
//
//    template<class Tuple>
//    struct TuplePrinter<Tuple, 0> {
//
//        static void print(const Tuple& t) {
//        }
//    };
//    
//    template<class... Args>
//    void print(const std::tuple<Args...>& t) {
//        std::cout << "(";
//        TuplePrinter < decltype(t), sizeof...(Args)>::print(t);
//        std::cout << ")\n";
//    }

    class InvokerBase {
    public:
        virtual ~InvokerBase() = default;
        virtual int get_size()=0;
        virtual void operator()(const char *input_buffer, ei_x_buff *output_buffer) = 0;
    };

    template <typename _T, typename _T2, typename _T3 >
    class Invoker;

    template <typename R, typename ...Ts
        , template<typename> class M 
        , template<typename...> class ...Ms >
    class Invoker<R(Ts...), M<R>, tuple<Ms<Ts>...> > : public InvokerBase {
    public:
        static constexpr int size = sizeof...(Ts);
        typedef function<R(Ts...) > Function;
        typedef tuple<Ts...> Tuple;
        typedef index_sequence_for<Ts...> Indices;
        typedef M<R> ReturnMarshaller;
        typedef tuple<Ms<Ts>...> ParameterMarshallerTuple;

        Invoker(Function func) : func(Function(func)) {
        };

        Invoker(const Invoker &i) = default;
        Invoker(Invoker &&i) = default;
        Invoker &operator=(const Invoker &other) = default;
        Invoker &operator=(Invoker &&other) = default;
        ~Invoker() = default;
        
        virtual int get_size(){
            return size;
        }

        virtual void operator()(const char *input_buffer, ei_x_buff *output_buffer) {
            //Unmarshal parameters into tuple
            Tuple params = unmarshal_params((char *) input_buffer, indices);
            //Invoke function and write output
            invoke(params, output_buffer);
            //Free allocations
            free_params(move(params), indices);
        }

    private:
        Indices indices;
        Function func;
        ReturnMarshaller return_marshaller;
        ParameterMarshallerTuple param_marshallers;

        template<typename U> using enable_if_void_t = enable_if_t<(is_same<U,void>::value)>;
        template<typename U> using enable_if_not_void_t = enable_if_t<!(is_same<U,void>::value)>;
        
        template<typename U = R>
        enable_if_void_t<U> invoke(Tuple params, ei_x_buff *output_buffer){
            //Call function
            tuple_call(params, indices);
            //Marshal void value 
            return_marshaller.marshal(output_buffer);
        }
        
        template<typename U = R>
        enable_if_not_void_t<U> invoke(Tuple params, ei_x_buff *output_buffer){
            //Call function
            R ret = tuple_call(params, indices);
            //Marshal return value
            return_marshaller.marshal(output_buffer, ret);
        }
        
        template<size_t ...index>
        R tuple_call(Tuple t, index_sequence<index...>) {
            return func((get<index>(t))...);
        };

        template<size_t ...index>
        Tuple unmarshal_params(char *input_buffer, index_sequence<index...>) {
            int param_index = 0;
            Tuple params{ ((get<index>(param_marshallers)).unmarshal(input_buffer, &param_index))... };
            return params;
        }

        template<size_t ...index>
        void free_params(Tuple && t, index_sequence<index...>) {
            (void) (int []) { ((get<index>(param_marshallers)).free(get<index>(t)), 0)... };
        }
    };

    template <typename R, typename ...Ts>
    InvokerBase * create_invoker(R(*fun)(Ts...)) {
        return static_cast<InvokerBase *> (new Invoker < R(Ts...), Marshaller<R>, tuple<Marshaller<Ts>...> >(fun));
    };

    template <typename R, typename ...Ts
            , template<typename> class M >
    InvokerBase * create_invoker(R(*fun)(Ts...), M<R>) {
        return static_cast<InvokerBase *> (new Invoker < R(Ts...), M<R>, tuple<Marshaller<Ts>...> >(fun));
    };

    template <typename R, typename ...Ts
            , template<typename> class M 
            , template<typename...> class ...Ms 
            >
    auto create_invoker(R(*fun)(Ts...), M<R>, tuple<Ms<Ts>...>) -> typename std::enable_if_t< (sizeof...(Ts) > 0), InvokerBase * >
    {
        return static_cast<InvokerBase *> (new Invoker < R(Ts...), M<R>, tuple<Ms<Ts>...> >(fun));
    };    
};

#endif	/* INVOKER_H */

