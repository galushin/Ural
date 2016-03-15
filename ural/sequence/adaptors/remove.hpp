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

    /** @brief Функция создания @c remove_if_cursor
    @param in входная последовательность
    @param pred унарный предикат, определяющий, какие элементы должны быть
    исключены из последовательности
    @return <tt> remove_if_cursor(cursor_fwd<Input>(in), make_callable(pred)) </tt>
    */
    template <class Input, class Predicate>
    remove_if_cursor<cursor_type_t<Input>, FunctionType<Predicate>>
    make_remove_if_cursor(Input && in, Predicate pred)
    {
        using Cursor = remove_if_cursor<cursor_type_t<Input>, FunctionType<Predicate>>;
        return Cursor(::ural::cursor_fwd<Input>(in), make_callable(std::move(pred)));
    }

    template <class Predicate>
    class remove_if_cursor_maker
    {
    public:
        explicit remove_if_cursor_maker(Predicate pred)
         : predicate(std::move(pred))
        {}

        Predicate predicate;
    };

    /** @brief Создание @c remove_if_cursor в конвейерном стиле
    @param in входная последовательность
    @param maker объект, хранящий предикат, определяющий элементы, которые
    должны быть исключены из входной последовательности
    */
    template <class Input, class Predicate>
    auto operator|(Input && in, remove_if_cursor_maker<Predicate> maker)
    -> decltype(::ural::experimental::make_remove_if_cursor(std::forward<Input>(in), std::move(maker.predicate)))
    {
        return ::ural::experimental::make_remove_if_cursor(std::forward<Input>(in), std::move(maker.predicate));
    }

    template <class Predicate>
    remove_if_cursor_maker<Predicate>
    removed_if(Predicate pred)
    {
        return remove_if_cursor_maker<Predicate>(std::move(pred));
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
        using cursor_tag = CommonType<typename Input::cursor_tag, finite_forward_cursor_tag>;

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

    /** @brief Функция создания @c remove_cursor
    @param in входная последовательность
    @param value значение, которое должно быть исключено из последовательности
    @param pred бинарный предикат, определяющий сравнение элементов со
    значением, которое должно быть удалено
    @return <tt> remove_cursor<>(std::forward<Input>(in), value, ural::equal_to<>{}); </tt>
    */
    template <class Input, class T, class BinaryPredicate>
    auto make_remove_cursor(Input && in, T const & value, BinaryPredicate pred)
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

    /** @brief Функция создания @c remove_cursor
    @param in входная последовательность
    @param value значение, которое должно быть исключено из последовательности
    @return <tt> make_remove_cursor(std::forward<Input>(in), value, ural::equal_to<>{}); </tt>
    */
    template <class Input, class T>
    auto make_remove_cursor(Input && in, T const & value)
    -> decltype(make_remove_cursor(std::forward<Input>(in), value, ural::equal_to<>{}))
    {
        return make_remove_cursor(std::forward<Input>(in), value, ural::equal_to<>{});
    }

    /** @brief Вспомогательный объект для конвейерного создания
    @c remove_cursor
    @tparam T тип значения, которое должно быть исключено из последовательности.
    @tparam BinaryPredicate тип бинарного предиката, используемого для
    определения значений, которые нужно исключить из последовательности.
    */
    template <class T, class BinaryPredicate>
    class remove_cursor_maker
    {
    public:
        /** @brief Конструктор
        @param x значение, которое должно быть исключено из последовательности.
        @param eq бинарный предикат
        @note В отличие от стандартного алгоритма @c remove, параметр @c x будет
        скопирован. Чтобы избежать копирования, используйте на формальном
        аргументе обёртку <tt> std::cref </tt>
        */
        explicit remove_cursor_maker(T x, BinaryPredicate eq)
         : predicate(std::move(eq))
         , value(std::move(x))
        {}

        /// @brief Предикат
        BinaryPredicate predicate;

        /// @brief Исключаемое значение
        T value;

    };

    /** @brief Конвейерное создание @c remove_cursor
    @param in входная последовательность
    @param maker вспомогательный объект, хранящий информацию об исключаемом
    значении
    */
    template <class Input, class T, class BinPred>
    auto operator|(Input && in, remove_cursor_maker<T, BinPred> maker)
    -> decltype(::ural::experimental::make_remove_cursor(std::forward<Input>(in), std::move(maker.value), std::move(maker.predicate)))
    {
        return ::ural::experimental::make_remove_cursor(std::forward<Input>(in),
                                                          std::move(maker.value),
                                                          std::move(maker.predicate));
    }

    /** @brief Создание вспомогательного объекта для конвейерного создания
    @c remove_cursor
    @param value значение, которое должно быть исключено из последовательности.
    @param bin_pred бинарный предикат, используемый для определения значений,
    которые нужно исключить из последовательности.
    @note В отличие от стандартного алгоритма @c remove, параметр @c value будет
    скопирован. Чтобы избежать копирования, используйте на формальном аргументе
    обёртку <tt> std::cref </tt>
    */
    template <class T, class BinaryPredicate>
    remove_cursor_maker<T, BinaryPredicate>
    removed(T value, BinaryPredicate bin_pred)
    {
        return remove_cursor_maker<T, BinaryPredicate>(std::move(value),
                                                         std::move(bin_pred));
    }

    /** @brief Создание вспомогательного объекта для конвейерного создания
    @c remove_cursor
    @param value значение, которое должно быть исключено из последовательности.
    @note В отличие от стандартного алгоритма @c remove, параметр @c value будет
    скопирован. Чтобы избежать копирования, используйте на формальном аргументе
    обёртку <tt> std::cref </tt>
    */
    template <class T>
    remove_cursor_maker<T, ural::equal_to<>>
    removed(T value)
    {
        return removed(std::move(value), ural::equal_to<>{});
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED
