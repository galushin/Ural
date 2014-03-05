#ifndef Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED

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

/** @file ural/sequence/replace.hpp
 @brief Создание последовательностей, в которых элементы, удовлетворяющие
 определённым условиям, заменены заданным значением.
*/

#include <ural/sequence/transform.hpp>
#include <ural/functional/replace.hpp>

namespace ural
{
    template <class Sequence, class T>
    auto make_replace_sequence(Sequence && seq, T const & old_value, T const & new_value)
    -> decltype(::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_functor(old_value, new_value)))
    {
        return ::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_functor(old_value, new_value));
    }

    template <class Sequence, class Predicate, class T>
    auto make_replace_if_sequence(Sequence && seq, Predicate pred, T const & new_value)
    -> decltype(::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_if_functor(std::move(pred), new_value)))
    {
        return ::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_if_functor(std::move(pred), new_value));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
