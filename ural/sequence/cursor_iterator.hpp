#ifndef Z_URAL_SEQUENCE_cursor_iterator_HPP_INCLUDED
#define Z_URAL_SEQUENCE_cursor_iterator_HPP_INCLUDED
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

/** @file ural/sequence/cursor_iterator.hpp
 @brief Итератор на базе курсора. Основная цель --- интеграция с циклом @c for
 для интервалов.
*/

#include <ural/optional.hpp>
#include <ural/operators.hpp>

namespace ural
{
inline namespace v0
{
    /// @cond false
    namespace details
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
    }
    // namespace details
    /// @endcond

    struct single_pass_cursor_tag
     : details::cursor_tag_base<single_pass_cursor_tag>
    {};

    struct input_cursor_tag
     : details::cursor_tag_base<input_cursor_tag, single_pass_cursor_tag>
    {};

    struct output_cursor_tag
     : details::cursor_tag_base<output_cursor_tag, single_pass_cursor_tag>
    {};

    struct forward_cursor_tag
     : details::cursor_tag_base<forward_cursor_tag, input_cursor_tag>
    {};

    struct random_access_cursor_tag
     : details::cursor_tag_base<random_access_cursor_tag, forward_cursor_tag>
    {};

    struct finite_single_pass_cursor_tag
     : details::cursor_tag_base<finite_single_pass_cursor_tag, single_pass_cursor_tag>
    {};

    struct finite_input_cursor_tag
     : details::cursor_tag_base<finite_input_cursor_tag, input_cursor_tag, finite_single_pass_cursor_tag>
    {};

    struct finite_forward_cursor_tag
     : details::cursor_tag_base<finite_forward_cursor_tag, forward_cursor_tag, finite_input_cursor_tag>
    {};

    struct bidirectional_cursor_tag
     : details::cursor_tag_base<bidirectional_cursor_tag, finite_forward_cursor_tag>
    {};

    struct finite_random_access_cursor_tag
     : details::cursor_tag_base<finite_random_access_cursor_tag, bidirectional_cursor_tag>
    {};

    finite_single_pass_cursor_tag
    decl_finite_cursor_tag(single_pass_cursor_tag);

    finite_forward_cursor_tag
    decl_finite_cursor_tag(forward_cursor_tag);

    bidirectional_cursor_tag
    decl_finite_cursor_tag(bidirectional_cursor_tag);

    finite_random_access_cursor_tag
    decl_finite_cursor_tag(random_access_cursor_tag);

    template <class Tag>
    using make_finite_cursor_tag_t = decltype(decl_finite_cursor_tag(std::declval<Tag>()));

    /** Итератор на основе курсора. Основная цель --- интеграция с циклом @c for
    для интервалов. Измерения показывают, что данные интераторы имеют "плату
    за абстракцию" примерно 2,5. Не следует использовать этот класс в критичном
    по времени выполнения коде.
    @brief Итератор на базе курсора.
    @param Cursor курсор
    @todo Есть ли необходимость и возможность усиливать категорию итератора
    до двунаправленного и/или произвольного доступа? Можно было бы реализовать
    операции итератора произвольного доступа относительно легко. Всё становится
    сложнее для двусторонних итераторов
    @todo макрос FOR_EACH для последовательностей
    @todo Можно ли устранить специализацию?
    */
    template <class Cursor>
    class cursor_iterator
    {
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return @b true, если курсор @c x исчерпан, иначе --- @b false.
        */
        friend bool operator==(cursor_iterator const & x,
                               cursor_iterator const & y)
        {
            assert(!y.impl_);
            assert(!!x.impl_);
            return !x.impl_.value();
        }

        using Cursor_type = typename std::remove_reference<Cursor>::type;
        using is_single_pass_t = std::is_same<typename Cursor::cursor_tag, input_cursor_tag>;

    public:
        // Типы
        /// @brief Категория итератора
        using iterator_category =
            typename std::conditional<is_single_pass_t::value,
                                      std::input_iterator_tag,
                                      std::forward_iterator_tag>::type ;

        /// @brief Тип ссылки
        using reference = ReferenceType<Cursor_type>;

        /// @brief Тип значения
        using value_type = ValueType<Cursor_type>;

        /// @brief Тип указателя
        using pointer = typename Cursor_type::pointer;

        /// @brief Тип расстояния
        using difference_type = DifferenceType<Cursor_type>;

        // Конструктор
        /** @brief Конструктор по-умолчанию. Создаёт итератор конца
        последовательности, представляемой курсором.
        */
        cursor_iterator()
         : impl_{experimental::nullopt}
        {}

        /** @brief Создание начального итератора для последовательности,
        представляемой курсором.
        @param s последовательность
        @post <tt> *this </tt> Будет посещать те же элементы, что и @c s
        */
        cursor_iterator(Cursor s)
         : impl_(std::move(s))
        {}

        /// @brief Конструктор копий
        cursor_iterator(cursor_iterator const &) = default;

        /// @brief Конструктор перемещения
        cursor_iterator(cursor_iterator &&) = default;

        //@{
        /** @brief Оператор присваивания
        @return *this
        */
        cursor_iterator & operator=(cursor_iterator const &) = default;
        cursor_iterator & operator=(cursor_iterator &&) = default;
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
        cursor_iterator & operator++()
        {
            assert(!!impl_);
            ++*impl_;
            return *this;
        }

    private:
        ural::experimental::optional<Cursor> impl_;
    };

    /** @brief Итератор на основе ссылки на последовательность
    @note Не должен пережить последовательность, на основе которой создан
    @tparam Cursor тип последовательности
    @param x, y аргументы
    */
    template <class Cursor>
    class cursor_iterator<Cursor&>
    {
        friend bool operator==(cursor_iterator const & x,
                               cursor_iterator const & y)
        {
            assert(!y.impl_);
            assert(!!x.impl_);
            return !x.impl_.value();
        }

    public:
        /// @brief Категория итератора
        typedef std::input_iterator_tag iterator_category;

        /// @brief Тип ссылки
        typedef typename Cursor::reference reference;

        /// @brief Тип значения
        typedef ValueType<Cursor> value_type;

        /// @brief Тип указателя
        typedef typename Cursor::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Cursor> difference_type;

        /** @brief Конструктор по-умолчанию. Создаёт итератор конца
        последовательности
        */
        cursor_iterator()
         : impl_(experimental::nullopt)
        {}

        /** @brief Создание начального итератора для последовательности
        @param cur курсор
        @post <tt> *this </tt> Будет посещать те же элементы, что и @c seq
        */
        cursor_iterator(Cursor & cur)
         : impl_(cur)
        {}

        //@{
        /// @brief Конструктор копий
        cursor_iterator(cursor_iterator const &) = default;
        cursor_iterator(cursor_iterator &&) = default;
        //@}

        //@{
        /// @brief Оператор присваивания
        cursor_iterator & operator=(cursor_iterator const &) = default;
        cursor_iterator & operator=(cursor_iterator &&) = default;
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
        cursor_iterator & operator++()
        {
            assert(!!impl_);
            ++*impl_;
            return *this;
        }

    private:
        ural::experimental::optional<Cursor&> impl_;
    };
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_cursor_iterator_HPP_INCLUDED
