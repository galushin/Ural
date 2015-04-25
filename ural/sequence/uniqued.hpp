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

#include <ural/algorithm/details/algo_base.hpp>

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
        typedef typename Input::value_type value_type;

        /// @brief Категория обхода
        typedef typename ural::common_tag<typename Input::traversal_tag,
                                          forward_traversal_tag>::type traversal_tag;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Input::distance_type distance_type;

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
        /** @brief Базовая последовательность
        @return Базовая последовательность
        @note <tt> this->base().front() </tt> и <tt> this->front() </tt>
        ссылаются на разные элементы в случае однопроходной последовательности
        */
        Input const & base() const
        {
            return this->base_impl(traversal_tag{});
        }

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
        Input const & base_impl(single_pass_traversal_tag) const
        {
            return this->next_;
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

    /** @brief Функция создания @c unique_sequence
    @param in входная последовательность
    @param pred бинарный предикат
    */
    template <class Forward, class BinaryPredicate>
    auto make_unique_sequence(Forward && in, BinaryPredicate pred)
    -> unique_sequence<decltype(::ural::sequence_fwd<Forward>(in)),
                        decltype(make_callable(std::move(pred)))>
    {
        typedef unique_sequence<decltype(::ural::sequence_fwd<Forward>(in)),
                                decltype(make_callable(std::move(pred)))> Seq;
        return Seq(::ural::sequence_fwd<Forward>(in),
                   make_callable(std::move(pred)));
    }

    /** @brief Функция создания @c unique_sequence
    @param in входная последовательность
    @return <tt> unique_sequence<Seq>(sequence_fwd<Forward>(in)) </tt>, где
    @c Seq -- <tt> unique_sequence<decltype(sequence_fwd<Forward>(in))> </tt>
    */
    template <class Forward>
    auto make_unique_sequence(Forward && in)
    -> unique_sequence<decltype(::ural::sequence_fwd<Forward>(in))>
    {
        typedef unique_sequence<decltype(::ural::sequence_fwd<Forward>(in))>
            Result;

         return Result(::ural::sequence_fwd<Forward>(in));
    }

    /** @brief Тип вспомогательного объека для создания @c unique_sequence
    с заданным предикатом в конвейерном стиле
    @tparam Predicate унарный предикат
    */
    template <class Predicate>
    class uniqued_helper_custom
    {
    public:
        /// @brief Используемый предикат
        Predicate predicate;
    };

    /** @brief Тип вспомогательного объекта для создания @c unique_sequence
    в конвейрном стиле
    */
    struct uniqued_helper
    {
    public:
        /** @brief Создание вспомогательного объекта, хранящего предикат
        @param pred предикат
        */
        template <class Predicate>
        auto operator()(Predicate pred) const
        -> uniqued_helper_custom<decltype(make_callable(std::move(pred)))>
        {
            return {std::move(pred)};
        }
    };

    /** @brief Создание @c unique_sequence в конвейерном стиле
    @param in входная последовательность
    @return <tt> make_unique_sequence(std::forward<Forward>(in)) </tt>
    */
    template <class Forward>
    auto operator|(Forward && in, uniqued_helper)
    -> unique_sequence<decltype(::ural::sequence_fwd<Forward>(in))>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in));
    }

    /** @brief Создание @c unique_sequence в конвейерном стиле
    @param in входная последовательность
    @param helper объект, хранящий бинарный предикат
    @return <tt> make_unique_sequence(std::forward<Forward>(in), helper.predicate) </tt>
    */
    template <class Forward, class Predicate>
    auto operator|(Forward && in, uniqued_helper_custom<Predicate> helper)
    -> unique_sequence<decltype(::ural::sequence_fwd<Forward>(in)), Predicate>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in),
                                            helper.predicate);
    }

    /// @brief Функциональный объект, создающий @c unique_sequence
    constexpr auto uniqued = uniqued_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
