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
    /** @brief Создание последовательности, в которой элементы, эквивалентные
    заданному значению, заменены на другое значение.
    @param seq исходная последовательность
    @param old_value заменяемое значение
    @param new_value новое значение
    @param bin_pred бинарный предикат, если не указать его, будет использоваться
    оператор ==
    */
    template <class Sequence, class T1, class T2,
              class BinaryPredicate = equal_to<>>
    auto make_replace_sequence(Sequence && seq,
                               T1 const & old_value, T2 const & new_value,
                               BinaryPredicate bin_pred = BinaryPredicate())
    {
        auto f = ::ural::make_replace_function(old_value, new_value,
                                               std::move(bin_pred));
        return ::ural::make_transform_sequence(std::move(f),
                                               std::forward<Sequence>(seq));

    }

    /** @brief Создание последовательности, в которой элементы, удовлетворяющие
    заданному предикату, заменены на другое значение.
    @param seq исходная последовательность
    @param pred предикат, определяющий, какие нужно заменить.
    @param new_value новое значение
    */
    template <class Sequence, class Predicate, class T>
    auto make_replace_if_sequence(Sequence && seq, Predicate pred, T new_value)
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
