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

/** @defgroup Sequences Последовательности
*/

#include <ural/defs.hpp>
#include <ural/concepts.hpp>
#include <ural/sequence/sequence_iterator.hpp>

#include <utility>

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
    @tparam Payload класс, от которого будет закрыто наследовать данный класс.
    Может использоваться для оптимизации пустых базовых классов.
    */
    template <class Seq, class Payload = ural::empty_type>
    class sequence_base
     : private Payload
    {
        /** @brief Преобразование в последовательность
        @param seq исходная последовательность
        @return seq
        */
        friend Seq sequence(Seq seq)
        {
            return seq;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !x == false </tt>
        @return @c x
        */
        friend Seq & operator++(Seq & s)
        {
            s.pop_front();
            return s;
        }

    public:
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool empty() const
        {
            return !static_cast<Seq const &>(*this);
        }

    protected:
        /** @brief Конструктор
        @param args список аргументов для конструктора базового класса
        */
        template <class... Args>
        sequence_base(Args && ... args)
         : Payload(std::forward<Args>(args)...)
        {}

        /// @brief Конструктор без аргументов
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

        /// @brief Деструктор
        ~ sequence_base() = default;

        //@{
        Payload & payload()
        {
            return *this;
        }

        Payload const & payload() const
        {
            return *this;
        }
        //@}
    };

    template <class Seq, class Base>
    sequence_iterator<Seq &> begin(sequence_base<Seq, Base> & s)
    {
        return sequence_iterator<Seq &>(static_cast<Seq&>(s));
    }

    //@{
    /** @brief Создание конечного итератора для последовательности
    @return <tt> sequence_iterator<Seq>{} </tt>
    */
    template <class Seq, class Base>
    sequence_iterator<Seq> end(sequence_base<Seq, Base> const &)
    {
        return sequence_iterator<Seq>{};
    }

    template <class Seq, class Base>
    sequence_iterator<Seq &> end(sequence_base<Seq, Base> &)
    {
        return sequence_iterator<Seq &>{};
    }
    //@}

    /** @brief Создание начального итератора для последовательности
    @param s последовательность
    @return <tt> sequence_iterator<Seq>{static_cast<Seq const&>(s)} </tt>
    */
    template <class Seq, class Base>
    sequence_iterator<Seq> begin(sequence_base<Seq, Base> const & s)
    {
        return sequence_iterator<Seq>{static_cast<Seq const&>(s)};
    }

    template <class Seq, class Base>
    sequence_iterator<Seq> begin(sequence_base<Seq, Base> && s)
    {
        return sequence_iterator<Seq>{static_cast<Seq &&>(s)};
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

    //@{
    /** @brief Бинарный оператор "плюс" для последовательности и расстояния
    @param s последовательность
    @param n свдиг
    @todo Передавать последовательность по значению
    @return <tt> Seq{s} + n </tt>
    */
    template <class Seq, class Base>
    Seq
    operator+(sequence_base<Seq, Base> const & s, DifferenceType<Seq> n)
    {
        auto result = static_cast<Seq const &>(s);
        result += n;
        return result;
    }

    template <class Seq, class Base>
    Seq
    operator+(DifferenceType<Seq> n, sequence_base<Seq, Base> const & s)
    {
        return std::move(s + n);
    }
    //@}

    /** @brief Отбрасывание начальной пройденной части копии последовательности
    @param s последовательность
    @return Копия последовательности @c s, у которой отброшена передняя
    пройденная часть
    */
    template <class Sequence>
    Sequence shrink_front(Sequence s)
    {
        s.shrink_front();
        return s;
    }

    /** @brief Класс функционального объекта, вычисляющий размер
    массивов/контейнеров и последовательностей
    */
    class size_fn
    {
    private:
        template <class Sequence>
        static DifferenceType<Sequence>
        size_impl(Sequence const & s, single_pass_traversal_tag)
        {
            DifferenceType<Sequence> n{0};

            for(auto in = s; !!in; ++ in)
            {
                ++ n;
            }

            return n;
        }

        template <class Sequence>
        static DifferenceType<Sequence>
        size_impl(Sequence const & s, random_access_traversal_tag)
        {
            return s.size();
        }

    public:
        /** @brief Размер последовательности
        @param s последовательность
        @return Количество непройденных элементов последовательности
        */
        template <class Sequence>
        DifferenceType<Sequence>
        operator()(Sequence const & s) const
        {
            return this->size_impl(s, ural::make_traversal_tag(s));
        }

        /** @brief Размер контейнера
        @param c контейнер
        @return <tt> this->size() </tt>
        */
        template <class Container>
        typename Container::size_type
        operator()(Container const & c) const;

        /** @brief Размер массива
        @tparam N количество элементов
        @param a массив
        @return @c N
        */
        template <class T, size_t N>
        size_t operator()(T(&a)[N]) const;
    };

    /** @brief Тип функционального объекта для продвижения последовательности на
    заданное число шагов
    */
    class advance_fn
    {
    public:
        /** @brief Продвижение последовательности на заданное число шагов
        @param s последовательность
        @param n число шагов
        */
        template <class Sequence>
        void operator()(Sequence & s, DifferenceType<Sequence> n) const
        {
            return this->impl(s, std::move(n), ural::make_traversal_tag(s));
        }

    private:
        template <class Sequence>
        static void impl(Sequence & s, DifferenceType<Sequence> n,
                         single_pass_traversal_tag)
        {
            for(; n > 0; -- n)
            {
                ++ s;
            }
        }

        template <class Sequence>
        static void impl(Sequence & s, DifferenceType<Sequence> n,
                         random_access_traversal_tag)
        {
            s += n;
        }
    };

    /** @brief Тип функционального объекта для операции продвижения копии
    последовательности на заданное число шагов.
    */
    class next_fn
    {
    public:
        /** @brief Продвижение копии последовательнисти на заданное количество
        шагов
        @param s последовательность
        @param n количество шагов
        @return Копия последовательность @c s продвинутая на @c n шагов c
        помощью <tt> advance </tt>.
        */
        template <class Sequence>
        Sequence
        operator()(Sequence s, DifferenceType<Sequence> n = 1) const
        {
            ::ural::advance_fn{}(s, n);
            return s;
        }
    };

    class exhaust_front_fn
    {
    public:
        template <class Sequence>
        Sequence operator()(Sequence s) const
        {
            // @todo Добавить в концепцию
            s.exhaust_front();
            return s;
        }
    };

    namespace
    {
        /** @brief Функциональный объект для продвижения последовательности на
        заданное число шагов
        */
        constexpr auto const & advance = odr_const<advance_fn>;

        /** @brief Функциональный объект для операции продвижения копии
        последовательности на заданное число шагов.
        */
        constexpr auto const & next = odr_const<next_fn>;

        /** @brief Функциональный объект, вычисляющий размер массивов,
        контейнеров и последовательностей.
        */
        constexpr auto const & size = odr_const<size_fn>;

        constexpr auto const & exhaust_front = odr_const<exhaust_front_fn>;

        // @todo Добавить exhaust_back, в том числе в концепцию
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
