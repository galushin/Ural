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

#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>
#include <ural/utility.hpp>
#include <ural/meta/map.hpp>

namespace ural
{
    /** @brief Класс исключения "Некорректный индекс"
    @tparam T тип индекса
    */
    template <class T>
    class bad_index
     : std::logic_error
    {
    public:
        /** @brief Конструктор
        @param msg сообщение об ошибке
        @param index индекс
        @param size размер
        */
        bad_index(char const * msg, T index, T size)
         : logic_error(msg)
         , index_(std::move(index))
         , size_(std::move(size))
        {}

    private:
        T index_;
        T size_;
    };

    /** @brief Строгая стратегия проверок для последовательности: возбуждает
    исключения в случае ошибок
    */
    class strict_sequence_policy
    {
    protected:
        ~strict_sequence_policy() = default;

    public:
        /** @brief Проверяет, что последовательность @c seq не пуста, в
        противном случае возбуждает исключение типа <tt> logic_error </tt>.
        @param seq проверяемая последовательность
        */
        template <class Seq>
        static void assert_not_empty(Seq const & seq)
        {
            if(!seq)
            {
                throw std::logic_error("Sequence must be not empty");
            }
        }

        /** @brief Проверка допустимости индекса
        @param seq последовательность
        @param index проверяемый индекс
        @throw bad_index<decltype(seq.size())>, если
        <tt> index >= seq.size() </tt>
        */
        template <class Seq>
        static void check_index(Seq const & seq,
                                typename Seq::distance_type index)
        {
            typedef typename Seq::distance_type D;

            if(index >= seq.size())
            {
                throw bad_index<D>("Invalid index", index, seq.size());
            }
        }

        /** @brief Проверка допустимости шага.
        @param seq последовательность
        @param n проверяемое значение шага
        @throw bad_index<decltype(seq.size())>, если
        <tt> index > seq.size() </tt>

        Отличается от проверки индекса тем, что может быть равен размеру
        последовательности.
        */
        template <class Seq>
        static void check_step(Seq const & seq,
                               typename Seq::distance_type n)
        {
            typedef typename Seq::distance_type D;

            if(n > seq.size())
            {
                throw bad_index<D>("Invalid step size", n, seq.size());
            }
        }
    };

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
    @tparam Policy тип политики обработки ошибок
    */
    template <class Iterator, class Policy = strict_sequence_policy>
    class iterator_sequence
     : public sequence_base<iterator_sequence<Iterator, Policy>>
    {
    public:
        /// @brief Тип итератора
        typedef Iterator iterator;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::pointer pointer;

        /// @brief Тип значения
        typedef typename std::iterator_traits<Iterator>::value_type value_type;

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
        typedef Policy policy_type;

        /** @brief Конструктор
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конец интервала
        @pre <tt> [first; last) </tt> должен быть допустимым интервалом
        */
        explicit iterator_sequence(Iterator first, Iterator last)
         : members_{Front_type{(first)}, Back_type{(last)}}
        {}

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

        // Последовательность произвольного доступа
        /** @brief Индексированный доступ
        @param index
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
        iterator const & end() const
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
        iterator const & traversed_end() const
        {
            return members_[ural::_2].old_value();
        }

        friend iterator begin(iterator_sequence const & s)
        {
            return s.begin();
        }

        friend iterator end(iterator_sequence const & s)
        {
            return s.end();
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        static auto constexpr is_bidirectional
            = std::is_convertible<traversal_tag, bidirectional_traversal_tag>::value
            || std::is_convertible<traversal_tag, random_access_traversal_tag>::value;

        typedef typename std::conditional<is_forward, with_old_value<iterator>, iterator>::type
            Front_type;
        typedef typename std::conditional<is_bidirectional, with_old_value<iterator>, iterator>::type
            Back_type;

        typedef tuple<Front_type, Back_type> Members;

    public:
        Members const & members() const
        {
            return this->members_;
        }

    private:
        Members members_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    */
    template <class Iterator1, class P1, class Iterator2, class P2>
    bool operator==(iterator_sequence<Iterator1, P1> const & x,
                    iterator_sequence<Iterator2, P2> const & y)
    {
        return x.members() == y.members();
    }

    /** @brief Функция создания @c iterator_sequence
    @param first итератор, задающий начало последовательности
    @param last итератор, задающий конец последовательности
    @pre <tt> [first; last) </tt> должен быть действительным интервалом
    @return <tt> iterator_sequence<Iterator>{first, last} </tt>
    */
    template <class Iterator>
    iterator_sequence<Iterator>
    make_iterator_sequence(Iterator first, Iterator last)
    {
        return iterator_sequence<Iterator>{first, last};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
