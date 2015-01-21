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
    /** @brief Последовательности на основе итераторов-вставок и других выходных
    итераторов
    @tparam OutputIterator тип выходного итератора
    */
    template <class OutputIterator>
    class output_iterator_sequence
     : public sequence_base<output_iterator_sequence<OutputIterator>>
    {
    public:
        // Типы
        /// @brief Тип возвращаемого значения для оператора *
        typedef decltype(*std::declval<OutputIterator>())  reference;

        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        // Конструкторы
        /** @brief Конструктор
        @param iter итератор, на основе которого будет создана данная
        последовательность
        */
        explicit output_iterator_sequence(OutputIterator iter)
         : iter_(std::move(iter))
        {}

        // Однопроходная последовательность
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
        template <class Container>
        constexpr std::back_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::back_inserter(c);
        }
    };

    constexpr auto back_inserter = back_inserter_helper{};

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
    template <class Container>
        constexpr std::front_insert_iterator<Container>
        operator()(Container & c) const
        {
            return std::front_inserter(c);
        }
    };

    constexpr auto front_inserter = front_inserter_helper{};

    template <class Container>
    std::front_insert_iterator<Container>
    operator|(Container & c, front_inserter_helper helper)
    {
        return helper(c);
    }

    /** @brief Выходная последовательность реализующая вставку в множество
    @tparam Container тип контейнера, хранящего множество
    */
    template <class Container>
    class set_insert_sequence
     : public sequence_base<set_insert_sequence<Container>>
    {
    public:
        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /** @brief Конструктор
        @param c контейнер, в который будет производится вставка
        @post Выражение вида <tt> **this = value </tt> будет эквивалентно
        <tt> c.insert(value) </tt>
        */
        explicit set_insert_sequence(Container & c)
         : c_{c}
        {}

        /** @brief Доступ к контейнеру, в который производится вставка
        @return Константная ссылка на контейнер, в который производится запись
        */
        Container & container() const
        {
            return c_.get();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b false --- это бесконечная последовательность
        */
        bool operator!() const
        {
            return false;
        }

        set_insert_sequence const & operator*() const
        {
            return *this;
        }

        template <class T>
        void operator=(T && x) const
        {
            c_.get().insert(std::forward<T>(x));
        }

        void pop_front()
        {};

    private:
        std::reference_wrapper<Container> c_;
    };

    struct set_inserter_helper
    {
    public:
        template <class Container>
        ural::set_insert_sequence<Container>
        operator()(Container & c) const
        {
            return ural::set_insert_sequence<Container>{c};
        }
    };

    template <class Container>
    ural::set_insert_sequence<Container>
    operator|(Container & c, set_inserter_helper)
    {
        return ural::set_inserter_helper{}(c);
    }

    constexpr auto set_inserter = set_inserter_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_INSERTION_HPP_INCLUDED
