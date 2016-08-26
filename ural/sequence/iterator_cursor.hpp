#ifndef Z_URAL_SEQUENCE_ITERATOR_CURSOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ITERATOR_CURSOR_HPP_INCLUDED

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

/** @file ural/sequence/iterator_cursor.hpp
 @brief Курсор на основе пары итератор/страж
*/

#include <ural/container/policy.hpp>
#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>
#include <ural/utility.hpp>

namespace ural
{
inline namespace v1
{
    /// @cond false
    // @todo Автоматическое построение по списку типов
    template <class IteratorTag>
    struct iterator_tag_to_cursor_tag
    {
    private:
        static finite_input_cursor_tag make(std::input_iterator_tag);
        static finite_forward_cursor_tag make(std::forward_iterator_tag);
        static bidirectional_cursor_tag make(std::bidirectional_iterator_tag);
        static finite_random_access_cursor_tag make(std::random_access_iterator_tag);

    public:
        typedef decltype(make(IteratorTag{})) type;
    };
    /// @endcond

    /** @brief Курсор на основе пары итераторов
    @tparam Iterator тип итератора
    @tparam Policy тип политики обработки ошибок
    */
    template <class Iterator,
              class Policy = use_default>
    class iterator_cursor
     : public cursor_base<iterator_cursor<Iterator, Policy>>
    {
    public:
        // Типы
        /// @brief Тип итератора
        typedef Iterator iterator;

        /// @brief Тип стража, то есть объекта определяющего конец интервала
        using sentinel = iterator;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип указателя
        typedef typename std::iterator_traits<Iterator>::pointer pointer;

        /// @brief Тип значения
        using value_type = value_type_t<std::iterator_traits<Iterator>>;

        /// @brief Тип расстояния
        using distance_type = typename std::iterator_traits<Iterator>::difference_type;

        /// @brief Категория итератора
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

        /// @brief Категория курсора
        using cursor_tag = typename iterator_tag_to_cursor_tag<iterator_category>::type;

        /// @brief Тип политики обработки ошибок
        using policy_type = experimental::defaulted_type_t<Policy, container_checking_throw_policy>;

        // Создание, копирование, уничтожение
        /** @brief Конструктор
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть допустимым интервалом
        */
        explicit iterator_cursor(iterator first, sentinel last)
         : members_(Front_type(std::move(first)), Back_type(std::move(last)))
        {}

        /** @brief Конструктор на основе совместимого курсора
        @param cur курсор
        @post <tt> *this == seq </tt>
        */
        template <class I1, class P1>
        iterator_cursor(iterator_cursor<I1, P1> cur)
         : members_(Front_type(std::move(cur).members()[ural::_1]),
                    Back_type(std::move(cur).members()[ural::_2]))
        {}

        /** @brief Оператор присваивания совместимого курсора
        @param cur курсор
        @post <tt> *this == seq </tt>
        @return <tt> *this </tt>
        */
        template <class I1, class P1>
        iterator_cursor &
        operator=(iterator_cursor<I1, P1> cur)
        {
            this->members_[ural::_1] = std::move(cur).members()[ural::_1];
            this->members_[ural::_2] = std::move(cur).members()[ural::_2];
            return *this;
        }

        // Курсор ввода
        /** @brief Проверка исчерпания курсора
        @return @b true, если у курсора больше нет элементов, иначе --- @b false.
        */
        bool operator!() const
        {
            return this->begin() == this->end();
        }

        /** @brief Доступ к текущему (переднему) элементу
        @return Ссылка на передний элемент курсора
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            policy_type::assert_not_empty(*this);

            return *ural::experimental::get(this->members_[ural::_1]);
        }

        /** @brief Доступ к членам первого элемента
        @retun Указатель на первый элемент курсора
        */
        pointer operator->() const
        {
            return this->begin().operator->();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            policy_type::assert_not_empty(*this);
            ++ ural::experimental::get(ural::experimental::get(members_, ural::_1));
        }

        // Прямой курсор
        /** @brief Пройденная передная часть курсора
        @return Пройденная передная часть курсора
        */
        iterator_cursor
        traversed_front() const
        {
            return iterator_cursor{this->traversed_begin(), this->begin()};
        }

        /// @brief Отбросить переднюю пройденную часть курсора
        void shrink_front()
        {
            members_[ural::_1].commit();
        }

        /** @brief Исходный курсор (включая пройденные части)
        @return Исходный курсор
        */
        iterator_cursor original() const
        {
            return iterator_cursor(this->traversed_begin(),
                                     this->traversed_end());
        }

        /** @brief Исчерпание курсора за константное время в прямом порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должен быть конечным
        */
        void exhaust_front()
        {
            this->members_[ural::_1] = this->end();

            assert(!*this == true);
        }

        // Двусторонний курсор
        /** @brief Переход к следующему элементу в задней части курсора
        @pre <tt> !*this == false </tt>
        */
        void pop_back()
        {
            policy_type::assert_not_empty(*this);
            -- ::ural::experimental::get(members_[ural::_2]);
        }

        /** @brief Доступ к последнему непройденному элементу
        @pre <tt> !*this == false </tt>
        @return Ссылка на последний непройденный элемент
        */
        reference back() const
        {
            policy_type::assert_not_empty(*this);
            auto tmp = this->end();
            -- tmp;
            return *tmp;
        }

        /** @brief Пройденная задняя часть курсора
        @return Пройденная задняя часть курсора
        */
        iterator_cursor traversed_back() const
        {
            return iterator_cursor(this->end(),
                                     this->traversed_end());
        }

        /// @brief Отбросить заднюю пройденную часть курсора
        void shrink_back()
        {
            members_[ural::_2].commit();
        }

        /** @brief Исчерпание курсора за константное время в обратном порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должен быть конечным
        */
        void exhaust_back()
        {
            this->members_[ural::_2] = this->begin();

            assert(!*this == true);
        }

        // Курсор произвольного доступа
        /** @brief Индексированный доступ
        @param index индекс
        @pre <tt> 0 <= index < this->size() </tt>
        @return Ссылка на элемент с индексом @c index, считая от первого
        непройденного элемента.
        */
        reference operator[](distance_type index) const
        {
            policy_type::check_index(*this, index);
            return this->begin()[index];
        }

        /** @brief Количество непройденных элементов курсора
        @return Количество непройденных элементов курсора
        */
        distance_type size() const
        {
            return this->end() - this->begin();
        }

        /** @brief Пропуск заданного числа элементов в передней части курсора
        @param n количество элементов
        @pre <tt> 0 <= index <= this->size() </tt>
        @return <tt> *this </tt>
        */
        iterator_cursor & operator+=(distance_type n)
        {
            policy_type::check_step(*this, n);

            ::ural::experimental::get(members_[ural::_1]) += n;
            return *this;
        }

        /** @brief Пропуск заданного числа элементов в задней части курсора
        @param n количество элементов
        @pre <tt> 0 <= index <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            policy_type::check_step(*this, n);
            assert(n >= 0);

            ural::experimental::get(members_[ural::_2]) -= n;
        }

        // Итераторы
        /** @brief Начало последовательностиы
        @return Итератор, соответствующий первому непройденному элементу
        курсора
        */
        iterator const & begin() const
        {
            return ural::experimental::get(ural::experimental::get(members_, ural::_1));
        }

        /** @brief Конец последовательности
        @return Итератор на элемент, следующий за последним непройденным
        элементом последовательности
        */
        sentinel const & end() const
        {
            return ural::experimental::get(ural::experimental::get(members_, ural::_2));
        }

        /** @brief Начало исходной последовательности
        @return Итератор, задающий начало исходной последовательности
        */
        iterator const & traversed_begin() const
        {
            return ural::experimental::get(members_, ural::_1).old_value();
        }

        /** @brief Конец исходной последовательности
        @return Итератор, задающий конец исходной последовательности
        */
        sentinel const & traversed_end() const
        {
            return this->traversed_end_impl(cursor_tag{});
        }

        friend iterator begin(iterator_cursor const & s)
        {
            return s.begin();
        }

        friend sentinel end(iterator_cursor const & s)
        {
            return s.end();
        }

    private:
        sentinel const & traversed_end_impl(forward_cursor_tag) const
        {
            return members_[ural::_2];
        }

        sentinel const & traversed_end_impl(bidirectional_cursor_tag) const
        {
            return members_[ural::_2].old_value();
        }

        using Front_type = wrap_with_old_value_if_forward_t<cursor_tag, iterator>;
        using Back_type = wrap_with_old_value_if_bidirectional_t<cursor_tag, sentinel>;

        typedef tuple<Front_type, Back_type> Members;

    public:
        Members const & members() const &
        {
            return this->members_;
        }

        Members && members() &&
        {
            return std::move(this->members_);
        }

    private:
        Members members_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    */
    template <class Iterator1, class P1,
              class Iterator2, class P2>
    bool operator==(iterator_cursor<Iterator1, P1> const & x,
                    iterator_cursor<Iterator2, P2> const & y)
    {
        return x.members() == y.members();
    }

    /** @brief Функция создания @c iterator_cursor
    @param first итератор, задающий начало последовательности
    @param last итератор, задающий конец последовательности
    @pre <tt> [first; last) </tt> должен быть действительным интервалом
    @return <tt> iterator_cursor<Iterator>{first, last} </tt>
    */
    template <class Iterator>
    iterator_cursor<Iterator>
    make_iterator_cursor(Iterator first, Iterator last)
    {
        return iterator_cursor<Iterator>(std::move(first), std::move(last));
    }
}
// namespace v1
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_CURSOR_HPP_INCLUDED
