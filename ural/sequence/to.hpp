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
#include <ural/algorithm/details/copy.hpp>

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
    <tt> sequence(std::forward<Sequence>(seq)) </tt>, а @c Value --- тип
    значений последовательности @c s.
    */
    template <class Sequence, template <class...> class Container, class... Args>
    auto operator|(Sequence && seq, to_container<Container, Args...>)
    -> Container<typename decltype(sequence(std::forward<Sequence>(seq)))::value_type, Args...>
    {
        typedef decltype(sequence(std::forward<Sequence>(seq))) Seq;
        typedef typename Seq::value_type Value;

        auto s = ural::sequence_fwd<Sequence>(seq);

        /* begin с большей вероятностью может найти применение данным,
        перемещённым из последовательности.
        Порядок объявления важен.
        Порядок вычисления аргументов функции может повлиять на корректность,
        поэтому построение итераторов вынесено в отдельные объявления
        */
        auto last = end(s);
        auto first = begin(std::move(s));

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
    -> Map<typename std::tuple_element<0, typename decltype(ural::sequence_fwd<Sequence>(seq))::value_type>::type,
           typename std::tuple_element<1, typename decltype(ural::sequence_fwd<Sequence>(seq))::value_type>::type,
           Args...>
    {
        typedef decltype(ural::sequence_fwd<Sequence>(seq)) Seq;
        typedef typename Seq::value_type Value;
        typedef typename std::tuple_element<0, Value>::type Key;
        typedef typename std::tuple_element<1, Value>::type Mapped;

        Map<Key, Mapped, Args...> result;

        for(auto && x : sequence(std::forward<Sequence>(seq)))
        {
            // @todo Оптимизация
            result.emplace(get(std::forward<decltype(x)>(x), ural::_1),
                           get(std::forward<decltype(x)>(x), ural::_2));
        }

        return result;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TO_HPP_INCLUDED
