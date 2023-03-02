#ifndef SOCK_CONNECT_DEBUG_OUTPUT_H
#define SOCK_CONNECT_DEBUG_OUTPUT_H

#include <iostream>
#include <string>
#include <typeinfo>
#include <mutex>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

static std::mutex debug_mutex;

template<class T>
std::string type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
        abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
        nullptr,
#endif
        std::free
    );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
    {
        r += " const";
    }
    if (std::is_volatile<TR>::value)
    {
        r += " volatile";
    }
    if (std::is_lvalue_reference<T>::value)
    {
        r += "&";
    }
    else if (std::is_rvalue_reference<T>::value)
    {
        r += "&&";
    }
    return r;
}

template<typename T>
std::string print_values(T *val, size_t sz)
{
    std::string os{};
    sz = sz / sizeof(T);
    if (sz < 17)
    {
        os.append("[");
        for (size_t i = 0; i < sz; i++)
        {
            if (i)
            {
                os.append("; ");
            }
            os += std::to_string(*(val + i));
        }
        os.append("]");
    }
    else
    {
        os = "[DATA SHORTENED]";
    }
    return os;
}

#endif //SOCK_CONNECT_DEBUG_OUTPUT_H
