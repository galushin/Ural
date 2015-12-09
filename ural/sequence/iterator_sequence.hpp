#ifndef Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED

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

/** @file ural/sequence/iterator_sequence.hpp
 @brief Последовательность на основе пары итераторов
*/

#include <ural/container/policy.hpp>
#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>
#include <ural/utility.hpp>
#include <ural/meta/map.hpp>

namespace ural
{
    /// @cond false
    // @todo Автоматическое построение по списку типов
    template <class IteratorTag>
    struct iterator_tag_to_traversal_tag
    {
    private:
        static single_pass_traversal_tag make(std::input_iterator_tag);
        static forward_traversal_tag make(std::forward_iterator_tag);
        static bidirectional_traversal_tag make(std::bidirectional_iterator_tag);
        static finite_random_access_traversal_tag make(std::random_access_iterator_tag);

    public:
        typedef decltype(make(IteratorTag{})) type;
    };
    /// @endcond

    /** @brief Последовательность на основе пары итераторов
    @tparam Iterator тип итератора
    @tparam Sentinel тип стража, то есть объекта, определяющего конец
    интервала
    @tparam Policy тип политики обработки ошибок
    */
    template <class Iterator,
              class Sentinel = use_default,
              class Policy = use_default>
    class iterator_sequence
     : public sequence_base<iterator_sequence<Iterator, Sentinel, Policy>>
    {
    public:
        /// @brief Тип итератора
        typedef Iterator iterator;

        typedef typename default_helper<Sentinel, iterator>::type sentinel;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::pointer pointer;

        /// @brief Тип значения
        typedef ValueType<std::iterator_traits<Iterator>> value_type;

        /// @brief Тип расстояния
        typedef typename std::iterator_traits<Iterator>::difference_type
            distance_type;

        /// @brief Категория итератора
        typedef typename std::iterator_traits<Iterator>::iterator_category
            iterator_category;

        /// @brief Категория обхода
        typedef typename iterator_tag_to_traversal_tag<iterator_category>::type
            traversal_tag;

        /// @brief Тип политики обработки ошибок
        typedef typename default_helper<Policy, container_checking_throw_policy>::type
            policy_type;

        /** @brief Конструктор
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть допустимым интервалом
        */
        explicit iterator_sequence(iterator first, sentinel last)
         : members_(Front_type(std::move(first)), Back_type(std::move(last)))
        {}

        /** @brief Конструктор на основе совместимой последовательности
        @param seq последовательность
        @post <tt> *this == seq </tt>
        @todo Оптимизация (rvalue)
        */
        template <class I1, class S1, class P1>
        iterator_sequence(iterator_sequence<I1, S1, P1> seq)
         : members_(Front_type(std::move(seq).members()[ural::_1]),
                    Back_type(std::move(seq).members()[ural::_2]))
        {}

        /** @brief Оператор присваивания совместимой последовательности
        @param seq последовательность
        @post <tt> *this == seq </tt>
        @return <tt> *this </tt>
        */
        template <class I1, class S1, class P1>
        iterator_sequence &
        operator=(iterator_sequence<I1, S1, P1> seq)
        {
            // @todo Оптимизация (rvalue)
            this->members_[ural::_1] = std::move(seq).members()[ural::_1];
            this->members_[ural::_2] = std::move(seq).members()[ural::_2];
            return *this;
        }

        /** @brief Проверка исчерпания последовательности
        @return @b true, если в последовательности больше нет элементов,
        иначе --- @b false.
        */
        bool operator!() const
        {
            return this->begin() == this->end();
        }

        /** @brief Доступ к текущему (переднему) элементу последовательности
        @return Ссылка на передний элемент последовательности
        @pre <tt> bool(*this) != false </tt>
        */
        reference front() const
        {
            policy_type::assert_not_empty(*this);

            return *ural::get(this->members_[ural::_1]);
        }

        /** @brief Доступ к членам первого элемента последовательности
        @retun Указатель на первый элемент последовательности
        */
        pointer operator->() const
        {
            return this->begin().operator->();
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            policy_type::assert_not_empty(*this);
            ++ ural::get(ural::get(members_, ural::_1));
        }

        // Многопроходная прямая последовательность
        /** @brief Пройденная передная часть последовательности
        @return Пройденная передная часть последовательности
        */
        iterator_sequence traversed_front() const
        {
            return iterator_sequence{this->traversed_begin(),
                                     this->begin()};
        }

        /// @brief Отбросить переднюю пройденную часть последовательности
        void shrink_front()
        {
            members_[ural::_1].commit();
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        iterator_sequence original() const
        {
            return iterator_sequence(this->traversed_begin(),
                                     this->traversed_end());
        }

        /** @brief Исчерпание последовательности за константное время в прямом
        порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должна быть конечной
        */
        void exhaust_front()
        {
            this->members_[ural::_1] = this->end();

            assert(!*this == true);
        }

        // Двусторонняя последовательность
        /** @brief Переход к следующему элементу в задней части
        последовательности
        @pre <tt> !*this == false </tt>
        */
        void pop_back()
        {
            policy_type::assert_not_empty(*this);
            -- ural::get(members_[ural::_2]);
        }

        /** @brief Доступ к последнему непройденному элементу последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на последний непройденный элемент последовательности
        */
        reference back() const
        {
            policy_type::assert_not_empty(*this);
            auto tmp = this->end();
            -- tmp;
            return *tmp;
        }

        /** @brief Пройденная задняя часть последовательности
        @return Пройденная задняя часть последовательности
        */
        iterator_sequence traversed_back() const
        {
            return iterator_sequence(this->end(),
                                     this->traversed_end());
        }

        /// @brief Отбросить заднюю пройденную часть последовательности
        void shrink_back()
        {
            members_[ural::_2].commit();
        }

        /** @brief Исчерпание последовательности за константное время в обратном
        порядке
        @post <tt> !*this == true </tt>
        @pre <tt> *this </tt> должна быть конечной
        */
        void exhaust_back()
        {
            this->members_[ural::_2] = this->begin();

            assert(!*this == true);
        }

        // Последовательность произвольного доступа
        /** @brief Индексированный доступ
        @param index индекс
        @pre <tt> 0 <= index < this->size() </tt>
        @return Ссылка на элемент с индексом @c index, считая от первого
        непройденного элемента последовательности.
        */
        reference operator[](distance_type index) const
        {
            policy_type::check_index(*this, index);
            return this->begin()[index];
        }

        /** @brief Размер последовательности
        @return Размер последовательности
        */
        distance_type size() const
        {
            return this->end() - this->begin();
        }

        /** @brief Пропуск заданного числа элементов в передней части
        последовательности
        @param n количество элементов
        @pre <tt> 0 <= index <= this->size() </tt>
        @return <tt> *this </tt>
        */
        iterator_sequence & operator+=(distance_type n)
        {
            policy_type::check_step(*this, n);

            ural::get(members_[ural::_1]) += n;
            return *this;
        }

        /** @brief Пропуск заданного числа элементов в задней части
        последовательности
        @param n количество элементов
        @pre <tt> 0 <= index <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            policy_type::check_step(*this, n);
            assert(n >= 0);

            ural::get(members_[ural::_2]) -= n;
        }

        // Итераторы
        /** @brief Начало последовательности
        @return Итератор, соответствующий первому непройденному элементу
        последовательности
        */
        iterator const & begin() const
        {
            return ural::get(ural::get(members_, ural::_1));
        }

        /** @brief Конец последовательности
        @return Итератор на элемент, следующий за последним непройденным
        элементом последовательности
        */
        sentinel const & end() const
        {
            return ural::get(ural::get(members_, ural::_2));
        }

        /** @brief Начало исходной последовательности
        @return Итератор, задающий начало исходной последовательности
        */
        iterator const & traversed_begin() const
        {
            return ural::get(members_, ural::_1).old_value();
        }

        /** @brief Конец исходной последовательности
        @return Итератор, задающий конец исходной последовательности
        */
        sentinel const & traversed_end() const
        {
            return this->traversed_end_impl(traversal_tag{});
        }

        friend iterator begin(iterator_sequence const & s)
        {
            return s.begin();
        }

        friend sentinel end(iterator_sequence const & s)
        {
            return s.end();
        }

    private:
        sentinel const & traversed_end_impl(forward_traversal_tag) const
        {
            return members_[ural::_2];
        }

        sentinel const & traversed_end_impl(bidirectional_traversal_tag) const
        {
            return members_[ural::_2].old_value();
        }

        using Front_type = wrap_with_old_value_if_forward_t<traversal_tag, iterator>;
        using Back_type = wrap_with_old_value_if_bidirectional_t<traversal_tag, sentinel>;

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
    template <class Iterator1, class S1, class P1,
              class Iterator2, class S2, class P2>
    bool operator==(iterator_sequence<Iterator1, S1, P1> const & x,
                    iterator_sequence<Iterator2, S2, P2> const & y)
    {
        return x.members() == y.members();
    }

    /** @brief Функция создания @c iterator_sequence
    @param first итератор, задающий начало последовательности
    @param last итератор, задающий конец последовательности
    @pre <tt> [first; last) </tt> должен быть действительным интервалом
    @return <tt> iterator_sequence<Iterator, Sentinel>{first, last} </tt>
    */
    template <class Iterator, class Sentinel>
    iterator_sequence<Iterator, Sentinel>
    make_iterator_sequence(Iterator first, Sentinel last)
    {
        return iterator_sequence<Iterator, Sentinel>(std::move(first),
                                                     std::move(last));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
