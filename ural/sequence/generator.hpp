#ifndef Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file ural/sequence/generator.hpp
 @brief Последовательность значений генератора (функции без аргументов).
*/

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    template <class Generator>
    class generator_sequence
     : public sequence_base<generator_sequence<Generator>,
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
        typedef size_t distance_type;
        typedef single_pass_traversal_tag traversal_tag;

        explicit generator_sequence(Generator gen)
         : Base_class{std::move(gen)}
        {}

        constexpr bool operator!() const
        {
            return false;
        }

        reference front() const
        {
            return this->functor()();
        }

        void pop_front()
        {}

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
