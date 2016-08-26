#ifndef Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED

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

/** @file ural/sequence/uniqued.hpp
 @brief Последовательность неповторяющихся соседних элементов
*/

#include <ural/utility/pipeable.hpp>
#include <ural/algorithm/core.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Курсор последовательности неповторяющихся соседних элементов
    базовой последовательности
    @tparam Input Тип базового курсора
    @tparam BinaryPredicat бинарный предикат, определяющий, совпадают ли два
    соседних элемента.

    @note Было решено увеличить размер курсора за возможность менять текущий
    элемент последовательности. Другим возможным вариантом было сделать ссылку
    константной.
    @todo Уменьшить дублирование, где это возможно
    */
    template <class Input, class BinaryPredicate = ural::equal_to<> >
    class unique_cursor
     : public cursor_base<unique_cursor<Input, BinaryPredicate>,
                            BinaryPredicate>
    {
        using Base = cursor_base<unique_cursor<Input, BinaryPredicate>,
                                   BinaryPredicate>;
    public:
        // Оператор "равно"
        friend bool operator==(unique_cursor const & x,
                               unique_cursor const & y)
        {
            return x.current_ == y.current_
                    && x.next_ == y.next_ && x.predicate() == y.predicate();
        }

        // Типы
        /// @brief Тип ссылки
        using reference = typename Input::reference;

        /// @brief Тип значения
        using value_type = value_type_t<Input>;

        /// @brief Категория курсора
        using cursor_tag = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = typename Input::pointer;

        /// @brief Тип расстояния
        using distance_type = difference_type_t<Input>;

        // Конструкторы
        /** @brief Конструктор
        @param in базовый курсор
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == BinaryPredicate{} </tt>
        */
        explicit unique_cursor(Input in)
         : unique_cursor(std::move(in), cursor_tag{})
        {}

        /** @brief Конструктор
        @param in базовый курсор
        @param pred используемый предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit unique_cursor(Input in, BinaryPredicate pred)
         : unique_cursor(std::move(in), std::move(pred), cursor_tag{})
        {}

        // Адаптор курсора
        //@{
        /** @brief Базовый курсор
        @return Константная ссылка на базовый курсор
        @note <tt> this->base().front() </tt> и <tt> this->front() </tt>
        ссылаются на разные элементы в случае однопроходного курсора.
        */
        Input const & base() const &
        {
            return this->base_impl(cursor_tag{});
        }

        Input && base() &&
        {
            return std::move(this->base_impl(cursor_tag()));
        }
        //@}

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return this->payload();
        }

        // Однопроходный курсор
        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !current_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            assert(!!*this);
            return *current_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            assert(!!*this);
            return this->pop_front_impl(cursor_tag{});
        }

        // Прямой курсор
        /** @brief Исходный курсор (вместе с пройденными частями)
        @return Исходный курсор
        */
        unique_cursor original() const
        {
            return unique_cursor(current_.original(), this->predicate());
        }

        /** @brief Пройденная передняя часть последовательности
        @return Пройденная передняя часть последовательности
        */
        unique_cursor<TraversedFrontType<Input>, BinaryPredicate>
        traversed_front() const
        {
            using Seq = unique_cursor<TraversedFrontType<Input>, BinaryPredicate>;
            return Seq(current_.traversed_front(), this->predicate());
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            current_.shrink_front();
        }

        /** @brief Исчерпание последовательности в прямом порядке за константное
        время.
        @post <tt> !*this == true </tt>
        */
        void exhaust_front()
        {
            ural::exhaust_front(current_);
            next_ = current_;
        }

    private:
        // Конструкторы
        unique_cursor(Input in, single_pass_cursor_tag)
         : Base()
         , current_()
         , next_(std::move(in))
        {
            if(!!next_)
            {
                current_ = *next_;
                ++ next_;
                this->seek();
            }
        }

        unique_cursor(Input in, forward_cursor_tag)
         : Base()
         , current_(std::move(in))
         , next_(current_)
        {
            if(!!next_)
            {
                ++ next_;
                this->seek();
            }
        }

        unique_cursor(Input in, BinaryPredicate pred, single_pass_cursor_tag)
         : Base(std::move(pred))
         , current_()
         , next_(std::move(in))
        {
            if(!!next_)
            {
                current_ = *next_;
                ++ next_;
                this->seek();
            }
        }

        unique_cursor(Input in, BinaryPredicate pred, forward_cursor_tag)
         : Base(std::move(pred))
         , current_(std::move(in))
         , next_(current_)
        {
            if(!!next_)
            {
                ++ next_;
                this->seek();
            }
        }

        // Базовая последовательность
        Input & base_impl(single_pass_cursor_tag)
        {
            return this->next_;
        }

        Input const & base_impl(single_pass_cursor_tag) const
        {
            return this->next_;
        }

        Input & base_impl(forward_cursor_tag)
        {
            return this->current_;
        }

        Input const & base_impl(forward_cursor_tag) const
        {
            return this->current_;
        }

        // Поиск следующего
        void seek()
        {
            assert(!!current_);

            next_ = find_fn{}(std::move(next_), *current_, not_fn(this->predicate()));
        }

        // Переход к следующему элементу
        void pop_front_impl(single_pass_cursor_tag)
        {
            if(!!next_)
            {
                current_ = *next_;
                ++ next_;
                this->seek();
            }
            else
            {
                current_ = Holder{};
            }
        }

        void pop_front_impl(forward_cursor_tag)
        {
            current_ = next_;

            if(!!next_)
            {
                ++ next_;
                this->seek();
            }
        }

    private:
        using optional_value = experimental::optional<value_type>;

        typedef std::is_convertible<cursor_tag, forward_cursor_tag> Is_forward;
        typedef typename std::conditional<!Is_forward::value, optional_value, Input>::type
            Holder;

        Holder current_;
        Input next_;
    };

    /** @brief Тип функционального объекта для создания @c unique_cursor
    с заданным условием эквивалентности элементов.
    */
    struct make_adjacent_filtered_cursor_fn
    {
    public:
        /** @brief Функция создания @c unique_cursor с заданным условием
        эквивалентности элементов
        @param in входная последовательность
        @param pred бинарный предикат
        */
        template <class Forward, class BinaryPredicate>
        auto operator()(Forward && in, BinaryPredicate pred) const
        {
            typedef unique_cursor<decltype(::ural::cursor_fwd<Forward>(in)),
                                    decltype(make_callable(std::move(pred)))> Seq;
            return Seq(::ural::cursor_fwd<Forward>(in),
                       make_callable(std::move(pred)));
        }
    };

    /// @brief Тип Функционального объекта для создания @c unique_cursor.
    class make_unique_cursor_fn
    {
    public:
        /** @brief Функция создания @c unique_cursor
        @param in входная последовательность
        @return <tt> unique_cursor<Seq>(cursor_fwd<Forward>(in)) </tt>, где
        @c Seq -- <tt> unique_cursor<decltype(cursor_fwd<Forward>(in))> </tt>
        */
        template <class Forward>
        auto operator()(Forward && in) const
        {
            auto f = make_adjacent_filtered_cursor_fn{};

            return f(::ural::cursor_fwd<Forward>(in), ural::equal_to<>{});
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c unique_cursor.
        constexpr auto & make_unique_cursor
            = odr_const<make_unique_cursor_fn>;

        /// @brief Объект для создающия @c unique_cursor в конвейрном стиле.
        constexpr auto & uniqued
            = odr_const<pipeable<make_unique_cursor_fn>>;

        /** @brief Объект для создающия @c unique_cursor с заданным условием
        эквивалентности элементов в конвейрном стиле.
        */
        constexpr auto & adjacent_filtered
            = odr_const<pipeable_maker<make_adjacent_filtered_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
