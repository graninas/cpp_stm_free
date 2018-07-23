#ifndef STM_STMF_STMF_H
#define STM_STMF_STMF_H

#include "../types.h"

#ifdef STM_DEBUG
#include <iostream>
#endif

namespace stm
{
namespace stmf
{

// STM methods

template <typename A, typename Next>
struct NewTVar
{
    A val;
    std::string name;
    std::function<Next(TVar<A>)> next;

    static NewTVar<Any, Next> toAny(
            const A& val,
            const std::string& name,
            const std::function<Next(TVar<A>)>& next)
    {
        std::function<Next(TVar<A>)> nextCopy = next;

        NewTVar<Any, Next> m;
        m.val = val;  // cast to any
        m.name = name;
        m.next = [=](const TVarAny& tvarAny)
        {
            TVar<A> tvar;
            tvar.id = tvarAny.id;
            tvar.name = tvarAny.name;
            return nextCopy(tvar);
        };
        return m;
    }

    ~NewTVar()
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: destructor, name: " << name << std::endl;
#endif
    }

    NewTVar()
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: empty constructor " << std::endl;
#endif
    }

    explicit NewTVar(const A& val,
                     const std::string& name,
                     const std::function<Next(TVar<A>)>& next)
        : val(val)
        , name(name)
        , next(next)
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: constructor " << std::endl;
#endif
    }

    NewTVar(const NewTVar<A, Next>& other)
        : val(other.val)
        , name(other.name)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: copy constructor, name: " << other.name << std::endl;
#endif
    }

    NewTVar(const NewTVar<A, Next>&& other)
        : val(other.val)
        , name(other.name)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: move constructor, name: " << other.name << std::endl;
#endif
    }

    NewTVar<A, Next>& operator=(NewTVar<A, Next> other)
    {

#ifdef STM_DEBUG
        std::cout << "NewTVar: copy assignment operator, name: " << other.name << std::endl;
#endif
        std::swap(val, other.val);
        std::swap(name, other.name);
        std::swap(next, other.next);
        return *this;
    }

    NewTVar<A, Next>& operator=(NewTVar<A, Next>&& other)
    {
#ifdef STM_DEBUG
        std::cout << "NewTVar: move assignment operator, name: " << other.name << std::endl;
#endif
        std::swap(val, other.val);
        std::swap(name, other.name);
        std::swap(next, other.next);
        return *this;
    }
};

template <typename A, typename Next>
struct ReadTVar
{
    TVar<A> tvar;
    std::function<Next(A)> next;

    static ReadTVar<Any, Next> toAny(
            const TVar<A>& tvar,
            const std::function<Next(A)>& next)
    {
        std::function<Next(A)> nextCopy = next;
        TVar<Any> tvar2;
        tvar2.id = tvar.id;
        tvar2.name = tvar.name;

        ReadTVar<Any, Next> m;
        m.tvar = tvar2;
        m.next = [=](const Any& val)
        {
            A val2 = std::any_cast<A>(val); // cast from any
            return nextCopy(val2);
        };
        return m;
    }

    ReadTVar()
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: empty constructor " << std::endl;
#endif
    }

    explicit ReadTVar(const TVar<A>& tvar,
                      const std::function<Next(A)>& next)
        : tvar(tvar)
        , next(next)
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: constructor " << std::endl;
#endif
    }

    ReadTVar(const ReadTVar<A, Next>& other)
        : tvar(other.tvar)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: copy constructor, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
    }

    ReadTVar(const ReadTVar<A, Next>&& other)
        : tvar(other.tvar)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: move constructor, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
    }

    ReadTVar<A, Next>& operator=(ReadTVar<A, Next> other)
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: copy assignment operator, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
        std::swap(tvar, other.tvar);
        std::swap(next, other.next);
        return *this;
    }

    ReadTVar<A, Next>& operator=(ReadTVar<A, Next>&& other)
    {
#ifdef STM_DEBUG
        std::cout << "ReadTVar: move assignment operator, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
        std::swap(tvar, other.tvar);
        std::swap(next, other.next);
        return *this;
    }
};

template <typename A, typename Next>
struct WriteTVar
{
    TVar<A> tvar;
    A val;
    std::function<Next(Unit)> next;

    static WriteTVar<Any, Next> toAny(
            const TVar<A>& tvar,
            const A& val,
            const std::function<Next(Unit)>& next)
    {
        std::function<Next(Unit)> nextCopy = next;
        TVar<Any> tvar2;
        tvar2.id = tvar.id;
        tvar2.name = tvar.name;

        WriteTVar<Any, Next> m;
        m.tvar = tvar2;
        m.val  = val; // cast to any
        m.next = [=](const Unit& unit)
        {
            return nextCopy(unit);
        };
        return m;
    }

    WriteTVar()
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: empty constructor " << std::endl;
#endif
    }

    explicit WriteTVar(const TVar<A>& tvar,
                       const A& val,
                       const std::function<Next(Unit)>& next)
        : tvar(tvar)
        , val(val)
        , next(next)
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: constructor " << std::endl;
#endif
    }

    WriteTVar(const WriteTVar<A, Next>& other)
        : tvar(other.tvar)
        , val(other.val)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: copy constructor, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
    }

    WriteTVar(const ReadTVar<A, Next>&& other)
        : tvar(other.tvar)
        , val(other.val)
        , next(other.next)
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: move constructor, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
    }

    WriteTVar<A, Next>& operator=(WriteTVar<A, Next> other)
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: copy assignment operator, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
        std::swap(tvar, other.tvar);
        std::swap(val, other.val);
        std::swap(next, other.next);
        return *this;
    }

    WriteTVar<A, Next>& operator=(WriteTVar<A, Next>&& other)
    {
#ifdef STM_DEBUG
        std::cout << "WriteTVar: move assignment operator, tvar id: "
                  << other.tvar.id
                  << ", tvar name: "
                  << other.tvar.name
                  << std::endl;
#endif
        std::swap(tvar, other.tvar);
        std::swap(val, other.val);
        std::swap(next, other.next);
        return *this;
    }
};

template <typename A, typename Next>
struct Retry
{
};

// Any-specified STM methods

template <typename Next>
using NewTVarA = NewTVar<Any, Next>;

template <typename Next>
using ReadTVarA = ReadTVar<Any, Next>;

template <typename Next>
using WriteTVarA = WriteTVar<Any, Next>;

template <typename Next>
using RetryA = Retry<Any, Next>;

// STMF algebraic data type

template <class Ret>
struct STMF
{
    std::variant<
        NewTVarA<Ret>,
        ReadTVarA<Ret>,
        WriteTVarA<Ret>,
        RetryA<Ret>
    > stmf;
};

} // namespace stmf
} // namespace stm

#endif // STM_STMF_STMF_H

