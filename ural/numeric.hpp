#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

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

/** @file ural/numeric.hpp
 @brief Обобщённые численные операции
*/

/** @defgroup Numerics Обобщённые численные операции
 @ingroup Algorithms
 @brief Обобщённые реализации численных операций
*/

#include <ural/defs.hpp>
#include <ural/math.hpp>
#include <ural/functional.hpp>
#include <ural/algorithm/core.hpp>
#include <ural/sequence/all.hpp>

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/numeric/partial_sums.hpp>
#include <ural/numeric/adjacent_differences.hpp>

namespace ural
{
inline namespace v1
{
    /** @ingroup Numerics
    @brief Тип функционального объекта для заполнения последовательности
    последовательными значениями
    */
    class iota_fn
    {
    public:
        /** @brief Заполняет последовательность @c seq последовательными
        значениями, начиная с @c init_value
        @param seq последовательность
        @param init_value начальное значение
        @return Значение, следующее за тем, которое было присвоено последнему
        элементу последовательности
        */
        template <class Output, class Incrementable>
        cursor_type_t<Output>
        operator()(Output && seq, Incrementable init_value) const
        {
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputCursor<cursor_type_t<Output>,
                                                           Incrementable>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<Incrementable>));
            BOOST_CONCEPT_ASSERT((concepts::WeakIncrementable<Incrementable>));

            return impl(::ural::cursor_fwd<Output>(seq),
                        std::move(init_value));
        }

    private:
        template <class Output, class Incrementable>
        Output impl(Output out, Incrementable init_value) const
        {
            BOOST_CONCEPT_ASSERT((concepts::OutputCursor<Output, Incrementable>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<Incrementable>));
            BOOST_CONCEPT_ASSERT((concepts::WeakIncrementable<Incrementable>));

            // @todo заменить на алгоритм (нужна неограниченная последовательность)
            for(; !!out; ++ out, (void) ++ init_value)
            {
                *out = init_value;
            }

            return std::move(out);
        }
    };

    /** @ingroup Numerics
    @brief Тип функционального объекта для вычисления суммы элементов
    последовательности
    */
    class accumulate_fn
    {
    public:
        /** @brief Вычисление суммы элементов последовательности
        @param in последовательность
        @param init_value начальное значение
        @param op бинарная операция, используемая в качестве сложения элементов
        @return Сумма @c init_value и всех элементов последовательности
        @todo Вывод начального значения?
        @note Порядок вычисления гарантирован, поэтому мы не требуем
        ассоциативности @c BinaryOperation.
        */
        template <class Input,
                  class T,
                  class BinaryOperation = ::ural::plus<>>
        T operator()(Input && in, T init_value,
                     BinaryOperation op = BinaryOperation()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryOperation, T const *,
                                                             cursor_type_t<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));

            using Result = indirect_callable_result_type_t<BinaryOperation, T const *,
                                                           cursor_type_t<Input>>;

            BOOST_CONCEPT_ASSERT((concepts::Same<T, Result>));

            return impl(::ural::cursor_fwd<Input>(in),
                        std::move(init_value),
                        ::ural::make_callable(std::move(op)));
        }

    private:
        template <class Input, class T, class BinaryOperation>
        T impl(Input in, T init_value, BinaryOperation op) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryOperation, T const *, Input>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));

            using Result = indirect_callable_result_type_t<BinaryOperation, T const *, Input>;
            BOOST_CONCEPT_ASSERT((concepts::Same<T, Result>));

            for(; !!in; ++ in)
            {
                init_value = op(std::move(init_value), *in);
            }

            return init_value;
        }
    };

    /** @ingroup Numerics
    @brief Тип функционального объекта объект для вычисления внутреннего
    произведения
    */
    class inner_product_fn
    {
    public:
        /** @brief Вычисление внутреннего произведения, то есть суммы
        произведений соответствующих элементов, двух последовательностей
        @param in1, in2 входные последовательности
        @param init_value начальное значения, определяющее тип результата
        @param add функциональный объект, определяющий операцию сложения
        @param mult функциональный объект, определяющий операцию умножения
        элементов
        @return Сумма произведений соответствующих элементов двух
        последовательностей
        @todo Что делать с последовательностями разной длинны?
        @todo Перегрузка с выводом типа результата?
        @note Порядок вычисления гарантирован, поэтому мы не требуем
        ассоциативности @c BinaryOperation1.
        */
        template <class Input1, class Input2, class T,
                  class BinaryOperation1 = ::ural::plus<>,
                  class BinaryOperation2 = ::ural::multiplies<>>
        T operator()(Input1 && in1, Input2 && in2, T init_value,
                     BinaryOperation1 add = BinaryOperation1(),
                     BinaryOperation2 mult = BinaryOperation2()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));

            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryOperation2,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));

            using Product = indirect_callable_result_type_t<BinaryOperation2, cursor_type_t<Input1>,
                                                            cursor_type_t<Input2>>;
            BOOST_CONCEPT_ASSERT((concepts::Function<BinaryOperation1, T, Product>));

            using Result = result_type_t<BinaryOperation1, T, Product>;

            BOOST_CONCEPT_ASSERT((concepts::Same<Result, T>));

            return impl(::ural::cursor_fwd<Input1>(in1),
                        ::ural::cursor_fwd<Input2>(in2),
                        std::move(init_value),
                        ::ural::make_callable(std::move(add)),
                        ::ural::make_callable(std::move(mult)));
        }

    private:
        template <class Input1, class Input2, class T,
                  class BinaryOperation1, class BinaryOperation2>
        T impl(Input1 in1, Input2 in2, T value,
               BinaryOperation1 add, BinaryOperation2 mult) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));

            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryOperation2, Input1, Input2>));

            using Product = indirect_callable_result_type_t<BinaryOperation2, Input1, Input2>;
            BOOST_CONCEPT_ASSERT((concepts::Function<BinaryOperation1, T, Product>));

            using Result = result_type_t<BinaryOperation1, T, Product>;
            BOOST_CONCEPT_ASSERT((concepts::Same<Result, T>));

            auto in_prod = ::ural::experimental::make_transform_cursor(std::move(mult),
                                                                         std::move(in1),
                                                                         std::move(in2));
            return ::ural::accumulate_fn{}(std::move(in_prod),
                                           std::move(value),
                                           std::move(add));
        }
    };

    /** @ingroup Numerics
    @brief Тип функционального объекта для вычисления частичных сумм
    */
    class partial_sum_fn
    {
    public:
        /** @brief Запись частичных сумм входной последовательности в выходную
        @param in входная последовательность
        @param out выходная последовательность
        @param bin_op операция, используемая для вычисления суммы. По умолчанию
        используется <tt> ::ural::plus<> </tt>, то есть оператор "плюс".
        @return Непройденные части входной и выходной последовательностей.
        */
        template <class Input, class Output,
                  class BinaryFunction = ::ural::plus<>>
        auto operator()(Input && in, Output && out,
                        BinaryFunction bin_op = BinaryFunction()) const
        -> tuple<decltype(::ural::cursor_fwd<Input>(in)),
                 decltype(::ural::cursor_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryFunction,
                                                             cursor_type_t<Input>,
                                                             cursor_type_t<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));

            using Result = indirect_callable_result_type_t<BinaryFunction, cursor_type_t<Input>,
                                                           cursor_type_t<Input>>;
            BOOST_CONCEPT_ASSERT((concepts::Writable<cursor_type_t<Output>, Result>));

            auto in_sum = ural::experimental::partial_sums(cursor_fwd<Input>(in),
                                                           ::ural::make_callable(std::move(bin_op)));
            auto res = ural::copy_fn{}(std::move(in_sum),
                                       ::ural::cursor_fwd<Output>(out));

            return ural::make_tuple(std::move(res[ural::_1]).base(),
                                    std::move(res[ural::_2]));
        }
    };

    /** @ingroup Numerics
    @brief Тип функционального объекта для вычисления разностей соседних
    элементов
    */
    class adjacent_difference_fn
    {
    public:
        /** @brief Запись разностей соседних элементов входной
        последовательности в выходную
        @param in входная последовательность
        @param out выходная последовательность
        @param bin_op операция, используемая для вычисления разности.
        По умолчанию используется <tt> ::ural::minus<> </tt>, то есть оператор
        "минус".
        @return Непройденные части входной и выходной последовательностей.
        */
        template <class Input, class Output,
                  class BinaryFunction = ::ural::minus<>>
        auto operator()(Input && in, Output && out,
                        BinaryFunction bin_op = BinaryFunction()) const
        -> tuple<decltype(::ural::cursor_fwd<Input>(in)),
                 decltype(::ural::cursor_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryFunction,
                                                             cursor_type_t<Input>,
                                                             cursor_type_t<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));

            using Result = indirect_callable_result_type_t<BinaryFunction, cursor_type_t<Input>,
                                                           cursor_type_t<Input>>;
            BOOST_CONCEPT_ASSERT((concepts::Writable<cursor_type_t<Output>, Result>));

            auto in_dif = ural::experimental::adjacent_differences(::ural::cursor_fwd<Input>(in),
                                                                   ::ural::make_callable(std::move(bin_op)));
            auto res = ural::copy_fn{}(std::move(in_dif),
                                       ::ural::cursor_fwd<Output>(out));
            return ural::make_tuple(std::move(res[ural::_1]).base(),
                                    std::move(res[ural::_2]));
        }
    };

    namespace
    {
        // 26.7 Обобщённые численные операции
        constexpr auto const & accumulate = odr_const<accumulate_fn>;
        constexpr auto const & inner_product = odr_const<inner_product_fn>;
        constexpr auto const & partial_sum = odr_const_holder<partial_sum_fn>::value;
        constexpr auto const & adjacent_difference = odr_const_holder<adjacent_difference_fn>::value;
        constexpr auto const & iota = odr_const<iota_fn>;
    }
}
// namespace v1

namespace experimental
{
    /** @brief Курсор, реализующий операцию свёртки
    @tparam RASequence1 тип первого курсора
    @tparam RASequence2 тип второго курсора
    */
    template <class RACursor1, class RACursor2>
    class convolution_cursor
     : public cursor_base<convolution_cursor<RACursor1, RACursor2> >
    {
    public:
        /// @brief Категория курсора
        using cursor_tag = finite_forward_cursor_tag;

        /// @brief Тип расстояния
        using distance_type = common_type_t<difference_type_t<RACursor1>,
                                            difference_type_t<RACursor2>> ;

        /// @brief Тип значения
        typedef decltype(*std::declval<RACursor1>() * *std::declval<RACursor2>())
            value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /** @brief Конструктор
        @param s1 первая последовательность
        @param s2 вторая последовательность
        */
        convolution_cursor(RACursor1 s1, RACursor2 s2)
         : members_(std::move(s1), std::move(s2), 0, value_type{0})
        {
            this->calc();
        }

        // Однопроходый курсор
        bool operator!() const
        {
            auto const n = members_[ural::_1].size() + members_[ural::_2].size();
            return members_[ural::_3] + 1 >= n;
        }

        reference front() const
        {
            return members_[ural::_4];
        }

        void pop_front()
        {
            ++ members_[ural::_3];
            this->calc();
        }

    private:
        void calc()
        {
            if(!members_[ural::_1] || !members_[ural::_2])
            {
                return;
            }

            auto const pos = members_[ural::_3];

            members_[ural::_4] = 0;

            // j = pos - i
            // 0 <= i <= n1 - 1
            // 0 <= pos - i <= n2-1
            // - pos <= -i <= n2-1 - pos
            // pos-n2+1 <= i <= pos

            auto const n2 = members_[ural::_2].size();
            auto const i_min = std::max(pos - n2 + 1, decltype(pos){0});
            auto const i_max = std::min(members_[ural::_1].size(), pos+1);

            for(auto i = i_min; i < i_max; ++ i)
            {
                assert(pos >= i);

                auto const j = pos - i;

                assert(i >= 0);
                assert(i < members_[ural::_1].size());

                assert(j >= 0);
                assert(j < members_[ural::_2].size());

                members_[ural::_4] += members_[ural::_1][i]* members_[ural::_2][j];
            }
        }

        ural::tuple<RACursor1, RACursor2, distance_type, value_type> members_;
    };

    template <class RASequence1, class RASequence2>
    auto make_convolution_cursor(RASequence1 && s1, RASequence2 && s2)
    -> convolution_cursor<decltype(::ural::cursor_fwd<RASequence1>(s1)),
                            decltype(::ural::cursor_fwd<RASequence2>(s2))>
    {
        return {::ural::cursor_fwd<RASequence1>(s1),
                ::ural::cursor_fwd<RASequence2>(s2)};
    }

    class discrete_convolution_function
    {
    public:
        template <class Vector>
        Vector operator()(Vector const & x, Vector const & y) const
        {
            assert(!ural::empty(x) || !ural::empty(y));

            Vector result(x.size() + y.size() - 1);

            copy_fn{}(::ural::experimental::make_convolution_cursor(x, y),
                      ural::cursor(result));

            return result;
        }
    };

    namespace
    {
        /// @brief Функциональный объект для вычисления дискретной свёртки
        constexpr auto const & discrete_convolution =
            odr_const<discrete_convolution_function>;
    }

    /** @brief Курсор последовательности для вычисления приближённого значения
    квадратого корня по итерационному методу Герона. Смотри, например
    http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
    */
    template <class RealType>
    class sqrt_heron_cursor
     : public cursor_base<sqrt_heron_cursor<RealType>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef RealType value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /** @brief Конструктор
        @param S число, из которого извлекается корень
        @param x0 начальное приближение
        @param eps желаемая точность
        */
        sqrt_heron_cursor(RealType S, RealType x0, RealType eps)
         : s_(std::move(S))
         , x0_(std::move(x0))
         , eps_(std::move(eps) * 0.1)
         , done_(false)
        {
            assert(s_ >= 0);

            if(s_ < eps_)
            {
                x0_ = s_;
            }
        }

        // Однопроходый курсор
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return this->done_;
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return this->x0_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            using std::abs;
            if(abs(ural::square(x0_) - s_) < eps_)
            {
                done_ = true;
                return;
            }

            assert(x0_ > 0);

            x0_ = (x0_ + s_ / x0_) / RealType(2);
        }

    private:
        RealType s_;
        RealType x0_;
        RealType eps_;
        bool done_;
    };

    template <class RealType>
    sqrt_heron_cursor<RealType>
    make_sqrt_heron_cursor(RealType S, RealType x0, RealType eps)
    {
        return sqrt_heron_cursor<RealType>(std::move(S), std::move(x0),
                                             std::move(eps));
    }

    /** @brief Последовательность строк треугольника Паскаля
    @tparam Vector тип массива, используемого для хранения строк
    */
    template <class Vector>
    class pascal_triangle_rows_cursor
     : public cursor_base<pascal_triangle_rows_cursor<Vector>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef Vector value_type;

        /// @brief Тип ссылки
        typedef Vector const & reference;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->front() == {1} </tt>
        */
        pascal_triangle_rows_cursor()
         : row_{1}
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе -- @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return row_;
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            Vector new_row(row_.size() + 1, 1);

            auto const middle = (new_row.size() + 1) / 2;

            for(auto i : ural::numbers(1, middle))
            {
                new_row[i] = row_[i] + row_[i-1];
            }

            for(auto i : ural::numbers(middle, row_.size()))
            {
                new_row[i] = new_row[row_.size() - i];
            }

            row_.swap(new_row);
        }

    private:
        Vector row_;
    };
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
