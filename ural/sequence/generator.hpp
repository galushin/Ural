#ifndef Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    template <class Generator>
    class generator_sequence
     : sequence_base<generator_sequence<Generator>,
                     decltype(make_functor(std::declval<Generator>()))>
    {
    public:
        typedef decltype(make_functor(std::declval<Generator>()))
            functor_type;

    private:
        typedef sequence_base<generator_sequence, functor_type>
            Base_class;

        template <class T>
        static T make_value(T);

    public:
        typedef decltype(std::declval<functor_type>()()) reference;
        typedef decltype(make_value(std::declval<reference>())) value_type;

        explicit generator_sequence(Generator gen)
         : Base_class{std::move(gen)}
        {}

        constexpr bool operator!() const
        {
            return false;
        }

        reference operator*() const
        {
            return this->functor()();
        }

        generator_sequence & operator++()
        {
            return *this;
        }

        functor_type const & functor() const
        {
            return *this;
        }
    };

    template <class Generator>
    generator_sequence<Generator>
    make_generator_sequence(Generator g)
    {
        return generator_sequence<Generator>{std::move(g)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
