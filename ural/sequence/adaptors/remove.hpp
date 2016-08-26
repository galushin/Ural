#ifndef Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED

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

/** @file ural/sequence/remove.hpp
 @brief Адаптор последовательности, исключающий элементы базовой
 последовательности, удовлетворяющие некоторому условию
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/algorithm/core.hpp>

namespace ural
{
namespace experimental
{
    /** Адаптор последовательности, пропускающий элементы, удовлетворяющие
    предикату.
    Если базовая последовательность является однопроходной, то
    данная последовательность так же будет однопроходной. В остальных случаях
    категория курсора --- прямая (многопроходная).
    Категория произвольного доступа не может быть реализована, так как размер
    последовательности невозможно узнать, не посетив все элементы.
    Категория двусторонней последовательности требует дополнительных затрат
    при конструировании, которые не нужны (?) при типичном использовании данного
    класса.
    @brief Адаптор последовательности, пропускающий элементы, удовлетворяющие
    предикату
    @tparam Input тип входной последовательности
    @tparam Predicate тип унарного предиката
    @todo Рассмотреть целесообразность реализации возможностей двунаправленной
    последовательности. Возможно, здесь нужно поступить следующим образом:
    сделать определение положения первого и последнего элемента ленивыми.
    @todo Использование возможностей адаптора
    */
    template <class Input, class Predicate>
    class remove_if_cursor
     : public cursor_adaptor<remove_if_cursor<Input, Predicate>, Input, Predicate>
    {
        using Base = cursor_adaptor<remove_if_cursor<Input, Predicate>, Input, Predicate>;

    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        // Конструкторы
        /** @brief Конструктор
        @param input входая последовательность
        @param pred предикат
        @post <tt> this->base() == input </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        remove_if_cursor(Input input, Predicate pred)
         : Base{std::move(input), std::move(pred)}
        {
            this->seek();
        }

        // Однопроходый курсор
        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            Base::pop_front();
            this->seek();
        }

        // Прямой курсор
        remove_if_cursor original() const;

        // Двусторонняя последовательность
        remove_if_cursor traversed_back() const;

        // Адаптор
        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return Base::payload();
        }

    private:
        friend Base;

        template <class OtherInput>
        remove_if_cursor<OtherInput, Predicate>
        rebind_base(OtherInput s) const
        {
            return remove_if_cursor<OtherInput, Predicate>(std::move(s),
                                                             this->predicate());
        }

        void seek()
        {
            this->mutable_base() = find_if_not_fn{}(std::move(this->mutable_base()),
                                                    this->predicate());
        }
    };

    class make_remove_if_cursor_fn
    {
    public:
        /** @brief Функция создания @c remove_if_cursor
        @param in входная последовательность
        @param pred унарный предикат, определяющий, какие элементы должны быть
        исключены из последовательности
        @return <tt> remove_if_cursor(cursor_fwd<Input>(in), make_callable(pred)) </tt>
        */
        template <class Input, class Predicate>
        remove_if_cursor<cursor_type_t<Input>, function_type_t<Predicate>>
        operator()(Input && in, Predicate pred) const
        {
            using Cursor = remove_if_cursor<cursor_type_t<Input>, function_type_t<Predicate>>;
            return Cursor(::ural::cursor_fwd<Input>(in), make_callable(std::move(pred)));
        }
    };

    namespace
    {
        constexpr auto const & make_remove_if_cursor
            = odr_const<make_remove_if_cursor_fn>;

        constexpr auto const & removed_if
            = odr_const<pipeable_maker<make_remove_if_cursor_fn>>;
    }

    /** @brief Адаптор последовательности, исключающий элементы базовой
    последовательности, равные заданному значению
    @tparam Input Тип базовой последовательности
    @tparam T тип значения
    @tparam BinaryPredicate тип бинарного предиката, задающего равенство.
    */
    template <class Input, class T, class BinaryPredicate>
    class remove_cursor
     : public cursor_adaptor<remove_cursor<Input, T, BinaryPredicate>, Input, BinaryPredicate>
    {
        using Base = cursor_adaptor<remove_cursor<Input, T, BinaryPredicate>, Input, BinaryPredicate>;

    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        // Конструкторы
        /** @brief Конструктор
        @param in базовая последовательность
        @param value пропускаемое значение
        @param pred бинарный предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->removed_value() </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit remove_cursor(Input in, T const & value, BinaryPredicate pred)
         : Base{std::move(in), std::move(pred)}
         , old_value_(value)
        {
            this->seek();
        }

        // Однопроходый курсор
        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            Base::pop_front();
            this->seek();
        }

        // Адаптор последовательности
        /** @brief Пропускаемое значение
        @return Пропускаемое значение
        */
        T const & removed_value() const
        {
            return this->old_value_;
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return this->payload();
        }

    private:
        void seek()
        {
            this->mutable_base() = find_fn{}(this->mutable_base(),
                                             this->removed_value(),
                                             ural::not_fn(this->predicate()));
        }

    private:
        friend Base;

        template <class OtherCursor>
        remove_cursor<OtherCursor, T, BinaryPredicate>
        rebind_base(OtherCursor seq) const
        {
            using Result = remove_cursor<OtherCursor, T, BinaryPredicate>;
            return Result(std::move(seq), this->removed_value(), this->predicate());
        }

        T old_value_;
    };

    class make_remove_cursor_fn
    {
    public:
        /** @brief Создание @c remove_cursor
        @param in входная последовательность
        @param value значение, которое должно быть исключено из последовательности
        @return <tt> (*this)(std::forward<Input>(in), value, ural::equal_to<>{}); </tt>
        */
        template <class Input, class T>
        remove_cursor<cursor_type_t<Input>, typename reference_wrapper_to_reference<T>::type, ural::equal_to<>>
        operator()(Input && in, T const & value) const
        {
            return (*this)(std::forward<Input>(in), value, ural::equal_to<>{});
        }

        /** @brief Создание @c remove_cursor
        @param in входная последовательность
        @param value значение, которое должно быть исключено из последовательности
        @param pred бинарный предикат, определяющий сравнение элементов со
        значением, которое должно быть удалено
        @return <tt> remove_cursor<>(std::forward<Input>(in), value, ural::equal_to<>{}); </tt>
        */
        template <class Input, class T, class BinaryPredicate>
        auto operator()(Input && in, T const & value, BinaryPredicate pred) const
        -> remove_cursor<decltype(::ural::cursor_fwd<Input>(in)),
                           typename reference_wrapper_to_reference<T>::type,
                           decltype(::ural::make_callable(std::move(pred)))>
        {
            using Cursor = remove_cursor<decltype(::ural::cursor_fwd<Input>(in)),
                                         typename reference_wrapper_to_reference<T>::type,
                                         decltype(make_callable(std::move(pred)))>;
            return Cursor(::ural::cursor_fwd<Input>(in), value,
                          ::ural::make_callable(std::move(pred)));
        }
    };

    namespace
    {
        constexpr auto const & make_remove_cursor = odr_const<make_remove_cursor_fn>;

        constexpr auto const & removed = odr_const<pipeable_maker<make_remove_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED
