#ifndef Z_URAL_ALGORITHM_COPY_HPP_INCLUDED
#define Z_URAL_ALGORITHM_COPY_HPP_INCLUDED

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

/** @file ural/algorithm/core.hpp
 @brief Базовые алгоритмы, через которые реализуются многие другие алгоритмы
 и операции курсоров.
 @note Этот файл предназначен для разработчиков библиотеки, не включайте его
 в свой код, только если это действительно необходимо.
*/

#include <ural/sequence/make.hpp>
#include <ural/concepts.hpp>
#include <ural/tuple.hpp>

#include <boost/concept/assert.hpp>

namespace ural
{
inline namespace v1
{
    /** @brief Функциональный объект, реализующий копирование элементов одной
    последовальности в другую
    @ingroup MutatingSequenceOperations
    */
    class copy_fn
    {
    private:
        template <class Input, class Output>
        static ural::tuple<Input, Output>
        copy_impl(Input in, Output out)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, Output>));

            for(; !!in && !!out; ++ in, (void) ++ out)
            {
                *out = *in;
            }

            return ural::tuple<Input, Output>(std::move(in), std::move(out));
        }

    public:
        /** Копирует элементы последовательности @c in в последовательность
        @c out по очереди, пока одна из них не будет исчерпана.
        @brief Копирование последовательностей
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части входной и выходной
        последовательностей (одна из них будет пустой).
        */
        template <class Input, class Output>
        tuple<cursor_type_t<Input>, cursor_type_t<Output>>
        operator()(Input && in, Output && out) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<cursor_type_t<Input>,
                                                               cursor_type_t<Output>>));

            return ::ural::copy_fn::copy_impl(::ural::cursor_fwd<Input>(in),
                                              ::ural::cursor_fwd<Output>(out));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск элемента
    последовательности, удовлетворяющего заданному предикату.
    @ingroup NonModifyingSequenceOperations
    */
    class find_if_fn
    {
    private:
        template <class Input, class Predicate>
        static Input
        impl(Input in, Predicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, Input>));

            for(; !!in; ++ in)
            {
                if(pred(*in))
                {
                    return in;
                }
            }
            return in;
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param pred унарный предикат
        @return Последовательность @c r, полученная из
        <tt> ::ural::cursor_fwd<Input>(in) </tt> продвижением до тех пор,
        пока не выполнится условие <tt> pred(r.front()) != false </tt>.
        */
        template <class Input, class Predicate>
        cursor_type_t<Input>
        operator()(Input && in, Predicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, cursor_type_t<Input>>));

            return this->impl(::ural::cursor_fwd<Input>(in),
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск заданного
    значения в последовательности.
    @ingroup NonModifyingSequenceOperations
    */
    class find_fn
    {
    private:
        template <class Input, class T, class BinaryPredicate>
        static Input
        impl(Input in, T const & value, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate, Input, T const *>));

            auto pred = [&](auto const & x) { return bin_pred(x, value); };

            return find_if_fn{}(std::move(in), std::move(pred));
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param value значение, которое нужно найти
        @param pred бинарный предикат, используемый для сравнения элементов
        последовательности и заданного значения. Если этот параметр не указан,
        то используется <tt> equal_to<>() </tt>, то есть оператор "равно".
        @return Последовательность @c r, полученная из
        <tt> ::ural::cursor_fwd<Input>(in) </tt> продвижением до тех пор, пока
        не встретится элемент @c x такой, что <tt> pred(r.front(), value) </tt>.
        */
        template <class Input, class T,
                  class BinaryPredicate = ural::equal_to<>>
        cursor_type_t<Input>
        operator()(Input && in, T const & value,
                   BinaryPredicate pred = BinaryPredicate()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate, cursor_type_t<Input>, T const *>));

            return this->impl(::ural::cursor_fwd<Input>(in), value,
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск элемента
    последовательности, не удовлетворяющего заданному предикату.
    @ingroup NonModifyingSequenceOperations
    */
    class find_if_not_fn
    {
    private:
        template <class Input, class Predicate>
        static Input impl(Input in, Predicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, Input>));

            return find_if_fn{}(std::move(in), ural::not_fn(std::move(pred)));
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param pred унарный предикат
        @return Последовательность @c r, полученная из
        <tt> ::ural::cursor_fwd<Input>(in) </tt> продвижением до тех пор, пока
        не выполнится условие <tt> pred(r.front()) == false </tt>.
        */
        template <class Input, class Predicate>
        cursor_type_t<Input>
        operator()(Input && in, Predicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, cursor_type_t<Input>>));

            return this->impl(::ural::cursor_fwd<Input>(in), std::move(pred));
        }
    };
}
// namespace v1
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
