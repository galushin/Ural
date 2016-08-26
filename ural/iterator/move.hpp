#ifndef Z_URAL_ITERATOR_MOVE_HPP_INCLUDED
#define Z_URAL_ITERATOR_MOVE_HPP_INCLUDED

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

/** @file ural/iterator/move.hpp
 @brief Итераторы с перемещением из С++17
*/

#include <ural/type_traits.hpp>
#include <ural/operators.hpp>

#include <type_traits>
#include <iterator>

namespace ural
{
inline namespace v1
{
    /** Адаптор итератора, который ведёт себя так же как базовый, за исключением
    того, что разыменование преобразует элемент, на который ссылается базовый
    итератор, в ссылку на временное значение. Некоторые алгоритмы могут вызваны
    с такими итераторами, чтобы заменить копирование на перемещение (24.5.3)
    @brief Итератор с перемещением
    @tparam Тип итератора
    @note Преобразования из/в <tt> std::move_iterator </tt> реализовывать
    нецелесообразно. Во-первых, их всегда можно выполнить с помощью вызовов
    вида <tt> make_itearator(m_iter.base()) </tt>. Во-вторых, обобщённые
    алгоритмы параметризированы типом итератора. В-третьих, данный класс введён
    из-за дефекта С++11/14, исправленного в С++17, так что преобразование в
    <tt> std::move_iterator </tt> может быть просто небезопасным.
    @note Можно заменить на <tt> std::move_iterator </tt>, если реализованы
    требования C++ 17.
    */
    template <class Iterator>
    class move_iterator
    {
        typedef typename std::iterator_traits<Iterator>::reference
            Base_reference;
    public:
        // Типы
        /// @brief Тип базового итератора
        using iterator_type = Iterator;

        /// @brief Категория итератора
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

        /// @brief Тип значений
        using value_type = value_type_t<std::iterator_traits<Iterator>>;

        /// @brief Тип разности
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;

        /// @brief Тип указателя
        using pointer = Iterator;

        /// @brief Тип ссылки
        using reference = typename moved_type<Base_reference>::type;

        // Конструкторы
        /** @brief Конструктор без аргументов
        @post <tt> this->base() == Iterator{} </tt>
        */
        move_iterator()
         : base_()
        {}

        /** @brief Конструктор
        @param i итератор
        @post <tt> this->base() == i </tt>
        */
        explicit move_iterator(Iterator const & i)
         : base_{i}
        {}

        /** @brief Конструктор на основе совместимого типа
        @param u исходный итератора
        @post <tt> this->base() == iterator_type{u.base()} </tt>
        */
        template <class U>
        move_iterator(move_iterator<U> const & u)
         : base_{u.base()}
        {}

        /** @brief Присваивание совместимого итератора
        @param u присваиваемый итератор
        @return <tt> *this </tt>
        @post <tt> this->base() == iterator_type{u.base()} </tt>
        */
        template <class U>
        move_iterator & operator=(move_iterator<U> const & u)
        {
            base_ = u.base();
            return *this;
        }

        // Адаптор итератора
        /** @brief Базовый итератор
        @return Базовый итератор
        */
        iterator_type base() const
        {
            return this->base_;
        }

        // Итератор ввода
        /** @brief Текущий элемент
        @return Текущий элемент
        */
        reference operator*() const
        {
            return static_cast<reference>(*this->base());
        }

        /** @brief Доступ к членам указываемого объекта
        @return <tt> this->base() </tt>
        */
        iterator_type operator->() const
        {
            return this->base();
        }

        /** @brief Переход к следующему элементу
        @return <tt> *this </tt>
        */
        move_iterator & operator++()
        {
            ++ this->base_;
            return *this;
        }

        // Двусторонний итератор
        /** @brief Переход к предыдущему
        @return <tt> *this </tt>
        */
        move_iterator & operator--()
        {
            -- this->base_;
            return *this;
        }

        // Префиксные операторы реализуются в <ural/operators.hpp>

        // Итератор произвольного доступа
        /** @brief Оператор "плюс"
        @param n количество элементов
        @return <tt> move_iterator(this->base() + n) </tt>
        */
        move_iterator operator+(difference_type n) const
        {
            auto result = *this;
            result += n;
            return result;
        }

        /** @brief Составное присваивание со сложением
        @param n количество элементов
        @return <tt> *this </tt>
        */
        move_iterator & operator+=(difference_type n)
        {
            this->base_ += n;
            return *this;
        }

        /** @brief Оператор "минус"
        @param n количество элементов
        @return <tt> move_iterator(this->base() - n) </tt>
        */
        move_iterator operator-(difference_type n) const
        {
            move_iterator r = *this;
            r -= n;
            return r;
        }

        /** @brief Составное присваивание с вычитанием
        @param n количество элементов
        @return <tt> *this </tt>
        */
        move_iterator & operator-=(difference_type n)
        {
            this->base_ -= n;
            return *this;
        }

        /** @brief Доступ по индексу
        @todo уточнить тип возвращаемого значения
        @param n индекс
        @return <tt> std::move(this->base()[n]) </tt>
        */
        reference operator[](difference_type n) const
        {
            return std::move(this->base()[n]);
        }

    private:
        Iterator base_;
    };

    /** @brief Функция создания @c move_iterator
    @tparam Тип итератора
    @param i итератор
    @return <tt> move_iterator<Iterator>(i) </tt>
    */
    template <class Iterator>
    move_iterator<Iterator>
    make_move_iterator(Iterator const & i)
    {
        return move_iterator<Iterator>(i);
    }

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.base() == y.base() </tt>
    */
    template <class Iterator1, class Iterator2>
    bool operator==(move_iterator<Iterator1> const & x,
                    move_iterator<Iterator2> const & y)
    {
        return x.base() == y.base();
    }

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.base() < y.base() </tt>
    */
    template <class Iterator1, class Iterator2>
    bool operator<(move_iterator<Iterator1> const & x,
                   move_iterator<Iterator2> const & y)
    {
        return x.base() < y.base();
    }

    /** @brief Бинарный оператор "минус"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.base() - y.base() </tt>
    */
    template <class Iterator1, class Iterator2>
    auto operator-(move_iterator<Iterator1> const & x,
                   move_iterator<Iterator2> const & y)
    -> decltype(x.base() - y.base())
    {
        return x.base() - y.base();
    }
}
// namespace v1
}
// namespace ural

#endif
// Z_URAL_ITERATOR_MOVE_HPP_INCLUDED
