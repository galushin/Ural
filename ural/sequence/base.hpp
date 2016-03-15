#ifndef Z_URAL_cursor_base_HPP_INCLUDED
#define Z_URAL_cursor_base_HPP_INCLUDED

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
#include <ural/sequence/cursor_iterator.hpp>

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
                        experimental::with_old_value<T>, T>
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
                        experimental::with_old_value<T>, T>
    {};

    /** @brief Синоним для <tt> wrap_with_old_value_if_bidirectional<Traversal, T>::type </tt>
    @tparam Traversal желаемая категория курсора
    @tparam T тип
    */
    template <class Traversal, class T>
    using wrap_with_old_value_if_bidirectional_t
        = typename wrap_with_old_value_if_bidirectional<Traversal, T>::type;

    /** @brief Базовый класс для курсоров (CRTP)
    @tparam Cur тип курсора-наследника
    @tparam Payload класс, от которого будет закрыто наследовать данный класс.
    Может использоваться для оптимизации пустых базовых классов.
    */
    template <class Cursor, class Payload = ural::empty_type>
    class cursor_base
     : private Payload
    {
        /** @brief Преобразование в курсор
        @param seq исходная последовательность
        @return seq
        */
        friend Cursor cursor(Cursor cur)
        {
            return cur;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !x == false </tt>
        @return @c x
        */
        friend Cursor & operator++(Cursor & cur)
        {
            cur.pop_front();
            return cur;
        }

        /** @brief Оператор вывода курсора в поток
        @param os поток вывода
        @param cur курсор
        @return @c os
        @todo устранить дублирование с write_separated
        */
        template <class OStream>
        friend OStream & operator<<(OStream & os, Cursor & cur)
        {
            os << "{";
            if(!!cur)
            {
                os << *cur;
                ++ cur;

                for(; !!cur; ++ cur)
                {
                    os << ", " << *cur;
                }
            }
            os << "}";

            return os;
        }

    public:
        /** @brief Проверка исчерпания курсора
        @return @b true, если курсора исчерпана, иначе --- @b false
        */
        bool empty() const
        {
            return !static_cast<Cursor const &>(*this);
        }

    protected:
        /** @brief Конструктор
        @param args список аргументов для конструктора базового класса
        */
        template <class... Args>
        constexpr cursor_base(Args && ... args)
         : Payload(std::forward<Args>(args)...)
        {}

        /// @brief Конструктор без аргументов
        constexpr cursor_base() = default;

        //@{
        /// @brief Конструкторы копирования и перемещения
        constexpr cursor_base(cursor_base &) = default;
        constexpr cursor_base(cursor_base const &) = default;
        constexpr cursor_base(cursor_base &&) = default;
        //@}

        //@{
        /** @brief Операторы присваивания
        @return <tt> *this </tt>
        */
        cursor_base & operator=(cursor_base &) = default;
        cursor_base & operator=(cursor_base const &) = default;
        cursor_base & operator=(cursor_base &&) = default;
        //@}

        /// @brief Деструктор
        ~ cursor_base() = default;

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
    /** @brief Создание начального итератора для курсора
    @param cur курсор
    @return <tt> cursor_iterator<Cursor>{static_cast<Cursor const&>(cur)} </tt>
    */
    template <class Cursor, class Base>
    cursor_iterator<Cursor> begin(cursor_base<Cursor, Base> const & cur)
    {
        return cursor_iterator<Cursor>{static_cast<Cursor const&>(cur)};
    }

    template <class Cursor, class Base>
    cursor_iterator<Cursor> begin(cursor_base<Cursor, Base> && cur)
    {
        return cursor_iterator<Cursor>{static_cast<Cursor &&>(cur)};
    }

    template <class Cursor, class Base>
    cursor_iterator<Cursor &> begin(cursor_base<Cursor, Base> & cur)
    {
        return cursor_iterator<Cursor &>(static_cast<Cursor &>(cur));
    }
    //@}

    //@{
    /** @brief Создание конечного итератора для курсора
    @return <tt> cursor_iterator<Cursor>{} </tt>
    */
    template <class Cursor, class Base>
    cursor_iterator<Cursor> end(cursor_base<Cursor, Base> const &)
    {
        return cursor_iterator<Cursor>{};
    }

    template <class Cursor, class Base>
    cursor_iterator<Cursor &> end(cursor_base<Cursor, Base> &)
    {
        return cursor_iterator<Cursor &>{};
    }
    //@}

    /** @brief Ссылка на текущий элемент курсора
    @param s последовательность
    @pre <tt> !s == false </tt>
    @return <tt> static_cast<Cursor const&>(s).front() </tt>
    @todo Реализовать через SFINAE и is_cursor, использовать decltype?
    */
    template <class Cursor, class Base>
    constexpr typename Cursor::reference
    operator*(cursor_base<Cursor, Base> const & s)
    {
        return static_cast<Cursor const&>(s).front();
    }

    //@{
    /** @brief Бинарный оператор "плюс" для курсора и расстояния
    @param cur курсор
    @param n свдиг
    @todo Передавать последовательность по значению
    @return <tt> Cursor{cur} + n </tt>
    */
    template <class Cursor, class Base>
    Cursor
    operator+(cursor_base<Cursor, Base> const & s, DifferenceType<Cursor> n)
    {
        auto result = static_cast<Cursor const &>(s);
        result += n;
        return result;
    }

    template <class Cursor, class Base>
    Cursor
    operator+(DifferenceType<Cursor> n, cursor_base<Cursor, Base> const & cur)
    {
        return std::move(cur + n);
    }
    //@}

    /** @brief Класс функционального объекта, вычисляющий размер
    массивов/контейнеров и последовательностей
    */
    class size_fn
    {
    private:
        template <class Cursor>
        static DifferenceType<Cursor>
        cursor_size(Cursor const & cur, finite_single_pass_cursor_tag)
        {
            DifferenceType<Cursor> n{0};

            for(auto in = cur; !!in; ++ in)
            {
                ++ n;
            }

            return n;
        }

        template <class Cursor>
        static DifferenceType<Cursor>
        cursor_size(Cursor const & cur, finite_random_access_cursor_tag)
        {
            return cur.size();
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

        template <class Cursor>
        DifferenceType<Cursor>
        dispatch(Cursor const & cur,
                 void_t<decltype(ural::make_cursor_tag(cur))> *) const
        {
            return this->cursor_size(cur, ural::make_cursor_tag(cur));
        }

        template <class Sequence>
        DifferenceType<Sequence>
        dispatch(Sequence const & s,
                 void_t<typename Sequence::allocator_type> *) const
        {
            return this->container_size(s, nullptr);
        }

    public:
        /** @brief Размер курсора или контейнера
        @param s последовательность
        @return Количество непройденных элементов курсора или размер контейнера
        */
        template <class Sequence>
        DifferenceType<Sequence>
        operator()(Sequence const & s) const
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

    /** @brief Тип функционального объекта для продвижения курсора на заданное
    число шагов
    */
    class advance_fn
    {
    public:
        /** @brief Продвижение курсора на заданное число шагов
        @param cur курсор
        @param n число шагов
        */
        template <class Cursor>
        void operator()(Cursor & cur, DifferenceType<Cursor> n) const
        {
            return this->impl(cur, std::move(n), ural::make_cursor_tag(cur));
        }

    private:
        template <class Cursor>
        static void impl(Cursor & cur, DifferenceType<Cursor> n,
                         single_pass_cursor_tag)
        {
            for(; n > 0; -- n)
            {
                ++ cur;
            }
        }

        template <class Cursor>
        static void impl(Cursor & cur, DifferenceType<Cursor> n,
                         random_access_cursor_tag)
        {
            cur += n;
        }
    };

    /// @brief Пропуск заданного числа элементов в задней части курсора
    class pop_back_n_fn
    {
    public:
        /** @brief Отбрасывает последние @c n элементов
        @pre <tt> n <= this->size() </tt>
        @param cur курсор
        @param n количество элементов
        */
        template <class BidirectionalCursor>
        void operator()(BidirectionalCursor & cur,
                        DifferenceType<BidirectionalCursor> n) const
        {
            this->impl(cur, n, make_cursor_tag(cur));
        }

    private:
        template <class Cursor>
        void impl(Cursor & cur, DifferenceType<Cursor> n, bidirectional_cursor_tag) const
        {
            for(; n > 0; -- n)
            {
                cur.pop_back();
            }
        }

        // @todo Покрыть тестом
        template <class Cursor>
        void impl(Cursor & cur, DifferenceType<Cursor> n, finite_random_access_cursor_tag) const
        {
            cur.pop_back(n);
        }
    };


    /** @brief Тип функционального объекта для операции продвижения копии
    курсора на заданное число шагов.
    */
    class next_fn
    {
    public:
        /** @brief Продвижение копии курсора на заданное количество шагов
        @param cur курсор
        @param n количество шагов
        @return Копия курсора @c s продвинутая на @c n шагов c помощью
        <tt> advance </tt>.
        */
        template <class Cursor>
        Cursor
        operator()(Cursor cur, DifferenceType<Cursor> n = 1) const
        {
            ::ural::advance_fn{}(cur, n);
            return cur;
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

    /** @brief Тип функционального объекта для исчерпания курсора в прямом
    направлении.
    */
    class exhaust_front_fn
    {
    public:
        /** @brief Вызывает <tt> cur.exhaust_front() </tt>
        @param cur курсор
        */
        template <class Cursor>
        void operator()(Cursor & cur) const
        {
            return cur.exhaust_front();
        }
    };

    /** @brief Тип функционального объекта для исчерпания курсора в обратном
    направлении.
    */
    class exhaust_back_fn
    {
    public:
        /** @brief Вызывает <tt> cur.exhaust_back() </tt>
        @param cur курсор
        */
        template <class Cursor>
        void operator()(Cursor & cur) const
        {
            cur.exhaust_back();
        }
    };

    /** @brief Тип функционального объекта для отбрасывания передней пройденной
    части курсора.
    */
    class shrink_front_fn
    {
    public:
        /** @brief Вызывает <tt> cur.shrink_front() </tt>
        @param cur курсор
        @return @c cur
        */
        template <class Cursor>
        Cursor &
        operator()(Cursor & cur) const
        {
            cur.shrink_front();
            return cur;
        }
    };

    /** @brief Тип функционального объекта для отбрасывания задней пройденной
    части курсора.
    */
    class shrink_back_fn
    {
    public:
        /** @brief Вызывает <tt> cur.shrink_back() </tt>
        @param cur курсор
        */
        template <class Cursor>
        void operator()(Cursor & cur)
        {
            cur.shrink_back();
        }
    };

    /** @brief Тип функционального объекта для создания курсора, отличающегося
    от исходного только тем, что передняя пройденная часть отсутствует.
    */
    struct shrink_front_copy_fn
    {
        /** @brief Создании курсора, отличающегося от исходного только тем, что
        передняя пройденная часть отсутствует.
        @param cur курсор
        @return Копию @c cur, к которой применена операция @c shrink_front
        */
        template <class Cursor>
        Cursor operator()(Cursor cur) const
        {
            cur.shrink_front();
            return cur;
        }
    };

    /// @cond false
    namespace details
    {
        struct at_fn
        {
        public:
            template <class Cursor, class F>
            auto operator()(Cursor const & cur, F f) const
            -> decltype(f(cur))
            {
                return f(cur);
            }

            template <class Cursor>
            ReferenceType<Cursor>
            operator()(Cursor const & cur, DifferenceType<Cursor> i) const
            {
                return cur[i];
            }
        };

        template <class Cursor, class Index1, class Index2>
        void indirect_swap_adl_hook(Cursor const & x, Index1 ix,
                                    Cursor const & y, Index2 iy)
        {
            using std::swap;
            swap(at_fn{}(x, ix), at_fn{}(y, iy));
        }

        struct indirect_swap_fn
        {
        public:
            template <class Cursor>
            void operator()(Cursor const & x, Cursor const & y) const
            {
                return (*this)(x, front, y, front);
            }

            template <class Cursor, class Index1, class Index2>
            void operator()(Cursor const & x, Index1 ix,
                            Cursor const & y, Index2 iy) const
            {
                using ::ural::details::indirect_swap_adl_hook;
                return indirect_swap_adl_hook(x, ix, y, iy);
            }
        };
    }
    /// @endcond

    namespace
    {
        /** @brief Функциональный объект для обмена элементов, на которые
        ссылаются курсоры
        */
        constexpr auto const & indirect_swap
            = odr_const<ural::details::indirect_swap_fn>;

        /** @brief Функциональный объект для пропуска заданного числа элементов
        в передней части курсора
        */
        constexpr auto const & advance = odr_const<advance_fn>;

        /** @brief Функциональный объект для пропуска заданного числа элементов
        в задней части курсора
        */
        constexpr auto const & pop_back_n = odr_const<pop_back_n_fn>;

        /** @brief Функциональный объект для операции продвижения копии курсора
        на заданное число шагов.
        */
        constexpr auto const & next = odr_const<next_fn>;

        /** @brief Функциональный объект, вычисляющий размер массивов, курсоров
        и контейнеров.
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

        /** @brief Функциональный объект для создания курсора, отличающегося от
        исходного только тем, что передняя пройденная часть отсутствует.
        */
        constexpr auto const & shrink_front_copy = odr_const<shrink_front_copy_fn>;

        /** @brief  Функциональный объект для исчерпания последовательности
        в прямом порядке
        */
        constexpr auto const & exhaust_front = odr_const<exhaust_front_fn>;

        /** @brief  Функциональный объект для исчерпания курсора в обратном
        порядке
        */
        constexpr auto const & exhaust_back = odr_const<exhaust_back_fn>;

        // @todo сгруппировать с front...
    }
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_cursor_base_HPP_INCLUDED
