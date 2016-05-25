#ifndef Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
#define Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED

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

/** @file ural/sequence/insertion.hpp
 @brief Последовательности на основе итераторов-вставок и других выходных
 итераторов
*/

#include <iterator>

#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Курсор на основе итераторов-вставок и других выходных итераторов
    @tparam OutputIterator тип выходного итератора
    @tparam D тип расстояния
    */
    template <class OutputIterator, class D = use_default>
    class weak_output_iterator_cursor
     : public cursor_base<weak_output_iterator_cursor<OutputIterator, D>>
    {
    public:
        // Типы
        /// @brief Тип возвращаемого значения для оператора *
        typedef decltype(*std::declval<OutputIterator>())  reference;

        /// @brief Тип расстояния
        using distance_type = experimental::defaulted_type_t<D, std::ptrdiff_t>;

        /// @brief Категория курсора
        using cursor_tag = output_cursor_tag;

        // Конструкторы
        /** @brief Конструктор
        @param iter итератор, на основе которого будет создана данная
        последовательность
        */
        explicit weak_output_iterator_cursor(OutputIterator iter)
         : iter_(std::move(iter))
        {}

        // Однопроходый курсор
        /** @brief Проверка того, что последовательность исчерпана
        @return @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        */
        reference operator*()
        {
            return *(this->iter_);
        }

        /** @brief Переход к следующему элементу
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            ++ iter_;
        }

    private:
        OutputIterator iter_;
    };

    /** @brief Вспомогательный класс для создания
    <tt> std::back_insert_iterator </tt>
    */
    struct back_inserter_helper
    {
    public:
        /** @brief Оператор вызова функции
        @param c контейнер, в который будет производится вставка
        @return <tt> std::back_insert_iterator<Container> </tt>
        */
        template <class Container>
        constexpr std::back_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::back_inserter(c);
        }
    };

    /** @brief Создание @c std::back_insert_iterator конвейерным синтаксисом
    @param c контейнера
    @return <tt> std::back_insert_iterator<Container>(c) </tt>
    */
    template <class Container>
    std::back_insert_iterator<Container>
    operator|(Container & c, back_inserter_helper helper)
    {
        return helper(c);
    }

    /** @brief Вспомогательный класс для создания
    <tt> std::front_insert_iterator </tt>
    */
    struct front_inserter_helper
    {
    public:
        /** @brief Оператор вызова функции
        @param c контейнер, в который будет производится вставка
        @return <tt> std::front_insert_iterator<Container> </tt>
        */
        template <class Container>
        constexpr std::front_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::front_inserter(c);
        }
    };

    /** @brief Создание @c std::front_insert_iterator конвейерным синтаксисом
    @param c контейнера
    @return <tt> std::front_insert_iterator<Container>(c) </tt>
    */
    template <class Container>
    std::front_insert_iterator<Container>
    operator|(Container & c, front_inserter_helper helper)
    {
        return helper(c);
    }

    /** @brief Выходной курсор, реализующия вставку в множество
    @tparam Container тип контейнера, хранящего множество
    */
    template <class Container>
    class set_insert_cursor
     : public cursor_base<set_insert_cursor<Container>>
    {
    public:
        /// @brief Категория курсора
        using cursor_tag = single_pass_cursor_tag;

        /// @brief Тип расстояния
        typedef typename Container::difference_type difference_type;

        /** @brief Конструктор
        @param c контейнер, в который будет производится вставка
        @post Выражение вида <tt> **this = value </tt> будет эквивалентно
        <tt> c.insert(value) </tt>
        */
        explicit set_insert_cursor(Container & c)
         : c_{c}
        {}

        /** @brief Доступ к контейнеру, в который производится вставка
        @return Константная ссылка на контейнер, в который производится запись
        */
        Container & container() const
        {
            return c_.get();
        }

        // Однопроходная курсор
        /** @brief Проверка исчерпания курсора
        @return @b false --- это бесконечная курсор
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Доступ к первому элементу
        @return <tt> *this </tt>
        */
        set_insert_cursor const & operator*() const
        {
            return *this;
        }

        /** @brief Оператор присваивания
        @param x добавляемое в контейнер значение
        */
        template <class T>
        void operator=(T && x) const
        {
            c_.get().insert(std::forward<T>(x));
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {};

    private:
        std::reference_wrapper<Container> c_;
    };

    /** @brief Вспомогательный класс для создания курсора вставки элементов в
    множество
    */
    struct set_inserter_helper
    {
    public:
        /** @brief Оператор вызова функции
        @param c контейнер
        @return <tt> set_insert_cursor<Container>(c) </tt>
        */
        template <class Container>
        set_insert_cursor<Container>
        operator()(Container & c) const
        {
            return set_insert_cursor<Container>{c};
        }
    };

    /** @brief Создание @c set_insert_cursor конвейерным синтаксисом
    @param c контейнера
    @return <tt> set_insert_cursor<Container>(c) </tt>
    */
    template <class Container>
    set_insert_cursor<Container>
    operator|(Container & c, set_inserter_helper)
    {
        return set_inserter_helper{}(c);
    }

    namespace
    {
        /** @brief Функциональный объект, создающий последовательность вставки
        во множество
        */
        constexpr auto const & set_inserter = odr_const<set_inserter_helper>;
    }
}
// namespace experimental

inline namespace v0
{
    namespace
    {
        /** @brief Функциональный объект, создающий итератор вставки в конец
        контейнера
        */
        constexpr auto const & back_inserter = odr_const<experimental::back_inserter_helper>;

        /** @brief Функциональный объект, создающий итератор вставки в начало
        контейнера
        */
        constexpr auto const & front_inserter = odr_const<experimental::front_inserter_helper>;
    }
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
