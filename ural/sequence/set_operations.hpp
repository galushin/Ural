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
 @todo Проверка концепций
*/

#include <ural/functional.hpp>
#include <ural/optional.hpp>

namespace ural
{
    enum class set_operations_state
    {
        first,
        second,
        both,
    };

    /** @brief Последовательность элементов, полученная в результате слияния
    элементов двух базовых последовательностей
    @tparam Input1 первая входная последовательность
    @tparam Input2 вторая входная последовательность
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class merge_sequence
     : public sequence_base<merge_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<merge_sequence, Compare> Base_class;
    public:
        typedef typename std::common_type<typename Input1::reference,
                                          typename Input2::reference>::type reference;

        typedef typename std::common_type<typename Input1::value_type,
                                          typename Input2::value_type>::type value_type;

        typedef typename common_tag<typename Input1::traversal_tag,
                                    typename Input2::traversal_tag,
                                    forward_traversal_tag>::type traversal_tag;

        explicit merge_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_ && !in2_;
        }

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

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            if(!in1_ && !in2_)
            {
                state_ = nullopt;
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

            if(functor()(*in2_, *in1_))
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
        ural::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare>
    auto merge(Input1 && in1, Input2 && in2, Compare cmp)
    -> merge_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef merge_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto merge(Input1 && in1, Input2 && in2)
    -> merge_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return merge(std::forward<Input1>(in1), std::forward<Input2>(in2),
                     ural::less<>{});
    }

    /** @brief Последовательность элементов, полученная в результате пересечения
    множеств элементов двух базовых последовательностей
    @tparam Input1 первая входная последовательность
    @tparam Input2 вторая входная последовательность
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_intersection_sequence
     : public sequence_base<set_intersection_sequence<Input1, Input2, Compare>,
                            Compare>
    {
        typedef sequence_base<set_intersection_sequence, Compare> Base_class;
    public:
        typedef typename Input1::reference reference;
        typedef typename Input1::value_type value_type;

        typedef typename common_tag<typename Input1::traversal_tag,
                                    typename Input2::traversal_tag,
                                    forward_traversal_tag>::type traversal_tag;

        explicit set_intersection_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_ || !in2_;
        }

        reference front() const
        {
            return *in1_;
        }

        void pop_front()
        {
            ++ in1_;
            this->seek();
        }

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(functor()(*in1_, *in2_))
                {
                    ++ in1_;
                }
                else if(functor()(*in2_, *in1_))
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

    template <class Input1, class Input2, class Compare>
    auto set_intersection(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_intersection(Input1 && in1, Input2 && in2)
    -> set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_intersection(std::forward<Input1>(in1),
                                std::forward<Input2>(in2), ural::less<>{});
    }

    /** @brief Последовательность элементов, полученная в результате взятия
    разности множеств элементов двух базовых последовательностей
    @tparam Input1 первая входная последовательность
    @tparam Input2 вторая входная последовательность
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_difference_sequence
     : public sequence_base<set_difference_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<set_difference_sequence, Compare> Base_class;
    public:
        typedef typename Input1::reference reference;
        typedef typename Input1::value_type value_type;

        typedef typename common_tag<typename Input1::traversal_tag,
                                    typename Input2::traversal_tag,
                                    forward_traversal_tag>::type traversal_tag;

        explicit set_difference_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_;
        }

        reference front() const
        {
            return *in1_;
        }

        void pop_front()
        {
            ++ in1_;
            this->seek();
        }

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(functor()(*in1_, *in2_))
                {
                    break;
                }
                else if(functor()(*in2_, *in1_))
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

    template <class Input1, class Input2, class Compare>
    auto set_difference(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_difference(Input1 && in1, Input2 && in2)
    -> set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_difference(std::forward<Input1>(in1),
                              std::forward<Input2>(in2), ural::less<>{});
    }

    /** @brief Последовательность элементов, полученная в результате взятия
    симметрической разности множеств элементов двух базовых последовательностей
    @tparam Input1 первая входная последовательность
    @tparam Input2 вторая входная последовательность
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_symmetric_difference_sequence
     : public sequence_base<set_symmetric_difference_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<set_symmetric_difference_sequence, Compare> Base_class;
    public:
        typedef typename std::common_type<typename Input1::reference,
                                          typename Input2::reference>::type reference;
        typedef typename std::common_type<typename Input1::value_type,
                                          typename Input2::value_type>::type value_type;

        typedef typename common_tag<typename Input1::traversal_tag,
                                    typename Input2::traversal_tag,
                                    forward_traversal_tag>::type traversal_tag;

        explicit set_symmetric_difference_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_ && !in2_;
        }

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

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(functor()(*in1_, *in2_))
                {
                    state_ = set_operations_state::first;
                    return;
                }
                else if(functor()(*in2_, *in1_))
                {
                    state_ = set_operations_state::second;
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
                state_ = nullopt;
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
        ural::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare>
    auto set_symmetric_difference(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_symmetric_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_symmetric_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_symmetric_difference(Input1 && in1, Input2 && in2)
    -> set_symmetric_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_symmetric_difference(std::forward<Input1>(in1),
                                        std::forward<Input2>(in2),
                                        ural::less<>{});
    }

    /** @brief Последовательность элементов, полученная в результате объединения
    множеств элементов двух базовых последовательностей
    @tparam Input1 первая входная последовательность
    @tparam Input2 вторая входная последовательность
    @tparam Compare функция сравнения
    */
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_union_sequence
     : public sequence_base<set_union_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<set_union_sequence, Compare> Base_class;
    public:
        typedef typename std::common_type<typename Input1::reference,
                                          typename Input2::reference>::type reference;
        typedef typename std::common_type<typename Input1::value_type,
                                          typename Input2::value_type>::type value_type;

        typedef typename common_tag<typename Input1::traversal_tag,
                                    typename Input2::traversal_tag,
                                    forward_traversal_tag>::type traversal_tag;

        explicit set_union_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class(std::move(cmp))
         , in1_{std::move(in1)}
         , in2_{std::move(in2)}
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_ && !in2_;
        }

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

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            if(!in1_ && !in2_)
            {
                state_ = nullopt;
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

            if(functor()(*in1_, *in2_))
            {
                state_ = set_operations_state::first;
            }
            else if(functor()(*in2_, *in1_))
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
        ural::optional<set_operations_state> state_;
    };

    template <class Input1, class Input2, class Compare>
    auto set_union(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_union_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_union_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_union(Input1 && in1, Input2 && in2)
    -> set_union_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_union(std::forward<Input1>(in1),
                         std::forward<Input2>(in2), ural::less<>{});
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED
