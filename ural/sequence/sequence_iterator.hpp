#ifndef Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
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

/** @file ural/sequence/sequence_iterator.hpp
 @brief Итератор на базе последовательности. Основная цель --- интеграция с
 циклом @c for для интервалов.
*/

#include <ural/optional.hpp>
#include <ural/operators.hpp>

namespace ural
{
    struct single_pass_traversal_tag
    {};

    struct forward_traversal_tag
     : single_pass_traversal_tag
    {};

    struct bidirectional_traversal_tag
     : forward_traversal_tag
    {};

    struct random_access_traversal_tag
     : forward_traversal_tag
    {};

    struct finite_random_access_traversal_tag
     : bidirectional_traversal_tag
    {
    public:
        constexpr operator random_access_traversal_tag() const;
    };

    single_pass_traversal_tag
    decl_common_type(single_pass_traversal_tag, single_pass_traversal_tag);

    forward_traversal_tag
    decl_common_type(forward_traversal_tag, forward_traversal_tag);

    bidirectional_traversal_tag
    decl_common_type(bidirectional_traversal_tag, bidirectional_traversal_tag);

    random_access_traversal_tag
    decl_common_type(random_access_traversal_tag, random_access_traversal_tag);

    /** Итератор последовательностей для интервалов. Основная цель ---
    интеграция с циклом @c for для интервалов. Измерения показывают, что
    данные интераторы имеют "плату за абстракцию" примерно 2,5.
    @brief Итератор на базе последовательности.
    @param Sequence последовательность
    @todo Есть ли необходимость и возможность усиливать категорию итератора
    до двунаправленного и/или произвольного доступа? Можно было бы реализовать
    операции итератора произвольного доступа относительно легко. Всё становится
    сложнее для двусторонних итераторов
    @todo макрос FOR_EACH для последовательностей
    */
    template <class Sequence>
    class sequence_iterator
    {
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return @b true, если последовательность @c x исчерпана, иначе
        --- @b false.
        */
        friend bool operator==(sequence_iterator const & x,
                               sequence_iterator const & y)
        {
            assert(!y.impl_);
            return !x.impl_.value();
        }

        typedef std::is_same<typename Sequence::traversal_tag, single_pass_traversal_tag>
            is_single_pass_t;

    public:
        // Типы
        /// @brief Категория итератора
        typedef typename std::conditional<is_single_pass_t::value,
                                          std::input_iterator_tag,
                                          std::forward_iterator_tag>::type iterator_category;

        /// @brief Тип ссылки
        typedef typename Sequence::reference reference;

        /// @brief Тип значения
        typedef ValueType<Sequence> value_type;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Sequence> difference_type;

        // Конструктор
        /** @brief Конструктор по-умолчанию. Создаёт итератор конца
        последовательности
        */
        sequence_iterator()
         : impl_{nullopt}
        {}

        /** @brief Создание начального итератора для последовательности
        @param s последовательность
        @post <tt> *this </tt> Будет посещать те же элементы, что и @c s
        */
        sequence_iterator(Sequence s)
         : impl_(std::move(s))
        {}

        sequence_iterator(sequence_iterator const &) = default;
        sequence_iterator(sequence_iterator &&) = default;

        sequence_iterator & operator=(sequence_iterator const &) = default;
        sequence_iterator & operator=(sequence_iterator &&) = default;

        // Итератор ввода
        /** @brief Ссылка на текущий элемент
        @return Ссылка на текущий элемент
        */
        reference operator*() const
        {
            assert(!!impl_);
            return **impl_;
        }

        /** @brief Переход к следующему элементу
        @return <tt> *this </tt>
        */
        sequence_iterator & operator++()
        {
            assert(!!impl_);
            ++*impl_;
            return *this;
        }

    private:
        ural::optional<Sequence> impl_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
