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
    /** @brief Последовательность неповторяющихся соседних элементов базовой
    последовательности
    @tparam Input Тип базовой последовательности
    @tparam BinaryPredicat бинарный предикат, определяющий, совпадают ли два
    последовательных элемента.

    @note Было решено увеличить размер объекта за возможность менять текущий
    элемент последовательности. Другим возможным вариантом было сделать ссылку
    константной.
    @todo Уменьшить дублирование, где это возможно
    */
    template <class Input, class BinaryPredicate = ural::equal_to<> >
    class unique_sequence
     : public sequence_base<unique_sequence<Input, BinaryPredicate>,
                            BinaryPredicate>
    {
        using Base = sequence_base<unique_sequence<Input, BinaryPredicate>,
                                   BinaryPredicate>;
    public:
        // Оператор "равно"
        friend bool operator==(unique_sequence const & x,
                               unique_sequence const & y)
        {
            return x.current_ == y.current_
                    && x.next_ == y.next_ && x.predicate() == y.predicate();
        }

        // Типы
        /// @brief Тип ссылки
        typedef typename Input::reference reference;

        /// @brief Тип значения
        typedef ValueType<Input> value_type;

        /// @brief Категория курсора
        using cursor_tag = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Input> distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == BinaryPredicate{} </tt>
        */
        explicit unique_sequence(Input in)
         : unique_sequence(std::move(in), cursor_tag{})
        {}

        /** @brief Конструктор
        @param in входная последовательность
        @param pred используемый предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit unique_sequence(Input in, BinaryPredicate pred)
         : unique_sequence(std::move(in), std::move(pred), cursor_tag{})
        {}

        // Адаптор последовательности
        //@{
        /** @brief Базовая последовательность
        @return Ссылка на базовую последовательность
        @note <tt> this->base().front() </tt> и <tt> this->front() </tt>
        ссылаются на разные элементы в случае однопроходной последовательности.
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

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !current_;
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
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

        // Прямая последовательность
        /** @brief Полная последовательность (вместе с пройденными частями)
        @return Исходная последовательность
        */
        unique_sequence original() const
        {
            return unique_sequence(current_.original(), this->predicate());
        }

        /** @brief Пройденная передняя часть последовательность
        @return Пройденная передняя часть последовательность
        */
        unique_sequence<TraversedFrontType<Input>, BinaryPredicate>
        traversed_front() const
        {
            using Seq = unique_sequence<TraversedFrontType<Input>, BinaryPredicate>;
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
        unique_sequence(Input in, single_pass_cursor_tag)
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

        unique_sequence(Input in, forward_cursor_tag)
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

        unique_sequence(Input in, BinaryPredicate pred, single_pass_cursor_tag)
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

        unique_sequence(Input in, BinaryPredicate pred, forward_cursor_tag)
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
        typedef optional<value_type> optional_value;
        typedef std::is_convertible<cursor_tag, forward_cursor_tag> Is_forward;
        typedef typename std::conditional<!Is_forward::value, optional_value, Input>::type
            Holder;

        Holder current_;
        Input next_;
    };

    /** @brief Тип функционального объекта для создания @c unique_sequence
    с заданным условием эквивалентности элементов.
    */
    struct make_adjacent_filtered_sequence_fn
    {
    public:
        /** @brief Функция создания @c unique_sequence с заданным условием
        эквивалентности элементов
        @param in входная последовательность
        @param pred бинарный предикат
        */
        template <class Forward, class BinaryPredicate>
        auto operator()(Forward && in, BinaryPredicate pred) const
        {
            typedef unique_sequence<decltype(::ural::sequence_fwd<Forward>(in)),
                                    decltype(make_callable(std::move(pred)))> Seq;
            return Seq(::ural::sequence_fwd<Forward>(in),
                       make_callable(std::move(pred)));
        }
    };

    /// @brief Тип Функционального объекта для создания @c unique_sequence.
    class make_unique_sequence_fn
    {
    public:
        /** @brief Функция создания @c unique_sequence
        @param in входная последовательность
        @return <tt> unique_sequence<Seq>(sequence_fwd<Forward>(in)) </tt>, где
        @c Seq -- <tt> unique_sequence<decltype(sequence_fwd<Forward>(in))> </tt>
        */
        template <class Forward>
        auto operator()(Forward && in) const
        {
            auto f = make_adjacent_filtered_sequence_fn{};

            return f(::ural::sequence_fwd<Forward>(in), ural::equal_to<>{});
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c unique_sequence в
        constexpr auto & make_unique_sequence
            = odr_const<make_unique_sequence_fn>;

        /// @brief Объект для создающия @c unique_sequence в конвейрном силе.
        constexpr auto & uniqued
            = odr_const<pipeable<make_unique_sequence_fn>>;

        /** @brief Объект для создающия @c unique_sequence с заданным условием
        эквивалентности элементов в конвейрном стиле.
        */
        constexpr auto & adjacent_filtered
            = odr_const<pipeable_maker<make_adjacent_filtered_sequence_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
