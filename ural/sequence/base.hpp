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
     : declare_type<decltype(::ural::decl_common_type(std::declval<T1>(), std::declval<T2>()))>
    {};

    /** @brief Специализация для произвольного количества типов
    @tparam Head первый тип
    @tparam Tail список остальных типов
    */
    template <class Head, class... Tail>
    struct common_tag<Head, Tail...>
    {
    private:
        typedef typename common_tag<Tail...>::type tail_common_type;

    public:
        typedef typename common_tag<Head, tail_common_type>::type type;
    };

    /** @brief Создание объекта типа категории обхода последовательности
    @return <tt> typename S::traversal_tag{} </tt>
    */
    template <class S>
    constexpr typename S::traversal_tag
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

        //@{
        /// @brief Конструкторы копирования и перемещения
        sequence_base(sequence_base &) = default;
        sequence_base(sequence_base const &) = default;
        sequence_base(sequence_base &&) = default;
        //@}

        //@{
        /** @brief Операторы присваивания
        @return <tt> *this </tt>
        */
        sequence_base & operator=(sequence_base &) = default;
        sequence_base & operator=(sequence_base const &) = default;
        sequence_base & operator=(sequence_base &&) = default;
        //@}

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
    @todo Реализовать через SFINAE и is_sequence, использовать decltype?
    */
    template <class Seq, class Base>
    typename Seq::reference
    operator*(sequence_base<Seq, Base> const & s)
    {
        return static_cast<Seq const&>(s).front();
    }

    /** @brief Бинарный оператор "плюс" для последовательности и расстояния
    @param s последовательность
    @param n свдиг
    @todo Передавать последовательность по значению
    @return <tt> Seq{s} + n </tt>
    */
    template <class Seq, class Base>
    Seq
    operator+(sequence_base<Seq, Base> const & s, typename Seq::distance_type n)
    {
        auto result = static_cast<Seq const &>(s);
        result += n;
        return result;
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
    }
    // namespace details

    template <class Sequence>
    typename Sequence::distance_type
    size(Sequence const & s)
    {
        return ::ural::details::size(s, ural::make_traversal_tag(s));
    }

    class advance_fn
    {
    public:
        template <class Sequence>
        void operator()(Sequence & s, typename Sequence::distance_type n) const
        {
            return this->impl(s, std::move(n), ural::make_traversal_tag(s));
        }

    private:
        template <class Sequence>
        static void impl(Sequence & s, typename Sequence::distance_type n,
                         single_pass_traversal_tag)
        {
            for(; n > 0; -- n)
            {
                ++ s;
            }
        }

        template <class Sequence>
        static void impl(Sequence & s, typename Sequence::distance_type n,
                         random_access_traversal_tag)
        {
            s += n;
        }
    };
    auto constexpr advance = advance_fn{};

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
