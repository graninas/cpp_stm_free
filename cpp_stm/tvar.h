#ifndef TVAR_H
#define TVAR_H

#include <string>
#include <functional>
#include <any>

#include "stupid_guid.h"

#define STM_DEBUG

#ifdef STM_DEBUG
#include <iostream>
#endif

namespace stm
{

using TVarId = utils::GUID;

template <typename T>
struct TVar
{
    std::string name;
    TVarId id;

//    TVar()
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: empty constructor " << std::endl;
//#endif
//    }

//    explicit TVar(const std::string& name,
//                  const TVarId& id)
//        : name(name)
//        , id(id)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: constructor " << std::endl;
//#endif
//    }

//    TVar(const TVar<T>& other)
//        : name(other.name)
//        , id(other.id)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: copy constructor, tvar id: "
//                  << other.id
//                  << ", tvar name: "
//                  << other.name
//                  << std::endl;
//#endif
//    }

//    TVar(const TVar<T>&& other)
//        : name(other.name)
//        , id(other.id)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: move constructor, tvar id: "
//                  << other.id
//                  << ", tvar name: "
//                  << other.name
//                  << std::endl;
//#endif
//    }

//    TVar<T>& operator=(TVar<T> other)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: copy assignment operator, tvar id: "
//                  << other.id
//                  << ", tvar name: "
//                  << other.name
//                  << std::endl;
//#endif
//        std::swap(name, other.name);
//        std::swap(id, other.id);
//        return *this;
//    }

//    TVar<T>& operator=(TVar<T>&& other)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: move assignment operator, tvar id: "
//                  << other.id
//                  << ", tvar name: "
//                  << other.name
//                  << std::endl;
//#endif
//        std::swap(name, other.name);
//        std::swap(id, other.id);
//        return *this;
//    }

//    friend void swap(TVar<T>& first, TVar<T>& second)
//    {
//#ifdef STM_DEBUG
//        std::cout << "TVar: swap, tvar id: "
//                  << second.id
//                  << ", tvar name: "
//                  << second.name
//                  << std::endl;
//#endif
//        std::swap(first.id, second.id);
//        std::swap(first.name, second.name);
//    }
};

} // namespace stm

#endif // TVAR_H
