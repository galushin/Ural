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

/** @file ural/sequence/cartesian_product.hpp
 @brief Последовательность всех кортежей
 @todo Устранить дублирование с @c transform_sequence
*/

#include <ural/concepts.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/adaptors/delimit.hpp>

namespace ural
{
    /** @brief Последовательность всех кортежей (в лексикографическом порядке)
    @tparam Inputs типы базовых последовательностей

    @note Идея "зациклить" все последовательности, кроме первой, кажется
    соблазнительной, но, к сожалению, это невозможно, так как требуется
    обнаружение "переполнения" для переноса разрядов.
    @todo Усилить категорию обхода
    */
    template <class... Inputs>
    class cartesian_product_sequence
     : public sequence_base<cartesian_product_sequence<Inputs...>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef tuple<typename Inputs::reference...> reference;

        /// @brief Категория обхода
        using traversal_tag
            = typename ural::common_tag<forward_traversal_tag, typename Inputs::traversal_tag...>::type;

        /// @brief Тип значения
        typedef tuple<ValueType<Inputs>...> value_type;

        /** @brief Тип указателя
        У кортежей нет именованных функций-членов, поэтому предоставление
        оператора <tt> -> </tt> нецелесообразно. Следовательно, в качестве типа
        указателя можно взять @b void.
        */
        typedef void pointer;

        /// @brief Тип расстояния
        typedef CommonType<DifferenceType<Inputs>...> distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param ins базовые последовательности
        @post <tt> this->base() == make_tuple(ins...) </tt>
        */
        explicit cartesian_product_sequence(Inputs... ins)
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
                     { return reference((*args)...); };

            return ::ural::apply(f, bases_);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            static_assert(sizeof...(Inputs) > 0, "");
            this->pop_front_impl(placeholder<sizeof...(Inputs) - 1>{});
        }

        // Прямая последовательность
        /** @brief Полная последовательность (вместе с пройденными частями)
        @return Исходная последовательность
        */
        cartesian_product_sequence original() const
        {
            auto f = [this](Inputs const & ... args)->cartesian_product_sequence
                     { return cartesian_product_sequence((args.original())...); };

            return ::ural::apply(f, bases_);
        }

        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        delimit_sequence<cartesian_product_sequence, value_type>
        traversed_front() const
        {
            return make_delimit_sequence(this->original(), this->front());
        }

        void shrink_front();

    private:
        template <size_t I>
        void shrink_fronts(placeholder<I>)
        {
            using Indicies = typename make_int_sequence_helper<size_t, I, sizeof...(Inputs)>::type;

            this->shrink_fronts(Indicies{});
        }

        template <size_t... Is>
        void shrink_fronts(ural::integer_sequence<size_t, Is...>)
        {
            using Sink = int[sizeof...(Is)];
            (void)Sink{(bases_[placeholder<Is>{}].shrink_front(), 0)...};
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

    private:
        tuple<Inputs...> bases_;
    };

    template <class... Inputs1, class... Inputs2>
    bool operator==(cartesian_product_sequence<Inputs1...> const & x,
                    cartesian_product_sequence<Inputs1...> const & y);

    /** @brief Создание последовательности всех возможных кортежей
    @param ins базовые последовательности
    */
    template <class... Inputs>
    cartesian_product_sequence<SequenceType<Inputs>...>
    make_cartesian_product_sequence(Inputs && ... ins)
    {
        typedef cartesian_product_sequence<SequenceType<Inputs>...> Result;
        return Result(::ural::sequence_fwd<Inputs>(ins)...);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED
