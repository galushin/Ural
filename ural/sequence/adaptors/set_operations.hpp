#ifndef Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED

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

/** @file ural/sequence/set_operations.hpp
 @brief Последовательности для операций над отсортированными множествами
*/

#include <ural/sequence/make.hpp>
#include <ural/concepts.hpp>
#include <ural/functional.hpp>
#include <ural/optional.hpp>

namespace ural
{
namespace experimental
{
    enum class set_operations_state
    {
        first,
        second,
        both,
    };

    /** @brief Курсор последовательности элементов, полученная в результате
    слияния двух базовых последовательностей.
    @tparam Input1 Тип первого базового курсора
    @tparam Input2 Тип второго базового курсора
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class merge_cursor
     : public cursor_base<merge_cursor<Input1, Input2, Compare>,
                             Compare>
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input1>));

        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input2>));

        typedef bool(Compare_signature)(typename Input1::reference,
                                        typename Input2::reference);

        BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, Compare_signature>));

        typedef cursor_base<merge_cursor, Compare> Base_class;

    public:
        /** @brief Оператора "равно"
        @param x, y аргументы
        @return <tt> x.bases() == y.bases() && x.function() == y.function() </tt>
        */
        friend bool operator==(merge_cursor const & x, merge_cursor const & y)
        {
            return x.first_base() == y.first_base()
                    && x.second_base() == y.second_base()
                    && x.function() == y.function();
        }

        /// @brief Тип ссылки
        using reference = common_type_t<typename Input1::reference, typename Input2::reference>;

        /// @brief Тип значения
        using value_type = common_type_t<value_type_t<Input1>, value_type_t<Input2>>;

        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input1::cursor_tag,
                                      typename Input2::cursor_tag,
                                      finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = common_type_t<typename Input1::pointer, typename Input2::pointer>;

        /// @brief Тип расстояния
        using distance_type = common_type_t<difference_type_t<Input1>, difference_type_t<Input2>>;

        /** @brief Конструктор
        @param in1 первый базовый курсор
        @param in2 второй базовый курсор
        @param cmp функция сравнения
        @post <tt> this->function() == cmp </tt>
        @post <tt> this->first_base() == in1 </tt>
        @post <tt> this->second_base() == in2 </tt>
        */
        explicit merge_cursor(Input1 in1, Input2 in2, Compare cmp = Compare{})
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        // Однопроходый курсор
        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !state_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            if(state_.value() == set_operations_state::second)
            {
                return *in2_;
            }
            else
            {
                return *in1_;
            }
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto const state_value = state_.value();

            if(state_value == set_operations_state::first)
            {
                ++ in1_;
            }
            else if(state_value == set_operations_state::second)
            {
                ++ in2_;
            }
            else
            {
               assert(false);
            }

            this->seek();
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть курсора
        @return Передняя пройденная часть курсора
        */
        merge_cursor<TraversedFrontType<Input1>, TraversedFrontType<Input2>, Compare>
        traversed_front() const
        {
            using Result = merge_cursor<TraversedFrontType<Input1>,
                                          TraversedFrontType<Input2>, Compare>;
            return Result(this->first_base().traversed_front(),
                          this->second_base().traversed_front(),
                          this->function());
        }

        // Адаптор курсоров
        /** @brief Используемая функция сравнения
        @return Используемая функция сравнения
        */
        Compare const & function() const
        {
            return this->payload();
        }

        //@{
        /** @brief Первый базовый курсор
        @return Ссылка на первый базовый курсор
        */
        Input1 const & first_base() const &
        {
            return this->in1_;
        }

        Input1 && first_base() &&
        {
            return std::move(this->in1_);
        }
        //@}

        //@{
        /** @brief Второй базовый курсор
        @return Cсылка на второй базовый курсор
        */
        Input2 const & second_base() const &
        {
            return this->in2_;
        }

        Input2 && second_base() &&
        {
            return std::move(this->in2_);
        }
        //@}

    private:
        void seek()
        {
            if(!in1_ && !in2_)
            {
                state_ = experimental::nullopt;
                return;
            }
            if(!in2_)
            {
                assert(!!in1_);
                state_ = set_operations_state::first;
                return;
            }
            if(!in1_)
            {
                assert(!!in2_);
                state_ = set_operations_state::second;
                return;
            }

            assert(!!in1_ && !!in2_);

            if(this->function()(*in2_, *in1_))
            {
                state_ = set_operations_state::second;
            }
            else
            {
                state_ = set_operations_state::first;
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
        ::ural::experimental::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare>
    auto merged(Input1 && in1, Input2 && in2, Compare cmp)
    -> merge_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))>
    {
        typedef merge_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))> Result;

        return Result(::ural::cursor_fwd<Input1>(in1),
                      ::ural::cursor_fwd<Input2>(in2),
                      ural::make_callable(std::move(cmp)));
    }

    template <class Input1, class Input2>
    auto merged(Input1 && in1, Input2 && in2)
    -> merge_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2))>
    {
        return ::ural::experimental::merged(std::forward<Input1>(in1),
                                            std::forward<Input2>(in2),
                                            ural::less<>{});
    }

    /** @brief Курсор последовательности элементов, полученной в результате
    пересечения множеств элементов двух базовых последовательностей
    @tparam Input1 Тип первого базового курсора
    @tparam Input2 Тип второго базового курсора
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_intersection_cursor
     : public cursor_base<set_intersection_cursor<Input1, Input2, Compare>,
                            Compare>
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input1>));

        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input2>));

        typedef bool(Compare_signature)(typename Input1::reference,
                                        typename Input2::reference);

        BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, Compare_signature>));

        typedef cursor_base<set_intersection_cursor, Compare> Base_class;
    public:
        /** @brief Оператора "равно"
        @param x, y аргументы
        @return <tt> x.bases() == y.bases() && x.function() == y.function() </tt>
        */
        friend bool operator==(set_intersection_cursor const & x,
                               set_intersection_cursor const & y)
        {
            return x.first_base() == y.first_base()
                    && x.second_base() == y.second_base()
                    && x.function() == y.function();
        }

        /// @brief Тип ссылки
        using reference = typename Input1::reference;

        /// @brief Тип значения
        using value_type = value_type_t<Input1>;

        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input1::cursor_tag,
                                      typename Input2::cursor_tag,
                                      finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = typename Input1::pointer;

        /// @brief Тип расстояния
        using distance_type = difference_type_t<Input1>;

        /** @brief Конструктор
        @param in1 первый базовый курсор
        @param in2 второй базовый курсор
        @param cmp функция сравнения
        @post <tt> this->function() == cmp </tt>
        @post <tt> this->first_base() == in1 </tt>
        @post <tt> this->second_base() == in2 </tt>
        */
        explicit set_intersection_cursor(Input1 in1, Input2 in2, Compare cmp = Compare{})
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        // Однопроходый курсор
        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !in1_ || !in2_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return *in1_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            ++ in1_;
            this->seek();
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть курсора
        @return Передняя пройденная часть курсора
        */
        set_intersection_cursor<TraversedFrontType<Input1>,
                                  TraversedFrontType<Input2>, Compare>
        traversed_front() const
        {
            using Result = set_intersection_cursor<TraversedFrontType<Input1>,
                                                   TraversedFrontType<Input2>, Compare>;
            return Result(this->first_base().traversed_front(),
                          this->second_base().traversed_front(),
                          this->function());
        }

        // Адаптор курсоров
        /** @brief Используемая функция сравнения
        @return Используемая функция сравнения
        */
        Compare const & function() const
        {
            return this->payload();
        }

        //@{
        /** @brief Первый базовый курсор
        @return Ссылка на первый базовый курсор
        */
        Input1 const & first_base() const &
        {
            return this->in1_;
        }

        Input1 && first_base() &&
        {
            return std::move(this->in1_);
        }
        //@}

        //@{
        /** @brief Второй базовый курсор
        @return Ссылка на второй базовый курсор
        */
        Input2 const & second_base() const &
        {
            return this->in2_;
        }

        Input2 && second_base() &&
        {
            return std::move(this->in2_);
        }
        //@}

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(function()(*in1_, *in2_))
                {
                    ++ in1_;
                }
                else if(function()(*in2_, *in1_))
                {
                    ++ in2_;
                }
                else
                {
                    break;
                }
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
    };

    template <class Input1, class Input2, class Compare = ural::less<>>
    auto make_set_intersection_cursor(Input1 && in1, Input2 && in2,
                                      Compare cmp = Compare{})
    -> set_intersection_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))>
    {
        typedef set_intersection_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))> Result;

        return Result(::ural::cursor_fwd<Input1>(in1),
                      ::ural::cursor_fwd<Input2>(in2),
                      ural::make_callable(std::move(cmp)));
    }

    /** @brief Курсор последовательности элементов, полученной в результате
    взятия разности множеств элементов двух базовых последовательностей.
    @tparam Input1 Тип первого базового курсора
    @tparam Input2 Тип второго базового курсора
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_difference_cursor
     : public cursor_base<set_difference_cursor<Input1, Input2, Compare>,
                             Compare>
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input1>));

        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input2>));

        typedef bool(Compare_signature)(typename Input1::reference,
                                        typename Input2::reference);

        BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, Compare_signature>));

        typedef cursor_base<set_difference_cursor, Compare> Base_class;
    public:
        /** @brief Оператора "равно"
        @param x, y аргументы
        @return <tt> x.bases() == y.bases() && x.function() == y.function() </tt>
        */
        friend bool operator==(set_difference_cursor const & x,
                               set_difference_cursor const & y)
        {
            return x.first_base() == y.first_base()
                    && x.second_base() == y.second_base()
                    && x.function() == y.function();
        }

        /// @brief Тип ссылки
        using reference = typename Input1::reference;

        /// @brief Тип значения
        using value_type = value_type_t<Input1>;

        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input1::cursor_tag,
                                      typename Input2::cursor_tag,
                                      finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = typename Input1::pointer;

        /// @brief Тип расстояния
        using distance_type = difference_type_t<Input1>;

        /** @brief Конструктор
        @param in1 первый базовый курсор
        @param in2 второй базовый курсор
        @param cmp функция сравнения
        @post <tt> this->first_base() == in1 </tt>
        @post <tt> this->second_base() == in2 </tt>
        @post <tt> this->function() == cmp </tt>
        */
        explicit set_difference_cursor(Input1 in1, Input2 in2, Compare cmp  = Compare{})
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !in1_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return *in1_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            ++ in1_;
            this->seek();
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть курсора
        @return Передняя пройденная часть курсора
        */
        set_difference_cursor<TraversedFrontType<Input1>,
                              TraversedFrontType<Input2>, Compare>
        traversed_front() const
        {
            using Result = set_difference_cursor<TraversedFrontType<Input1>,
                                          TraversedFrontType<Input2>, Compare>;
            return Result(this->first_base().traversed_front(),
                          this->second_base().traversed_front(),
                          this->function());
        }

        // Адаптор курсоров
        /** @brief Используемая функция сравнения
        @return Используемая функция сравнения
        */
        Compare const & function() const
        {
            return this->payload();
        }

        //@{
        /** @brief Первый базовый курсор
        @return Ссылка на первый базовый курсор
        */
        Input1 const & first_base() const &
        {
            return this->in1_;
        }

        Input1 && first_base() &&
        {
            return std::move(this->in1_);
        }
        //@}

        //@{
        /** @brief Второй базовый курсор
        @return Ссылка на второй базовый курсор
        */
        Input2 const & second_base() const &
        {
            return this->in2_;
        }

        Input2 && second_base() &&
        {
            return std::move(this->in2_);
        }
        //@}

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(function()(*in1_, *in2_))
                {
                    break;
                }
                else if(function()(*in2_, *in1_))
                {
                    ++ in2_;
                }
                else
                {
                    ++in1_;
                    ++in2_;
                }
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
    };

    template <class Input1, class Input2, class Compare = less<>>
    auto make_set_difference_cursor(Input1 && in1, Input2 && in2, Compare cmp = Compare{})
    -> set_difference_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))>
    {
        typedef set_difference_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))> Result;

        return Result(::ural::cursor_fwd<Input1>(in1),
                      ::ural::cursor_fwd<Input2>(in2),
                      ural::make_callable(std::move(cmp)));
    }

    /** @brief Курсор последовательности элементов, полученной в результате
    взятия симметрической разности множеств элементов двух базовых
    последовательностей
    @tparam Input1 Тип первого базового курсора
    @tparam Input2 Тип второго базового курсора
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_symmetric_difference_cursor
     : public cursor_base<set_symmetric_difference_cursor<Input1, Input2, Compare>,
                             Compare>
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input1>));

        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input2>));

        typedef bool(Compare_signature)(typename Input1::reference,
                                        typename Input2::reference);

        BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, Compare_signature>));

        typedef cursor_base<set_symmetric_difference_cursor, Compare> Base_class;
    public:
        /** @brief Оператора "равно"
        @param x, y аргументы
        @return <tt> x.bases() == y.bases() && x.function() == y.function() </tt>
        */
        friend bool operator==(set_symmetric_difference_cursor const & x,
                               set_symmetric_difference_cursor const & y)
        {
            return x.first_base() == y.first_base()
                    && x.second_base() == y.second_base()
                    && x.function() == y.function();
        }

        /// @brief Тип ссылки
        using reference = common_type_t<typename Input1::reference, typename Input2::reference>;

        /// @brief Тип значения
        using value_type = common_type_t<value_type_t<Input1>, value_type_t<Input2>>;

        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input1::cursor_tag,
                                      typename Input2::cursor_tag,
                                      finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = common_type_t<typename Input1::pointer, typename Input2::pointer>;

        /// @brief Тип расстояния
        using distance_type = common_type_t<difference_type_t<Input1>, difference_type_t<Input2>>;

        /** @brief Конструктор
        @param in1 первый базовый курсор
        @param in2 второй базовый курсор
        @param cmp функция сравнения
        @post <tt> this->first_base() == in1 </tt>
        @post <tt> this->second_base() == in2 </tt>
        @post <tt> this->function() == cmp </tt>
        */
        explicit set_symmetric_difference_cursor(Input1 in1, Input2 in2,
                                                   Compare cmp = Compare{})
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !in1_ && !in2_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            if(state_.value() == set_operations_state::second)
            {
                return *in2_;
            }
            else
            {
                return *in1_;
            }
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto const state_value = state_.value();

            if(state_value == set_operations_state::first)
            {
                ++ in1_;
            }
            else if(state_value == set_operations_state::second)
            {
                ++ in2_;
            }
            else
            {
                assert(false);
            }

            this->seek();
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        set_symmetric_difference_cursor<TraversedFrontType<Input1>,
                                        TraversedFrontType<Input2>, Compare>
        traversed_front() const
        {
            using Result = set_symmetric_difference_cursor<TraversedFrontType<Input1>,
                                          TraversedFrontType<Input2>, Compare>;
            return Result(this->first_base().traversed_front(),
                          this->second_base().traversed_front(),
                          this->function());
        }

        // Адаптор курсоров
        /** @brief Используемая функция сравнения
        @return Используемая функция сравнения
        */
        Compare const & function() const
        {
            return this->payload();
        }

        //@{
        /** @brief Первый базовый курсор
        @return Ссылка на первый базовый курсор
        */
        Input1 const & first_base() const &
        {
            return this->in1_;
        }

        Input1 && first_base() &&
        {
            return std::move(this->in1_);
        }
        //@}

        //@{
        /** @brief Второй базовый курсор
        @return Ссылка на второй базовый курсор
        */
        Input2 const & second_base() const &
        {
            return this->in2_;
        }

        Input2 && second_base() &&
        {
            return std::move(this->in2_);
        }
        //@}

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(function()(*in1_, *in2_))
                {
                    state_ = set_operations_state::first;
                    return;
                }
                else if(function()(*in2_, *in1_))
                {
                    state_ = set_operations_state::second;
                    return;
                }
                else
                {
                    ++ in1_;
                    ++ in2_;
                }
            }

            if(!!in1_)
            {
                state_ = set_operations_state::first;
            }
            else if(!!in2_)
            {
                state_ = set_operations_state::second;
            }
            else
            {
                state_ = experimental::nullopt;
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
        ural::experimental::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare = less<>>
    auto make_set_symmetric_difference_cursor(Input1 && in1, Input2 && in2,
                                                Compare cmp = Compare{})
    -> set_symmetric_difference_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))>
    {
        typedef set_symmetric_difference_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                 decltype(::ural::cursor_fwd<Input2>(in2)),
                                 decltype(ural::make_callable(std::move(cmp)))> Result;

        return Result(::ural::cursor_fwd<Input1>(in1),
                      ::ural::cursor_fwd<Input2>(in2),
                      ural::make_callable(std::move(cmp)));
    }

    /** @brief Курсор последовательности элементов, полученная в результате
    объединения множеств элементов двух базовых последовательностей.
    @tparam Input1 Тип первого базового курсора
    @tparam Input2 Тип второго базового курсора
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_union_cursor
     : public cursor_base<set_union_cursor<Input1, Input2, Compare>, Compare>
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input1>));

        BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableCursor<Input2>));

        typedef bool(Compare_signature)(typename Input1::reference,
                                        typename Input2::reference);

        BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, Compare_signature>));

        typedef cursor_base<set_union_cursor, Compare> Base_class;
    public:
        /** @brief Оператора "равно"
        @param x, y аргументы
        @return <tt> x.bases() == y.bases() && x.function() == y.function() </tt>
        */
        friend bool operator==(set_union_cursor const & x,
                               set_union_cursor const & y)
        {
            return x.first_base() == y.first_base()
                    && x.second_base() == y.second_base()
                    && x.function() == y.function();
        }

        /// @brief Тип ссылки
        using reference = common_type_t<typename Input1::reference, typename Input2::reference>;

        /// @brief Тип значения
        using value_type = common_type_t<value_type_t<Input1>, value_type_t<Input2>>;

        /// @brief Категория курсора
        using cursor_tag = common_type_t<typename Input1::cursor_tag,
                                      typename Input2::cursor_tag,
                                      finite_forward_cursor_tag>;

        /// @brief Тип указателя
        using pointer = common_type_t<typename Input1::pointer, typename Input2::pointer>;

        /// @brief Тип расстояния
        using distance_type = common_type_t<difference_type_t<Input1>, difference_type_t<Input2>>;

        /** @brief Конструктор
        @param in1 первый базовый курсор
        @param in2 второй базовый курсор
        @param cmp функция сравнения
        @post <tt> this->first_base() == in1 </tt>
        @post <tt> this->second_base() == in2 </tt>
        @post <tt> this->function() == cmp </tt>
        */
        explicit set_union_cursor(Input1 in1, Input2 in2, Compare cmp = Compare{})
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        /** @brief Проверка исчерпания
        @return @b true, если курсор исчерпан, иначе --- @b false.
        */
        bool operator!() const
        {
            return !in1_ && !in2_;
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            if(state_.value() == set_operations_state::second)
            {
                return *in2_;
            }
            else
            {
                return *in1_;
            }
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            auto const state_value = state_.value();

            if(state_value == set_operations_state::first)
            {
                ++ in1_;
            }
            else if(state_value == set_operations_state::second)
            {
                ++ in2_;
            }
            else
            {
                ++ in1_;
                ++ in2_;
            }

            this->seek();
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        set_union_cursor<TraversedFrontType<Input1>,
                           TraversedFrontType<Input2>, Compare>
        traversed_front() const
        {
            using Result = set_union_cursor<TraversedFrontType<Input1>,
                                          TraversedFrontType<Input2>, Compare>;
            return Result(this->first_base().traversed_front(),
                          this->second_base().traversed_front(),
                          this->function());
        }

        // Адаптор курсоров
        /** @brief Используемая функция сравнения
        @return Используемая функция сравнения
        */
        Compare const & function() const
        {
            return this->payload();
        }

        //@{
        /** @brief Первый базовый курсор
        @return Ссылка на первый базовый курсор
        */
        Input1 const & first_base() const &
        {
            return this->in1_;
        }

        Input1 && first_base() &&
        {
            return std::move(this->in1_);
        }
        //@}

        //@{
        /** @brief Второй базовый курсор
        @return Ссылка на второй базовый курсор
        */
        Input2 const & second_base() const &
        {
            return this->in2_;
        }

        Input2 && second_base() &&
        {
            return std::move(this->in2_);
        }
        //@}

    private:
        void seek()
        {
            if(!in1_ && !in2_)
            {
                state_ = experimental::nullopt;
                return;
            }
            if(!in2_)
            {
                state_ = set_operations_state::first;
                return;
            }
            if(!in1_)
            {
                state_ = set_operations_state::second;
                return;
            }

            if(function()(*in1_, *in2_))
            {
                state_ = set_operations_state::first;
            }
            else if(function()(*in2_, *in1_))
            {
                state_ = set_operations_state::second;
            }
            else
            {
                state_ = set_operations_state::both;
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
        ::ural::experimental::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare = less<>>
    auto make_set_union_cursor(Input1 && in1, Input2 && in2,
                                 Compare cmp = Compare{})
    -> set_union_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                          decltype(::ural::cursor_fwd<Input2>(in2)),
                          decltype(ural::make_callable(std::move(cmp)))>
    {
        typedef set_union_cursor<decltype(::ural::cursor_fwd<Input1>(in1)),
                                   decltype(::ural::cursor_fwd<Input2>(in2)),
                                   decltype(ural::make_callable(std::move(cmp)))> Result;

        return Result(::ural::cursor_fwd<Input1>(in1),
                      ::ural::cursor_fwd<Input2>(in2),
                      ural::make_callable(std::move(cmp)));
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED
