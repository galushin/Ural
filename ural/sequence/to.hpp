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

#include <ural/sequence/make.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/algorithm/core.hpp>

namespace ural
{
    /** @brief Вспомогательный тип для преобразования последовательностей
    в контейнер
    @tparam Container шаблон контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class...> class Container, class... Args>
    struct to_container
    {};

    /** @brief Создание контейнера по последовательности
    @tparam Container шаблон контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    @param seq последовательность
    @return <tt> Container<Value>(begin(s), end(s)) </tt>, где @c s есть
    <tt> ::ural::sequence_fwd<Sequence>(seq) </tt>, а @c Value --- тип
    значений последовательности @c s.
    */
    template <class Sequence, template <class...> class Container, class... Args>
    auto operator|(Sequence && seq, to_container<Container, Args...>)
    -> Container<ValueType<SequenceType<Sequence>>, Args...>
    {
        typedef ValueType<SequenceType<Sequence>> Value;

        auto s = ural::sequence_fwd<Sequence>(seq);

        auto first = begin(s);
        auto last = end(s);

        return Container<Value, Args...>(std::move(first), std::move(last));
    }

    /** @brief Вспомогательный тип для преобразования последовательностей
    в ассоцитивный контейнер
    @tparam Map шаблон типа ассоциативного контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class, class, class...> class Map, class... Args>
    struct to_map
    {};

    /** @brief Создание ассоциативного контейнера из последовательности
    @param seq последовательность
    @tparam Map шаблон типа ассоциативного контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <class Sequence, template <class, class, class...> class Map, class... Args>
    auto operator|(Sequence && seq, to_map<Map, Args...>)
    -> Map<typename std::tuple_element<0, ValueType<SequenceType<Sequence>>>::type,
           typename std::tuple_element<1, ValueType<SequenceType<Sequence>>>::type,
           Args...>
    {
        typedef ValueType<SequenceType<Sequence>> Value;
        typedef typename std::tuple_element<0, Value>::type Key;
        typedef typename std::tuple_element<1, Value>::type Mapped;

        Map<Key, Mapped, Args...> result;

        for(auto && x : ::ural::sequence_fwd<Sequence>(seq))
        {
            result.emplace_hint(result.end(),
                                get(std::forward<decltype(x)>(x), ural::_1),
                                get(std::forward<decltype(x)>(x), ural::_2));
        }

        return result;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TO_HPP_INCLUDED
