#ifndef Z_URAL_ALGORITHM_CONTAINER_HPP_INCLUDED
#define Z_URAL_ALGORITHM_CONTAINER_HPP_INCLUDED

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

/** @file ural/algorithm/container.hpp
 @brief Алгоритмы, вносящие изменения в структуру контейнеров
 @todo Использовать функции-члены, когда они есть (см. list::remove)
*/

#include <ural/algorithm/mutating.hpp>

#include <ural/sequence/iterator_sequence.hpp>

namespace ural
{
namespace experimental
{
    /// @brief Функциональный объект для функции-члена контейнеров @c erase
    class erase_fn
    {
    public:
        /** @brief Удаление последовательности элементов из контейнера
        @param c контейнер
        @param cur курсор, задающий последовательность элементов контейнера @c c
        @return Аналог <tt> c.erase(seq.begin(), seq.end()) </tt>
        */
        template <class Container, class Iterator, class Policy>
        Container & operator()(Container & c, iterator_cursor<Iterator, Policy> const & cur) const
        {
            c.erase(cur.begin(), cur.end());
            return c;
        }
    };

    /** @brief Функциональный объект для удаления последовательных дубликатов
    из контейнера.
    */
    class unique_erase_fn
    {
    public:
        /** @brief Удаление последовательных дубликатов из контейнера
        @param c контейнер
        @param bin_pred бинарный предикат, с помощью которого определяются
        дубликаты
        @return @c c
        */
        template <class Container, class BinaryPredicate = ::ural::equal_to<>>
        Container &
        operator()(Container & c,
                   BinaryPredicate bin_pred = BinaryPredicate()) const
        {
            auto to_erase = ::ural::unique_fn{}(c, std::move(bin_pred));
            ::ural::experimental::erase_fn{}(c, to_erase);
            return c;
        }
    };

    class remove_if_erase_fn
    {
    public:
        /** Физически удаляет элементы, удовлетворяющие предикату, из контейнера
        @brief Оператор вызова функции
        @param c контейнер
        @param pred предикат
        @return @c c
        */
        template <class Container, class Predicate>
        Container & operator()(Container & c, Predicate pred) const
        {
            auto to_erase = remove_if_fn{}(c, pred);
            erase_fn{}(c, to_erase);
            return c;
        }
    };

    class remove_erase_fn
    {
    public:
        /** Физически удаляет элементы, равные заданному значению
        @brief Оператор вызова функции
        @param target контейнер
        @param value значение
        @return @c target
        */
        template <class Container, class Value>
        Container & operator()(Container & target, Value const & value) const
        {
            auto to_erase = remove_fn{}(target, value);

            erase_fn{}(target, to_erase);

            return target;
        }
    };

    /** @brief Тип функционального объекта для вставки последовательности
    элементов последовательности в начало контейнера
    */
    class push_front_fn
    {
    public:
        /** @brief Вставка последовательности значений в начало контейнера
        @param to контейнер, в который будут вставлены элементы
        @param from последовательность, элементы которой должны быть добавлены
        в контейнер.
        @return <tt> *this </tt>
        */
        template <class Container, class InputSequence>
        Container & operator()(Container & to, InputSequence && from) const
        {
            ural::copy_fn{}(std::forward<InputSequence>(from),
                            to | ural::front_inserter);
            return to;
        }
    };

    /** @brief Тип функционального объекта для вставки последовательности
    элементов последовательности в конец контейнера
    */
    class push_back_fn
    {
    public:
        /** @brief Вставка последовательности значений в конец контейнера
        @param to контейнер, в который будут вставлены элементы
        @param from последовательность, элементы которой должны быть добавлены
        в контейнер.
        @return <tt> *this </tt>
        */
        template <class Container, class InputSequence>
        Container & operator()(Container & to, InputSequence && from) const
        {
            ural::copy_fn{}(std::forward<InputSequence>(from),
                            to | ural::back_inserter);
            return to;
        }
    };

    /** @brief Тип функционального объекта для вставки последовательности
    элементов в заданную точку контейнера
    */
    class insert_fn
    {
    public:
        /** @brief Вставка последовательности значений в заданную точку
        контейнера
        @param to контейнер, в который будут вставлены элементы
        @param pos точка, перед которой должны быть вставлены элементы
        @param from последовательность, элементы которой должны быть добавлены
        в контейнер.
        @return <tt> *this </tt>
        */
        template <class Container, class Iterator, class InputSequence>
        Container & operator()(Container & to,
                               Iterator pos,
                               InputSequence && from) const
        {
            ural::copy_fn{}(std::forward<InputSequence>(from),
                            std::inserter(to, pos));
            return to;
        }
    };

    namespace
    {
        constexpr auto const & erase = odr_const<erase_fn>;

        constexpr auto const & remove_erase = odr_const<remove_erase_fn>;
        constexpr auto const & remove_if_erase = odr_const<remove_if_erase_fn>;
        constexpr auto const & remove_erase_if = remove_if_erase;

        constexpr auto const & unique_erase = odr_const<unique_erase_fn>;

        constexpr auto const & insert = odr_const<insert_fn>;
        constexpr auto const & push_front = odr_const<push_front_fn>;
        constexpr auto const & push_back = odr_const<push_back_fn>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_CONTAINER_HPP_INCLUDED
