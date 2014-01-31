#ifndef Z_URAL_RANDOM_HPP_INCLUDED
#define Z_URAL_RANDOM_HPP_INCLUDED

#include <cstdlib>

#include <ural/defs.hpp>

namespace ural
{
    class c_rand_engine
    {
    public:
        typedef decltype(std::rand()) result_type;

        result_type operator()() const
        {
            return std::rand();
        }

        constexpr result_type min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return 0;
        }

        constexpr result_type max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return RAND_MAX;
        }
    };
}
// namespace ural

#endif
// Z_URAL_RANDOM_HPP_INCLUDED
