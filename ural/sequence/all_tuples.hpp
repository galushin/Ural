#ifndef Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED

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

/** @file ural/sequence/all_tuples.hpp
 @brief Последовательность всех кортежей
 @todo Устранить дублирование с @c transform_sequence
*/

#include <ural/concepts.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>

namespace ural
{
    /** @brief Последовательность всех кортежей (в лексикографическом порядке)
    @tparam Inputs типы базовых последовательностей

    Идея "зациклить" все последовательности, кроме первой, кажется
    соблазнительной, но, к сожалению, это невозможно, так как требуется
    обнаружение "переполнения" для переноса разрядов.
    */
    template <class... Inputs>
    class all_tuples_sequence
     : public sequence_base<all_tuples_sequence<Inputs...>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef tuple<typename Inputs::reference...> reference;

        /// @brief Тип значения
        typedef tuple<typename Inputs::value_type...> value_type;

        // Конструкторы
        /** @brief Конструктор
        @param ins базовые последовательности
        @post <tt> this->base() == make_tuple(ins...) </tt>
        */
        explicit all_tuples_sequence(Inputs... ins)
         : bases_{std::move(ins)...}
        {
            typedef typename std::tuple_element<0, decltype(bases_)>::type
                Front_seq;
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Front_seq>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Front_seq>));

            this->shrink_fronts(ural::_2);
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !bases_[ural::_1];
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            auto f = [this](Inputs const & ... args)->reference
                     { return this->deref(args...); };

            return apply(f, bases_);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            static_assert(sizeof...(Inputs) > 0, "");
            this->pop_front_impl(placeholder<sizeof...(Inputs) - 1>{});
        }

    private:
        template <size_t I>
        void shrink_fronts(placeholder<I>)
        {
            typedef typename std::tuple_element<I, decltype(bases_)>::type
                Seq;
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Seq>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Seq>));

            bases_[ural::_1].shrink_front();
            return this->shrink_fronts(placeholder<I+1>{});
        }

        void shrink_fronts(placeholder<sizeof...(Inputs)>)
        {
            return;
        }

        void pop_front_impl(placeholder<0>)
        {
            ++ bases_[ural::_1];
        }

        template <size_t I>
        void pop_front_impl(placeholder<I> first)
        {
            ++ bases_[first];

            if(!bases_[first])
            {
                bases_[first] = bases_[first].traversed_front();
                return this->pop_front_impl(placeholder<I-1>{});
            }
        }

        reference deref(Inputs const & ... ins) const
        {
            return reference((*ins)...);
        }

    private:
        tuple<Inputs...> bases_;
    };

    /** @brief Создание последовательности всех возможных кортежей
    @param ins базовые последовательности
    */
    template <class... Inputs>
    auto make_all_tuples_sequence(Inputs && ... ins)
    -> all_tuples_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
    {
        typedef all_tuples_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
            Result;
        return Result{sequence(std::forward<Inputs>(ins))...};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED
