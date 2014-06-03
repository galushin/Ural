#ifndef Z_URAL_SEQUENCE_TO_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TO_HPP_INCLUDED

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

/** @file ural/sequence/to.hpp
@brief Преобразование последовательностей в контейнер
*/

#include <ural/sequence/insertion.hpp>
#include <ural/algorithm/details/copy.hpp>

namespace ural
{
    /** @brief Вспомогательный тип для преобразования последовательностей
    в контейнер
    @tparam Container шаблон контейнера
    */
    template <template <class...> class Container>
    struct to_container
    {};

    /** @brief Создание контейнера по последовательности
    @param seq последовательность
    */
    template <class Sequence, template <class...> class Container>
    auto operator|(Sequence && seq, to_container<Container>)
    -> Container<typename decltype(sequence(std::forward<Sequence>(seq)))::value_type>
    {
        typedef decltype(sequence(std::forward<Sequence>(seq))) Seq;
        typedef typename Seq::value_type Value;

        auto s = sequence(std::forward<Sequence>(seq));

        return Container<Value>(begin(s), end(s));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TO_HPP_INCLUDED
