#ifndef Z_URAL_ARCHETYPES_HPP_INCLUDED
#define Z_URAL_ARCHETYPES_HPP_INCLUDED

#include <ural/sequence/base.hpp>

namespace ural
{
    template <class T>
    struct value_consumer
    {
        void operator=(T const &)
        {}
    };

namespace archetypes
{
    template <class Signature>
    class functor;

    template <class R, class... Args>
    class functor<R(Args...)>
    {
    public:
        R operator()(Args...) const
        {
            return instance();
        }

    private:
        static R instance()
        {
            static R r;
            return r;
        }
    };

    template <class... Args>
    class functor<void(Args...)>
    {
    public:
        void operator()(Args...) const
        {}
    };

    template <class Value>
    class input_sequence
     : public ural::sequence_base<input_sequence<Value>>
    {
    public:
        typedef Value value_type;
        typedef value_type & reference;

        bool operator!() const
        {
            return true;
        }

        reference front() const
        {
            return this->value();
        }

        void pop_front()
        {}

    private:
        static reference value()
        {
            static value_type x;
            return x;
        }
    };
}
// namespace archetypes
}
// namespace ural

#endif
// Z_URAL_ARCHETYPES_HPP_INCLUDED
