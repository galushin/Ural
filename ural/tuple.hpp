#ifndef Z_URAL_TUPLE_HPP_INCLUDED
#define Z_URAL_TUPLE_HPP_INCLUDED

#include <tuple>

#include <ural/placeholders.hpp>

namespace ural
{
    template <class... Ts>
    class tuple
     : public std::tuple<Ts...>
    {
        typedef std::tuple<Ts...> Base;
    public:
        constexpr tuple()
         : Base{}
        {}

        constexpr explicit tuple(Ts const & ... arg)
         : Base(arg...)
        {}

        template <size_t Index>
        constexpr typename std::tuple_element<Index, Base>::type const &
        operator[](ural::placeholder<Index>) const
        {
            return std::get<Index>(*this);
        }
    };
}
// namespace ural

#endif
// Z_URAL_TUPLE_HPP_INCLUDED
