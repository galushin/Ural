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
inline namespace v0
{
    template <class T>
    T decl_common_type(T, T);

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

    template <class... Types>
    using common_tag_t = typename common_tag<Types...>::type;

    /** @brief Создание объекта типа категории курсора
    @return <tt> typename S::cursor_tag{} </tt>
    */
    template <class S>
    constexpr typename S::cursor_tag
    make_cursor_tag(S const & )
    {
        return typename S::cursor_tag{};
    }

    /** @brief Класс-характеристика, оборачивающая @c T в @c with_old_value,
    если @c CursorTag --- forward_cursor_tag или более сильная категория
    курсора
    @tparam CursorTag желаемая категория курсора
    @tparam T тип
    */
    template <class CursorTag, class T>
    struct wrap_with_old_value_if_forward
     : std::conditional<std::is_convertible<CursorTag, forward_cursor_tag>::value,
                        with_old_value<T>, T>
    {};

    /** @brief Синоним для <tt> wrap_with_old_value_if_forward<CursorTag, T>::type </tt>
    @tparam CursorTag желаемая категория курсора
    @tparam T тип
    */
    template <class CursorTag, class T>
    using wrap_with_old_value_if_forward_t
        = typename wrap_with_old_value_if_forward<CursorTag, T>::type;

    /** @brief Класс-характеристика, оборачивающая @c T в @c with_old_value,
    если @c CursorTag --- bidirectional_traversal_tag или более сильная
    категория
    @tparam CursorTag желаемая категория курсора
    @tparam T тип
    */
    template <class Traversal, class T>
    struct wrap_with_old_value_if_bidirectional
     : std::conditional<std::is_convertible<Traversal, bidirectional_cursor_tag>::value,
                        with_old_value<T>, T>
    {};

    /** @brief Синоним для <tt> wrap_with_old_value_if_bidirectional<Traversal, T>::type </tt>
    @tparam Traversal желаемая категория курсора
    @tparam T тип
    */
    template <class Traversal, class T>
    using wrap_with_old_value_if_bidirectional_t
        = typename wrap_with_old_value_if_bidirectional<Traversal, T>::type;

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
        constexpr sequence_base(Args && ... args)
         : Payload(std::forward<Args>(args)...)
        {}

        /// @brief Конструктор без аргументов
        constexpr sequence_base() = default;

        //@{
        /// @brief Конструкторы копирования и перемещения
        constexpr sequence_base(sequence_base &) = default;
        constexpr sequence_base(sequence_base const &) = default;
        constexpr sequence_base(sequence_base &&) = default;
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
        constexpr Payload & payload()
        {
            return *this;
        }

        constexpr Payload const & payload() const
        {
            return *this;
        }
        //@}
    };

    //@{
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

    template <class Seq, class Base>
    sequence_iterator<Seq &> begin(sequence_base<Seq, Base> & s)
    {
        return sequence_iterator<Seq &>(static_cast<Seq&>(s));
    }
    //@}

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

    /** @brief Ссылка на текущий элемент последовательности
    @param s последовательность
    @pre <tt> !s == false </tt>
    @return <tt> static_cast<Seq const&>(s).front() </tt>
    @todo Реализовать через SFINAE и is_sequence, использовать decltype?
    */
    template <class Seq, class Base>
    constexpr typename Seq::reference
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

    /** @brief Класс функционального объекта, вычисляющий размер
    массивов/контейнеров и последовательностей
    */
    class size_fn
    {
    private:
        template <class Sequence>
        static DifferenceType<Sequence>
        sequence_size(Sequence const & s, finite_single_pass_cursor_tag)
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
        sequence_size(Sequence const & s, finite_random_access_cursor_tag)
        {
            return s.size();
        }

        template <class Container>
        typename Container::difference_type
        container_size(Container const & c, void_t<decltype(c.size())> *) const
        {
            return c.size();
        }

        template <class Container>
        typename Container::difference_type
        container_size(Container const & c, ...) const
        {
            return std::distance(c.begin(), c.end());
        }

        template <class Sequenced>
        DifferenceType<Sequenced>
        dispatch(Sequenced const & s,
                 void_t<decltype(ural::make_cursor_tag(s))> *) const
        {
            return this->sequence_size(s, ural::make_cursor_tag(s));
        }

        template <class Sequenced>
        DifferenceType<Sequenced>
        dispatch(Sequenced const & s,
                 void_t<typename Sequenced::allocator_type> *) const
        {
            return this->container_size(s, nullptr);
        }

    public:
        /** @brief Размер последовательности или контейнера
        @param s последовательность
        @return Количество непройденных элементов последовательности
        */
        template <class Sequenced>
        DifferenceType<Sequenced>
        operator()(Sequenced const & s) const
        {
            return this->dispatch(s, nullptr);
        }

        /** @brief Размер массива
        @tparam N количество элементов
        @tparam T тип элементов
        @return @c N
        */
        template <class T, size_t N>
        constexpr size_t operator()(T(&)[N]) const
        {
            return N;
        }

        /** @brief Размер <tt> std::array </tt>
        @tparam N количество элементов
        @tparam T тип элементов
        @return @c N
        */
        template <class T, size_t N>
        constexpr size_t operator()(std::array<T, N> const & a) const
        {
            return a.size();
        }
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
            return this->impl(s, std::move(n), ural::make_cursor_tag(s));
        }

    private:
        template <class Sequence>
        static void impl(Sequence & s, DifferenceType<Sequence> n,
                         single_pass_cursor_tag)
        {
            for(; n > 0; -- n)
            {
                ++ s;
            }
        }

        template <class Sequence>
        static void impl(Sequence & s, DifferenceType<Sequence> n,
                         random_access_cursor_tag)
        {
            s += n;
        }
    };

    /** @brief Пропуск заданного числа элементов в задней части
    последовательности
    */
    class pop_back_n_fn
    {
    public:
        /** @brief Отбрасывает последние @c n элементов
        @pre <tt> n <= this->size() </tt>
        @param s последовательность
        @param n количество элементов
        */
        template <class BidirectionalSequence>
        void operator()(BidirectionalSequence & x,
                        DifferenceType<BidirectionalSequence> n) const
        {
            this->impl(x, n, make_cursor_tag(x));
        }

    private:
        template <class T>
        void impl(T & x, DifferenceType<T> n, bidirectional_cursor_tag) const
        {
            for(; n > 0; -- n)
            {
                x.pop_back();
            }
        }

        // @todo Покрыть тестом
        template <class T>
        void impl(T & x, DifferenceType<T> n, finite_random_access_cursor_tag) const
        {
            x.pop_back(n);
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

    /// @brief Тип Функционального объекта для функции-члена @c original
    class original_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param x аргумент
        @return <tt> x.original() </tt>
        */
        template <class T>
        auto operator()(T const & x) const
        {
            return x.original();
        }
    };

    /// @brief Тип функционального объекта для функции-члена @c traversed_front
    class traversed_front_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param x аргумент
        @return <tt> x.traversed_front() </tt>
        */
        template <class T>
        auto operator()(T const & x) const
        {
            return x.traversed_front();
        }
    };

    /// @brief Тип Функционального объекта для функции-члена @c traversed_back
    class traversed_back_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param x аргумент
        @return <tt> x.traversed_back() </tt>
        */
        template <class T>
        auto operator()(T const & x) const
        {
            return x.traversed_back();
        }
    };

    /** @brief Тип функционального объекта для исчерпания последовательности
    в прямом направлении.
    */
    class exhaust_front_fn
    {
    public:
        /** @brief Вызывает <tt> s.exhaust_front() </tt>
        @param s последовательность
        */
        template <class Sequence>
        void operator()(Sequence & s) const
        {
            return s.exhaust_front();
        }
    };

    /** @brief Тип функционального объекта для исчерпания последовательности
    в обратном направлении.
    */
    class exhaust_back_fn
    {
    public:
        /** @brief Вызывает <tt> s.exhaust_back() </tt>
        @param s последовательность
        */
        template <class Sequence>
        void operator()(Sequence & s) const
        {
            s.exhaust_back();
        }
    };

    /** @brief Тип функционального объекта для отбрасывания передней пройденной
    части последовательности.
    */
    class shrink_front_fn
    {
    public:
        /** @brief Вызывает <tt> s.shrink_front() </tt>
        @param s последовательность
        @return @c s
        */
        template <class Sequence>
        Sequence &
        operator()(Sequence & s) const
        {
            s.shrink_front();
            return s;
        }
    };

    /** @brief Тип функционального объекта для отбрасывания задней пройденной
    части последовательности.
    */
    class shrink_back_fn
    {
    public:
        /** @brief Вызывает <tt> s.shrink_back() </tt>
        @param s последовательность
        */
        template <class Sequence>
        void operator()(Sequence & s)
        {
            s.shrink_back();
        }
    };

    /** @brief Тип функционального объекта для создания последовательности,
    отличающейся от исходной только тем, что передняя пройденная часть
    отсутствует.
    */
    struct shrink_front_copy_fn
    {
        /** @brief Создании последовательности, отличающейся от исходной только
        тем, что передняя пройденная часть отсутствует.
        @param s последовательность
        @return Копию @c s, к которой применена операция @c shrink_front
        */
        template <class Sequence>
        Sequence operator()(Sequence s) const
        {
            s.shrink_front();
            return s;
        }
    };

    /// @cond false
    namespace details
    {
        struct at_fn
        {
        public:
            template <class Sequence, class F>
            auto operator()(Sequence const & s, F f) const
            -> decltype(f(s))
            {
                return f(s);
            }

            template <class Sequence>
            ReferenceType<Sequence>
            operator()(Sequence const & s, DifferenceType<Sequence> i) const
            {
                return s[i];
            }
        };

        template <class Sequence, class Index1, class Index2>
        void indirect_swap_adl_hook(Sequence const & x, Index1 ix,
                                    Sequence const & y, Index2 iy)
        {
            using std::swap;
            swap(at_fn{}(x, ix), at_fn{}(y, iy));
        }

        struct indirect_swap_fn
        {
        public:
            template <class Sequence>
            void operator()(Sequence const & x, Sequence const & y) const
            {
                return (*this)(x, front, y, front);
            }

            template <class Sequence, class Index1, class Index2>
            void operator()(Sequence const & x, Index1 ix,
                            Sequence const & y, Index2 iy) const
            {
                using ::ural::details::indirect_swap_adl_hook;
                return indirect_swap_adl_hook(x, ix, y, iy);
            }
        };
    }
    /// @endcond

    namespace
    {
        /** @brief Функциональный объект для обмена элементов
        последовательностей
        */
        constexpr auto const & indirect_swap
            = odr_const<ural::details::indirect_swap_fn>;

        /** @brief Функциональный объект для пропуска заданного числа элементов
        в передней части последовательности
        */
        constexpr auto const & advance = odr_const<advance_fn>;

        /** @brief Функциональный объект для пропуска заданного числа элементов
        в задней части последовательности
        */
        constexpr auto const & pop_back_n = odr_const<pop_back_n_fn>;

        /** @brief Функциональный объект для операции продвижения копии
        последовательности на заданное число шагов.
        */
        constexpr auto const & next = odr_const<next_fn>;

        /** @brief Функциональный объект, вычисляющий размер массивов,
        контейнеров и последовательностей.
        */
        constexpr auto const & size = odr_const<size_fn>;

        /// @brief Функциональный объект для функции-члена @c original
        constexpr auto const & original = odr_const<original_fn>;

        /// @brief Функциональный объект для функции-члена @c traversed_front
        constexpr auto const & traversed_front = odr_const<traversed_front_fn>;

        /// @brief Функциональный объект для функции-члена @c traversed_back
        constexpr auto const & traversed_back = odr_const<traversed_back_fn>;

        /// @brief Функциональный объект для функции-члена @c shrink_front
        constexpr auto const & shrink_front = odr_const<shrink_front_fn>;

        /// @brief Функциональный объект для функции-члена @c shrink_back
        constexpr auto const & shrink_back = odr_const<shrink_back_fn>;

        /** @brief Функциональный объект для создания последовательности,
        отличающейся от исходной только тем, что передняя пройденная часть
        отсутствует.
        */
        constexpr auto const & shrink_front_copy = odr_const<shrink_front_copy_fn>;

        /** @brief  Функциональный объект для исчерпания последовательности
        в прямом порядке
        */
        constexpr auto const & exhaust_front = odr_const<exhaust_front_fn>;

        /** @brief  Функциональный объект для исчерпания последовательности
        в обратном порядке
        */
        constexpr auto const & exhaust_back = odr_const<exhaust_back_fn>;

        // @todo сгруппировать с front...
    }
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
