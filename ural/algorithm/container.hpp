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

/** @file ural/algorithm.hpp
 @brief Алгоритмы, вносящие изменения в структуру контейнеров
 @todo Использовать функции-члены, когда они есть (см. list::remove)
*/

#include <ural/algorithm/mutating.hpp>

#include <ural/sequence/iterator_sequence.hpp>

namespace ural
{
    /// @brief Функциональный объект для функции-члена контейнеров @c erase
    class erase_fn
    {
    public:
        /** @brief Удаление последовательности элементов из контейнера
        @param c контейнер
        @param seq последовательность элементов контейнера @c seq
        @return Аналог <tt> c.erase(seq.begin(), seq.end()) </tt>
        */
        template <class Container, class Iterator, class Policy>
        auto operator()(Container & c,
                        iterator_sequence<Iterator, Policy> seq) const
        -> typename Container::iterator
        {
            return c.erase(seq.begin(), seq.end());
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
            ::ural::erase_fn{}(c, to_erase);
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

    inline namespace
    {
        constexpr auto const erase = erase_fn{};

        constexpr auto const remove_erase = remove_erase_fn{};
        constexpr auto const remove_if_erase = remove_if_erase_fn{};

        constexpr auto const unique_erase = unique_erase_fn{};
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_CONTAINER_HPP_INCLUDED
