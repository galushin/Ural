#ifndef Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BASE_HPP_INCLUDED

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

/** @file ural/sequence/base.hpp
 @brief Базовый класс для последовательностей (CRTP)
*/

#include <ural/defs.hpp>
#include <utility>

#include <ural/sequence/sequence_iterator.hpp>

namespace ural
{
    struct single_pass_traversal_tag
    {};

    struct forward_traversal_tag
     : single_pass_traversal_tag
    {};

    struct bidirectional_traversal_tag
     : forward_traversal_tag
    {};

    struct random_access_traversal_tag
     : forward_traversal_tag
    {};

    struct finite_random_access_traversal_tag
     : random_access_traversal_tag
    {
    public:
        constexpr operator bidirectional_traversal_tag() const;
    };

    single_pass_traversal_tag
    decl_common_type(single_pass_traversal_tag, single_pass_traversal_tag);

    forward_traversal_tag
    decl_common_type(forward_traversal_tag, forward_traversal_tag);

    bidirectional_traversal_tag
    decl_common_type(bidirectional_traversal_tag, bidirectional_traversal_tag);

    random_access_traversal_tag
    decl_common_type(random_access_traversal_tag, random_access_traversal_tag);

    /** @brief Класс-характеристика для вычисления общего типа пачки тэгов
    @tparam Types типы-тэги
    */
    template <class... Types>
    struct common_tag;

    /** @brief Специализация для одного типа
    @tparam T тип
    */
    template <class T>
    struct common_tag<T>
     : declare_type<T>
    {};

    /** @brief Специализация для двух типов
    @tparam T1 первый тип
    @tparam T2 второй тип
    */
    template <class T1, class T2>
    struct common_tag<T1, T2>
     : declare_type<decltype(decl_common_type(std::declval<T1>(), std::declval<T2>()))>
    {};

    template <class Head, class... Tail>
    struct common_tag<Head, Tail...>
    {
    private:
        typedef typename common_tag<Tail...>::type tail_common_type;

    public:
        typedef typename common_tag<Head, tail_common_type>::type type;
    };

    template <class S>
    typename S::traversal_tag
    make_traversal_tag(S const & )
    {
        return typename S::traversal_tag{};
    }

    /** @brief Базовый класс для последовательностей (CRTP)
    @tparam Seq тип последовательности-наследника
    */
    template <class Seq, class Base = ural::empty_type>
    class sequence_base
     : public Base
    {
        friend Seq sequence(Seq s)
        {
            return s;
        }

        friend Seq operator++(Seq & seq, int)
        {
            Seq seq_old = seq;
            ++ seq;
            return seq_old;
        }

        friend Seq & operator++(Seq & s)
        {
            s.pop_front();
            return s;
        }

    protected:
        /** @brief Конструктор
        @param args список аргументов для конструктора базового класса
        */
        template <class... Args>
        sequence_base(Args && ... args)
         : Base(std::forward<Args>(args)...)
        {}

        sequence_base() = default;

        ~ sequence_base() = default;
    };

    template <class Seq, class Base>
    sequence_iterator<Seq> begin(sequence_base<Seq, Base> const & s)
    {
        return sequence_iterator<Seq>{static_cast<Seq const&>(s)};
    }

    template <class Seq, class Base>
    sequence_iterator<Seq> end(sequence_base<Seq, Base> const &)
    {
        return sequence_iterator<Seq>{};
    }

    /** @brief Ссылка на текущий элемент последовательности
    @param s последовательность
    @pre <tt> !s == false </tt>
    @return <tt> static_cast<Seq const&>(s).front() </tt>
    @todo Реализовать через SFINAE и is_sequence
    */
    template <class Seq, class Base>
    typename Seq::reference
    operator*(sequence_base<Seq, Base> const & s)
    {
        return static_cast<Seq const&>(s).front();
    }

    template <class Sequence>
    Sequence shrink_front(Sequence s)
    {
        s.shrink_front();
        return s;
    }

    namespace details
    {
        template <class Sequence>
        typename Sequence::distance_type
        size(Sequence const & s, single_pass_traversal_tag)
        {
            typename Sequence::distance_type n{0};

            for(auto in = s; !!in; ++ in)
            {
                ++ n;
            }

            return n;
        }

        template <class Sequence>
        typename Sequence::distance_type
        size(Sequence const & s, random_access_traversal_tag)
        {
            return s.size();
        }

        template <class Sequence>
        void advance(Sequence & s, typename Sequence::distance_type n,
                     single_pass_traversal_tag)
        {
            for(; n > 0; -- n)
            {
                ++ s;
            }
        }

        template <class Sequence>
        void advance(Sequence & s, typename Sequence::distance_type n,
                     random_access_traversal_tag)
        {
            s += n;
        }
    }

    template <class Sequence>
    typename Sequence::distance_type
    size(Sequence const & s)
    {
        return ::ural::details::size(s, ural::make_traversal_tag(s));
    }

    template <class Sequence>
    void advance(Sequence & s, typename Sequence::distance_type n)
    {
        return ::ural::details::advance(s, std::move(n),
                                        ural::make_traversal_tag(s));
    }

    template <class Sequence>
    Sequence next(Sequence s, typename Sequence::distance_type n = 1)
    {
        ::ural::advance(s, n);
        return s;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
