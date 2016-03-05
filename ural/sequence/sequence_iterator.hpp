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
    template <class T, class... Other>
    struct cursor_tag_base
     : virtual Other...
    {
        friend T decl_common_type(T, T)
        {
            return T{};
        }
    };

    struct single_pass_cursor_tag
     : cursor_tag_base<single_pass_cursor_tag>
    {};

    struct input_cursor_tag
     : cursor_tag_base<input_cursor_tag, single_pass_cursor_tag>
    {};

    struct output_cursor_tag
     : cursor_tag_base<output_cursor_tag, single_pass_cursor_tag>
    {};

    struct forward_cursor_tag
     : cursor_tag_base<forward_cursor_tag, input_cursor_tag>
    {};

    struct pre_bidirectional_cursor_tag
     : cursor_tag_base<pre_bidirectional_cursor_tag, forward_cursor_tag>
    {};

    struct random_access_cursor_tag
     : cursor_tag_base<random_access_cursor_tag, forward_cursor_tag>
    {};

    struct finite_single_pass_cursor_tag
     : cursor_tag_base<finite_single_pass_cursor_tag, single_pass_cursor_tag>
    {};

    struct finite_input_cursor_tag
     : cursor_tag_base<finite_input_cursor_tag, input_cursor_tag, finite_single_pass_cursor_tag>
    {};

    struct finite_forward_cursor_tag
     : cursor_tag_base<finite_forward_cursor_tag, forward_cursor_tag, finite_input_cursor_tag>
    {};

    struct finite_pre_bidirectional_cursor_tag
     : cursor_tag_base<finite_pre_bidirectional_cursor_tag,
                       pre_bidirectional_cursor_tag, finite_forward_cursor_tag>
    {};

    struct bidirectional_cursor_tag
     : cursor_tag_base<bidirectional_cursor_tag, finite_pre_bidirectional_cursor_tag>
    {};

    struct finite_random_access_cursor_tag
     : cursor_tag_base<finite_random_access_cursor_tag, bidirectional_cursor_tag>
    {};

    finite_single_pass_cursor_tag
    decl_finite_cursor_tag(single_pass_cursor_tag);

    finite_forward_cursor_tag
    decl_finite_cursor_tag(forward_cursor_tag);

    finite_pre_bidirectional_cursor_tag
    decl_finite_cursor_tag(pre_bidirectional_cursor_tag);

    bidirectional_cursor_tag
    decl_finite_cursor_tag(bidirectional_cursor_tag);

    finite_random_access_cursor_tag
    decl_finite_cursor_tag(random_access_cursor_tag);

    template <class Tag>
    using make_finite_cursor_tag_t = decltype(decl_finite_cursor_tag(std::declval<Tag>()));

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
            assert(!!x.impl_);
            return !x.impl_.value();
        }

        typedef typename std::remove_reference<Sequence>::type Sequence_type;

        typedef std::is_same<typename Sequence_type::cursor_tag, input_cursor_tag>
            is_single_pass_t;

    public:
        // Типы
        /// @brief Категория итератора
        typedef typename std::conditional<is_single_pass_t::value,
                                          std::input_iterator_tag,
                                          std::forward_iterator_tag>::type iterator_category;

        /// @brief Тип ссылки
        typedef typename Sequence_type::reference reference;

        /// @brief Тип значения
        typedef ValueType<Sequence_type> value_type;

        /// @brief Тип указателя
        typedef typename Sequence_type::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Sequence_type> difference_type;

        // Конструктор
        /** @brief Конструктор по-умолчанию. Создаёт итератор конца
        последовательности
        */
        sequence_iterator()
         : impl_{v0::nullopt}
        {}

        /** @brief Создание начального итератора для последовательности
        @param s последовательность
        @post <tt> *this </tt> Будет посещать те же элементы, что и @c s
        */
        sequence_iterator(Sequence s)
         : impl_(std::move(s))
        {}

        /// @brief Конструктор копий
        sequence_iterator(sequence_iterator const &) = default;

        /// @brief Конструктор перемещения
        sequence_iterator(sequence_iterator &&) = default;

        //@{
        /** @brief Оператор присваивания
        @return *this
        */
        sequence_iterator & operator=(sequence_iterator const &) = default;
        sequence_iterator & operator=(sequence_iterator &&) = default;
        //@}

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
        ural::v0::optional<Sequence> impl_;
    };

    /** @brief Итератор на основе ссылки на последовательность
    @note Не должен пережить последовательность, на основе которой создан
    @tparam Sequence тип последовательности
    @param x, y аргументы
    */
    template <class Sequence>
    class sequence_iterator<Sequence&>
    {
        friend bool operator==(sequence_iterator const & x,
                               sequence_iterator const & y)
        {
            assert(!y.impl_);
            assert(!!x.impl_);
            return !x.impl_.value();
        }

    public:
        /// @brief Категория итератора
        typedef std::input_iterator_tag iterator_category;

        /// @brief Тип ссылки
        typedef typename Sequence::reference reference;

        /// @brief Тип значения
        typedef ValueType<Sequence> value_type;

        /// @brief Тип указателя
        typedef typename Sequence::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Sequence> difference_type;

        /** @brief Конструктор по-умолчанию. Создаёт итератор конца
        последовательности
        */
        sequence_iterator()
         : impl_(v0::nullopt)
        {}

        /** @brief Создание начального итератора для последовательности
        @param seq последовательность
        @post <tt> *this </tt> Будет посещать те же элементы, что и @c seq
        */
        sequence_iterator(Sequence & seq)
         : impl_(seq)
        {}

        //@{
        /// @brief Конструктор копий
        sequence_iterator(sequence_iterator const &) = default;
        sequence_iterator(sequence_iterator &&) = default;
        //@}

        //@{
        /// @brief Оператор присваивания
        sequence_iterator & operator=(sequence_iterator const &) = default;
        sequence_iterator & operator=(sequence_iterator &&) = default;
        //@}

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
        ural::v0::optional<Sequence&> impl_;
    };
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SEQUENCE_ITERATOR_HPP_INCLUDED
