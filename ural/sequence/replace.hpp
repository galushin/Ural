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
    template <class Sequence, class T1, class T2, class BinaryPredicate>
    auto make_replace_sequence(Sequence && seq,
                               T1 const & old_value, T2 const & new_value,
                               BinaryPredicate bin_pred)
    -> decltype(::ural::make_transform_sequence(::ural::make_replace_function(old_value, new_value, std::move(bin_pred)),
                                                std::forward<Sequence>(seq)))
    {
        auto f = ::ural::make_replace_function(old_value, new_value,
                                               std::move(bin_pred));
        return ::ural::make_transform_sequence(std::move(f),
                                               std::forward<Sequence>(seq));

    }

    template <class Sequence, class T1, class T2>
    auto make_replace_sequence(Sequence && seq, T1 const & old_value, T2 const & new_value)
    -> decltype(::ural::make_replace_sequence(std::forward<Sequence>(seq), old_value, new_value, equal_to<>{}))
    {
        return ::ural::make_replace_sequence(std::forward<Sequence>(seq),
                                             old_value, new_value, equal_to<>{});
    }

    template <class Sequence, class Predicate, class T>
    auto make_replace_if_sequence(Sequence && seq, Predicate pred, T new_value)
    -> decltype(::ural::make_transform_sequence(make_replace_if_function(std::move(pred), std::move(new_value)),
                                                std::forward<Sequence>(seq)))
    {
        auto f = ::ural::make_replace_if_function(std::move(pred),
                                                  std::move(new_value));
        return ::ural::make_transform_sequence(std::move(f),
                                               std::forward<Sequence>(seq));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
