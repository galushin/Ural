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
    /** @brief Тип функционального объекта для преобразования последовательности
    в контейнер.
    @tparam Container шаблон типа контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class...> class Container, class... Args>
    struct to_container_fn
    {
    public:
        /** @brief Создание контейнера по последовательности
        @param seq последовательность
        @return <tt> Container<Value>(begin(s), end(s)) </tt>, где @c s есть
        <tt> ::ural::sequence_fwd<Sequence>(seq) </tt>, а @c Value --- тип
        значений последовательности @c s.
        */
        template <class Sequence>
        Container<ValueType<SequenceType<Sequence>>, Args...>
        operator()(Sequence && seq) const
        {
            typedef ValueType<SequenceType<Sequence>> Value;

            auto s = ural::sequence_fwd<Sequence>(seq);

            using ::std::begin;
            using ::std::end;
            auto first = begin(s);
            auto last = end(s);

            return Container<Value, Args...>(std::move(first), std::move(last));
        }
    };

    /** @brief Вспомогательный тип для преобразования последовательностей
    в контейнер в конвейерном стиле.
    @tparam Container шаблон типа контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class...> class Container, class... Args>
    using to_container = pipeable<to_container_fn<Container, Args...>>;

    /** @brief Вспомогательный тип для преобразования последовательностей
    в ассоцитивный контейнер
    @tparam Map шаблон типа ассоциативного контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class, class, class...> class Map, class... Args>
    struct to_map_fn
    {
    public:
        /** @brief Создание ассоциативного контейнера из последовательности
        @param seq последовательность
        */
        template <class Sequence>
        Map<typename std::tuple_element<0, ValueType<SequenceType<Sequence>>>::type,
            typename std::tuple_element<1, ValueType<SequenceType<Sequence>>>::type,
            Args...>
        operator()(Sequence && seq) const
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
    };

    /** @brief Вспомогательный тип для преобразования последовательностей
    в ассоцитивный контейнер в конвейерном стиле
    @tparam Map шаблон типа ассоциативного контейнера
    @tparam Args дополнительные аругменты, например: тип функции сравнения,
    распределителя памяти и т.д.
    */
    template <template <class, class, class...> class Map, class... Args>
    using to_map = pipeable<to_map_fn<Map, Args...>>;

    /** @brief Функциональный объект для преобразования последовательности в
    контейнер с полным указанием типа контейнера:
    <tt> vector<int> v = seq | as_container </tt>
    */
    struct as_container_fn
    {
        template <class Sequence>
        class convertible_to_any_container
        {
        public:
            explicit convertible_to_any_container(Sequence && seq)
             : seq_(std::forward<Sequence>(seq))
            {}

            template <class Container>
            operator Container() const
            {
                using ::std::begin;
                using ::std::end;
                return Container(begin(seq_), end(seq_));
            }

        private:
            Sequence seq_;
        };

    public:
        /** @brief Оператор вызова функции
        @param seq последовательность
        @return Объект, определяющий оператор преобразования в любой
        @c STL-подобный контейнер.
        */
        template <class Sequence>
        convertible_to_any_container<Sequence>
        operator()(Sequence && seq) const
        {
            using Result = convertible_to_any_container<Sequence>;
            return Result(std::forward<Sequence>(seq));
        }
    };

    namespace
    {
        /** @brief Объект для преобразования последовательности в контейнер
        в конвейерном стиле
        */
        constexpr auto const & as_container
            = odr_const<pipeable<as_container_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TO_HPP_INCLUDED
