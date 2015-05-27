#ifndef Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED
#define Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED

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

/** @file ural/algorithm/non_modifying.hpp
 @brief ���������, �� �������������� ������������������
*/

/** @defgroup NonModifyingSequenceOperations ���������������� ��������
 @ingroup Algorithms
 @brief ���������� �������� �� ��������� ����� ������������������� � ��
 ���������� ������� ��������� ������������ �������������������.
*/

#include <ural/sequence/function_output.hpp>

#include <ural/algorithm/core.hpp>

namespace ural
{
    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� �������, ������������, ��� ��� ��������
    ������������������ ������������� ���������.
    */
    class all_of_fn
    {
    public:
        /** @brief ���������, ��� ��� �������� ������������������ �������������
        ��������� ���������
        @param in ������� �����������������
        @param pred ��������
        @return @b true, ���� ��� ���� ��������� @c x ������������������ @c in
        ����������� <tt> pred(x) != false </tt>
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !find_if_not_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� �������, ������������, ��� �� ����
    ������� ������������������ �� ������������� ���������.
    */
    class none_of_fn
    {
    public:
        /** @brief ���������, ��� �� ���� ������� ������������������ ��
        ������������� ��������� ���������.
        @param in ������� �����������������
        @param pred ��������
        @return @b true, ���� �� ��� ������ �������� @c x ������������������
        @c in ����������� <tt> pred(x) != false </tt>. ��� ������ �������
        ������������������ ���������� @b true.
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !find_if_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� �������, ������������, ��� ���� �� ����
    ������� ������������������ ������������� ���������.
    */
    class any_of_fn
    {
    public:
        /** @brief ���������, ��� ���� �� ���� ������� ������������������
        ������������� ��������� ���������.
        @param in ������� �����������������
        @param pred ��������
        @return @b true, ���� ��� ���� �� ������ �������� @c x ������������������
        @c in ����������� <tt> pred(x) != false </tt>
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !none_of_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� �������, ����������� ��������
    �������������� ������ ������� �������� ������������������.
    */
    class for_each_fn
    {
    private:
        template <class Input, class UnaryFunction>
        static tuple<Input, UnaryFunction>
        impl(Input in, UnaryFunction f)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction, Input>));

            auto r = ural::copy_fn{}(in, ural::make_function_output_sequence(std::move(f)));
            return ::ural::make_tuple(std::move(r[ural::_1]),
                                      std::move(r[ural::_2]).function());
        }

    public:
        /** @brief ��������� �������������� ������ � ������� ��������
        ������������������
        @param in ������� ������������������
        @param f �������������� ������
        @return ������, ������ ��������� �������� ���������� ������������
        <tt> ::ural::sequence_fwd<Input>(in) </tt> �� ����������, � ������
        --- <tt> ::ural::make_callable(std::move(f)) </tt> ����� ��� ����������
        �� ���� ��������� ������������������.
        */
        template <class Input, class UnaryFunction>
        auto operator()(Input && in, UnaryFunction f) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::make_callable(std::move(f)))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction, SequenceType<Input>>));

            return for_each_fn::impl(::ural::sequence_fwd<Input>(in),
                                     ::ural::make_callable(std::move(f)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ������� �������� ������������������, �������������� ������ ��
    ��������� ������ ������������������
    */
    class find_first_of_fn
    {
    public:
        /** @brief ����� ������� ��������� �������� ������������������
        @param in ������������������, � ������� �������������� �����
        @param s ������������������ ������� ���������
        @param bin_pred �������� ��������, �� ��������� ������������
        <tt> ::ural::equal_to<> </tt>, �� ���� �������� "�����".
        @return ������������������ @c r, ���������� ��
        <tt> ::ural::sequence_fwd<Input>(in) </tt> ������������ �� ��� ���, ����
        <tt> r.front() </tt> �� ������ ������������� ������ �� ��������� @c s.
        */
        template <class Input, class Forward,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Forward && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate,
                                                              SequenceType<Input>,
                                                              SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate, Input, Forward>));

            for(; !!in; ++ in)
            {
                auto r = find_fn{}(s, *in, bin_pred);

                if(!!r)
                {
                    return in;
                }
            }
            return in;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ������� �������� ������������������, �� ��������������
    �� ������ �� ��������� ������ ������������������
    @todo ��������� ������������ � find_first_of
    */
    class find_first_not_of_fn
    {
    public:
        /** @brief ����� ������� ��������, �� ����������� ��������� ������
        �������� ������������������
        @param in ������������������, � ������� �������������� �����
        @param s ������������������ ������� ���������
        @param bin_pred �������� ��������, �� ��������� ������������
        <tt> ::ural::equal_to<> </tt>, �� ���� �������� "�����"
        */
        template <class Input, class Forward,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Forward && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input>));

            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));

            typedef typename Input::reference Ref1;
            typedef typename Forward::reference Ref2;
            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate,
                                                     bool(Ref2, Ref1)>));

            for(; !!in; ++ in)
            {
                auto r = ::ural::find_fn{}(s, *in, bin_pred);

                if(!r)
                {
                    return in;
                }
            }

            return in;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� ������� ��� ������ ���� �������� ���������
    ������������������, ��������������� ��������� �������
    */
    class adjacent_find_fn
    {
    public:
        /** @brief ����� �������� ���������, ��������������� ��������� �������
        @param s ������� ������������������
        @param bin_pred �������� ��������
        @return ������������������ @c r, ���������� ��
        <tt> ::ural::sequence_fwd<Forward>(s) </tt> ������������ �� ��� ���,
        ���� ������ ������������ � ��������� �� ��� ��������� ������
        ������������������ �� ������ �������������� � ������ @c pred.
        */
        template <class Forward, class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward && s,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(pred)));
        }
    private:
        template <class Forward, class BinaryPredicate>
        static Forward impl(Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Forward>));

            if(!s)
            {
                return s;
            }

            auto s_next = ural::next(s);

            for(; !!s_next; ++ s_next)
            {
                if(bin_pred(*s, *s_next))
                {
                    return s;
                }
                s = s_next;
            }
            return s_next;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ��� ��������������� ������� ��� �������� ���������� ���������,
    ��������������� ��������� ���������.
    */
    class count_if_fn
    {
    private:
        template <class Input, class UnaryPredicate>
        static typename Input::distance_type
        impl(Input in, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, Input>));

            typename Input::distance_type result{0};

            for(; !!in; ++ in)
            {
                if (pred(*in))
                {
                    ++ result;
                }
            }
            return result;
        }

    public:
        /** @brief ������������ ���������� ��������� ������������������,
        ��������������� ���������.
        @param in ������� �����������������
        @param pred ��������
        @return ���������� ��������� @c x ������������������ @c in, �����, ���
        <tt> pred(x) != false </tt>.
        */
        template <class Input, class UnaryPredicate>
        auto operator()(Input && in, UnaryPredicate pred) const
        -> typename decltype(::ural::sequence_fwd<Input>(in))::distance_type
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate,
                                                              SequenceType<Input>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ��� ��������������� ������� ��� �������� ���������� ���������,
    ������������� ��������� ��������.
    */
    class count_fn
    {
    private:
        template <class Input, class T, class BinaryPredicate>
        static typename Input::distance_type
        impl(Input in, T const & value, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Input, T const *>));

            return count_if_fn{}(std::move(in),
                                 std::bind(std::move(pred), ural::_1,
                                           std::ref(value)));
        }

    public:
        /** @brief ������������ ���������� ��������� ������������������, ������
        ��������� ��������.
        @param in ������� �����������������
        @param value ��������
        @return ���������� ��������� @c x ������������������ @c in, �����, ���
        <tt> pred(x, value) == true </tt>.
        */
        template <class Input, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, T const & value,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> typename decltype(::ural::sequence_fwd<Input>(in))::distance_type
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input>,
                                                             T const *>));

            return this->impl(::ural::sequence_fwd<Input>(in), value,
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� ������� ��� ������ ������ ���� ���������������
    ��������� �������������������, ������� �� �������� ��������������
    */
    class mismatch_fn
    {
    public:
        /** @brief ����� ���� ������������� ��������������� ���������
        @param in1 ������ ������� ������������������
        @param in2 ������ ������� ������������������
        @param bin_pred �������� ��������, �� ��������� ������������
        <tt> ::ural::equal_to<> </tt>, �� ���� �������� "�����".
        @return ������ �� ���� ������������������� @c r1 � @c r2, ����������
        ������������ <tt> ::ural::sequence_fwd<Input1>(in1) </tt>
        � <tt> ::ural::sequence_fwd<Input1>(in2) </tt> �������������� ���������
        �� ��� ���, ���� �� ������ �������� �� ������ ���������������� � ������
        @c bin_pred.
        */
        template <class Input1, class Input2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input1 && in1, Input2 && in2,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static tuple<Input1, Input2>
        impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Input1, Input2>));

            typedef tuple<Input1, Input2> Tuple;
            for(; !!in1 && !!in2; ++ in1, (void) ++ in2)
            {
                if(!pred(*in1, *in2))
                {
                    break;
                }
            }
            return Tuple{std::move(in1), std::move(in2)};
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� ������� �������� ��������������� ����
    �������������������
    */
    class equal_fn
    {
    public:
        /** @brief �������� ��������� ���� �������������������
        @param in1 ������ ������� ������������������
        @param in2 ������ ������� ������������������
        @param bin_pred �������� ��������, �� ��������� ������������
        <tt> ::ural::equal_to<> </tt>, �� ���� �������� "�����"
        @return @b true, ���� ������������������ @c in1 � @c in2 ����� ������
        ����� � �� ��������������� �������� ������������� ��������� @c bin_pred.
        */
        template <class Input1, class Input2,
                  class BinaryPredicate = ::ural::equal_to<>>
        bool operator()(Input1 && in1, Input2 && in2,
                        BinaryPredicate pred = BinaryPredicate()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Input1>,
                                                                 SequenceType<Input2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(pred)));
        }
    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static bool impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Input1, Input2, BinaryPredicate>));

            auto const r = ural::mismatch_fn{}(std::move(in1), std::move(in2),
                                               std::move(pred));
            return !r[ural::_1] && !r[ural::_2];
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief �������������� ������ ��� �������� ����, ��� ���� ������������������
    �������� ������������� ������
    */
    class is_permutation_fn
    {
    public:
        /** @brief �������� ����, ��� ���� ������������������ ��������
        ������������� ������.
        @param s1, s2 ������ ������������������
        @param pred �������� ��������
        @return @b true, ���� ��� ������� �������� @c s1 ���������� ������������
        � ������ @c pred ������� @c s2.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        bool operator()(Forward1 && s1, Forward2 && s2,
                        BinaryPredicate pred = BinaryPredicate()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward1>,
                                                                 SequenceType<Forward2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward1>(s1),
                              ::ural::sequence_fwd<Forward2>(s2),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static bool
        impl(Forward1 s1, Forward2 s2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward1, Forward2, BinaryPredicate>));

            std::tie(s1, s2) = ural::mismatch_fn{}(std::move(s1), std::move(s2),
                                                   pred);

            s1.shrink_front();
            s2.shrink_front();

            if(ural::size(s1) != ural::size(s2))
            {
                return false;
            }

            for(; !!s1; ++ s1)
            {
                // ���������� ��������, ������� ��� �����������
                if(!!find_fn{}(s1.traversed_front(), *s1, pred))
                {
                    continue;
                }

                auto s = s1;
                ++ s;
                auto const n1 = 1 + count_fn{}(s, *s1, pred);
                auto const n2 = count_fn{}(s2, *s1, pred);

                if(n1 != n2)
                {
                    return false;
                }
            }
            return true;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� ������� ��� ������ ���������������������
    */
    class search_fn
    {
    public:
        /** @brief ����� ���������������������
        @param in ������������������
        @param s ������� ���������������������
        @param bin_pred �������� ��������
        @return ������������������, ���������� �� @c in ������������ �� ���
        ����, ���� @c s �� ������ ������������� � ������ @c pred �������� �
        ������������ �����.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward1 && in, Forward2 && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward1>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward1>,
                                                                 SequenceType<Forward2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward1>(in),
                              ::ural::sequence_fwd<Forward2>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template<class Forward1, class Forward2, class BinaryPredicate>
        static Forward1 impl(Forward1 in, Forward2 s, BinaryPredicate p)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward1, Forward2, BinaryPredicate>));

            for(;; ++ in)
            {
                auto i = in;
                auto i_s = s;
                for(;; ++ i, (void) ++ i_s)
                {
                    if(!i_s)
                    {
                        return in;
                    }
                    if(!i)
                    {
                        return i;
                    }
                    if(!p(*i, *i_s))
                    {
                        break;
                    }
                }
            }
            assert(false);
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief �������������� ������ ������ ��������������������� ����������
    ��������� �������� �����
    */
    class search_n_fn
    {
    public:
        /** @brief ������ ��������������������� ���������� �������� ��������
        �����.
        @param in ������� ������������������
        @param count ����� ���������������������
        @param value ��������
        @return ������������������, ���������� �� @c in ������������ �� ��� ���,
        ���� � ������� �� ����� ��������� ������ @c count ���������,
        ������������� @c value.
        */
        template <class Forward, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward && in,
                        DifferenceType<SequenceType<Forward>> count,
                        T const & value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward>,
                                                                 T const *,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              std::move(count), value,
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Forward, class T,  class BinaryPredicate>
        static Forward impl(Forward in,
                            DifferenceType<Forward> const n,
                            T const & value,
                            BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward, T const *, BinaryPredicate>));

            if(n == 0)
            {
                return in;
            }

            for(; !!in; ++ in)
            {
                if(!bin_pred(*in, value))
                {
                    continue;
                }

                auto candidate = in;
                auto cur_count = DifferenceType<Forward>{0};

                while(true)
                {
                    ++ cur_count;
                    if(cur_count == n)
                    {
                        return candidate;
                    }
                    ++ in;
                    if(!in)
                    {
                        return in;
                    }
                    if(!bin_pred(*in, value))
                    {
                        break;
                    }
                }
            }
            return in;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief ����� ��������������� ������� ��� ������ ���������� ���������
    ��������������������� � ������������������
    */
    class find_end_fn
    {
    public:
        /** @brief ����� ���������� ��������� ���������������������
        @param in ������������������
        @param s ���������������������
        @param bin_pred �������� ��������, �� ��������� ������������
        <tt> equal_to<> </tt>, �� ���� �������� "�����"
        @return ������������������ @c r, ���������� ��
        <tt> ::ural::sequence_fwd<Forward1>(in) </tt> ������������ ����� �������,
        ��� @c s �������� ������������ �������� ������������ ����� ����
        ������������������, ������ @c s ����������� � ������������ �����
        ���� ������������������ ����� ���� ���.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward1 && in, Forward2 && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward1>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Forward1>,
                                                             SequenceType<Forward2>>));

            return this->impl(::ural::sequence_fwd<Forward1>(in),
                              ::ural::sequence_fwd<Forward2>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }
    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static Forward1
        impl(Forward1 in, Forward2 s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Forward1, Forward2>));

            if(!s)
            {
                return in;
            }

            auto result = ::ural::search_fn{}(in, s, bin_pred);;
            auto new_result = result;

            for(;;)
            {
                if(!new_result)
                {
                    return result;
                }
                else
                {
                    result = std::move(new_result);
                    in = result;
                    ++ in;
                    new_result = ::ural::search_fn{}(in, s, bin_pred);
                }
            }
            return result;
        }
    };
}
//namespace ural

#endif
// Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED
