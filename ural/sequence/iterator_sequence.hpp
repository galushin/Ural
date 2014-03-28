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
 @todo Хранить reverse_iterator для конца интервала
*/

#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>
#include <ural/utility.hpp>

namespace ural
{
    /** @brief Строгая стратегия проверок для последовательности: возбуждает
    исключения в случае ошибок
    */
    class strict_sequence_policy
    {
    protected:
        ~strict_sequence_policy() = default;

    public:
        /** @param seq проверяемая последовательность
        @brief Проверяет, что последовательность @c seq не пуста, в противном
        случае возбуждает исключение типа <tt> logic_error </tt>.
        */
        template <class Seq>
        static void assert_not_empty(Seq const & seq)
        {
            if(!seq)
            {
                throw std::logic_error("Sequence must be not empty");
            }
        }

        template <class Seq>
        static void check_index(Seq const & seq,
                                typename Seq::distance_type index)
        {
            if(index >= seq.size())
            {
                // @todo Передавать больше информации
                throw std::logic_error("Invalid index");
            }
        }

        template <class Seq>
        static void check_step(Seq const & seq,
                               typename Seq::distance_type n)
        {
            if(n > seq.size())
            {
                // @todo Передавать больше информации
                throw std::logic_error("Invalid step size");
            }
        }
    };

    /// @cond false
    template <class Iterator>
    struct iterator_tag_to_traversal_tag;

    template <>
    struct iterator_tag_to_traversal_tag<std::input_iterator_tag>
     : declare_type<single_pass_traversal_tag>
    {};

    template <>
    struct iterator_tag_to_traversal_tag<std::forward_iterator_tag>
     : declare_type<forward_traversal_tag>
    {};

    template <>
    struct iterator_tag_to_traversal_tag<std::bidirectional_iterator_tag>
     : declare_type<bidirectional_traversal_tag>
    {};

    template <>
    struct iterator_tag_to_traversal_tag<std::random_access_iterator_tag>
     : declare_type<random_access_traversal_tag>
    {};
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
        typedef Iterator iterator;

        /// @brief Тип ссылки
        typedef typename std::iterator_traits<Iterator>::reference reference;

        /// @brief Тип значения
        typedef typename std::iterator_traits<Iterator>::value_type value_type;

        /// @brief Тип расстояния
        typedef typename std::iterator_traits<Iterator>::difference_type
            distance_type;

        typedef typename std::iterator_traits<Iterator>::iterator_category
            iterator_category;

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
         : members_{Front_type{std::move(first)}, Back_type{std::move(last)}}
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

            return *this->begin();
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            policy_type::assert_not_empty(*this);
            ++ ural::get(members_.first());
        }

        // Многопроходная прямая последовательность
        iterator_sequence traversed_front() const
        {
            return iterator_sequence{this->traversed_begin(),
                                     this->begin()};
        }

        void shrink_front()
        {
            members_.first().commit();
        }

        iterator_sequence original() const
        {
            return iterator_sequence(this->traversed_begin(),
                                     this->traversed_end());
        }

        // Двусторонняя последовательность
        void pop_back()
        {
            policy_type::assert_not_empty(*this);
            -- ural::get(members_.second());
        }

        reference back() const
        {
            policy_type::assert_not_empty(*this);
            auto tmp = this->end();
            -- tmp;
            return *tmp;
        }

        iterator_sequence traversed_back() const
        {
            return iterator_sequence(this->end(),
                                     this->traversed_end());
        }

        void shrink_back()
        {
            members_.second().commit();
        }

        // Последовательность произвольного доступа
        reference operator[](distance_type index) const
        {
            policy_type::check_index(*this, index);
            return this->begin()[index];
        }

        distance_type size() const
        {
            return this->end() - this->begin();
        }

        iterator_sequence & operator+=(distance_type n)
        {
            policy_type::check_step(*this, n);
            ural::get(members_.first()) += n;
            return *this;
        }

        void pop_back(distance_type n)
        {
            policy_type::check_step(*this, n);
            assert(n >= 0);

            ural::get(members_.second()) -= n;
        }

        // Итераторы
        iterator begin() const
        {
            return ural::get(members_.first());
        }

        iterator end() const
        {
            return ural::get(members_.second());
        }

        iterator traversed_begin() const
        {
            return members_.first().old_value();
        }

        iterator traversed_end() const
        {
            return members_.second().old_value();
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

        typedef boost::compressed_pair<Front_type, Back_type> Members;

    public:
        Members const & members() const
        {
            return this->members_;
        }

    private:
        Members members_;
    };

    template <class Iterator1, class P1, class Iterator2, class P2>
    bool operator==(iterator_sequence<Iterator1, P1> const & x,
                    iterator_sequence<Iterator2, P2> const & y)
    {
        return x.members().first() == y.members().first()
                && x.members().second() == y.members().second();
    }

    template <class Iterator>
    iterator_sequence<Iterator>
    make_iterator_sequence(Iterator first, Iterator last)
    {
        return iterator_sequence<Iterator>{first, last};
    }

    template <class Iterator, class Policy>
    iterator_sequence<Iterator, Policy>
    operator+(iterator_sequence<Iterator, Policy> i,
              typename iterator_sequence<Iterator, Policy>::distance_type n)
    {
        i += n;
        return i;
    }

}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ITERATOR_SEQUENCE_HPP_INCLUDED
