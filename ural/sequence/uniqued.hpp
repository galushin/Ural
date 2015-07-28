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
     : public sequence_base<unique_sequence<Input, BinaryPredicate>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Input::reference reference;

        /// @brief Тип значения
        typedef ValueType<Input> value_type;

        /// @brief Категория обхода
        typedef typename ural::common_tag<typename Input::traversal_tag,
                                          forward_traversal_tag>::type traversal_tag;

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
         : unique_sequence(std::move(in), traversal_tag{})
        {
            this->seek();
        }

        /** @brief Конструктор
        @param in входная последовательность
        @param pred используемый предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit unique_sequence(Input in, BinaryPredicate pred)
         : unique_sequence(std::move(in), std::move(pred), traversal_tag{})
        {
            this->seek();
        }

        // Адаптор последовательности
        //@{
        /** @brief Базовая последовательность
        @return Ссылка на базовую последовательность
        @note <tt> this->base().front() </tt> и <tt> this->front() </tt>
        ссылаются на разные элементы в случае однопроходной последовательности
        */
        Input const & base() const &
        {
            return this->base_impl(traversal_tag{});
        }

        Input && base() &&
        {
            return std::move(this->base_impl(traversal_tag()));
        }
        //@}

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return this->eq_;
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
            return this->pop_front_impl(traversal_tag{});
        }

    private:
        // Конструкторы
        unique_sequence(Input in, single_pass_traversal_tag)
         : current_()
         , next_(std::move(in))
         , eq_()
        {
            if(!!next_)
            {
                current_ = *next_;
                ++ next_;
            }
        }

        unique_sequence(Input in, forward_traversal_tag)
         : current_(std::move(in))
         , next_(current_)
         , eq_()
        {
            if(!!next_)
            {
                ++ next_;
            }
        }

        unique_sequence(Input in, BinaryPredicate pred, single_pass_traversal_tag)
         : current_()
         , next_(std::move(in))
         , eq_(std::move(pred))
        {
            if(!!next_)
            {
                current_ = *next_;
                ++ next_;
            }
        }

        unique_sequence(Input in, BinaryPredicate pred, forward_traversal_tag)
         : current_(std::move(in))
         , next_(current_)
         , eq_(std::move(pred))
        {
            if(!!next_)
            {
                ++ next_;
            }
        }

        // Базовая последовательность
        Input & base_impl(single_pass_traversal_tag)
        {
            return this->next_;
        }

        Input const & base_impl(single_pass_traversal_tag) const
        {
            return this->next_;
        }

        Input & base_impl(forward_traversal_tag)
        {
            return this->current_;
        }

        Input const & base_impl(forward_traversal_tag) const
        {
            return this->current_;
        }

        // Поиск следующего
        void seek()
        {
            next_ = find_fn{}(std::move(next_), *current_, not_fn(this->predicate()));
        }

        // Переход к следующему элементу
        void pop_front_impl(single_pass_traversal_tag)
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

        void pop_front_impl(forward_traversal_tag)
        {
            current_ = next_;

            if(!!next_)
            {
                ++ next_;
                this->seek();
            }
        }

    private:
        typedef std::is_same<traversal_tag, single_pass_traversal_tag>
            Is_singple_pass;
        typedef optional<value_type> optional_value;
        typedef typename std::conditional<Is_singple_pass::value, optional_value, Input>::type
            Holder;

        Holder current_;
        Input next_;
        BinaryPredicate eq_;
    };

    /** @brief Тип Функционального объекта для создания @c unique_sequence в
    функциональном стиле.
    */
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
            typedef unique_sequence<decltype(::ural::sequence_fwd<Forward>(in))>
                Result;

             return Result(::ural::sequence_fwd<Forward>(in));
        }

        /** @brief Функция создания @c unique_sequence
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

    namespace
    {
        /// @brief Функциональный объект для создания @c unique_sequence в
        constexpr auto & make_unique_sequence
            = odr_const<make_unique_sequence_fn>;

        /// @brief Объект для создающия @c unique_sequence в конвейрном силе.
        constexpr auto & uniqued
            = odr_const<pipeable_maker<make_unique_sequence_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
