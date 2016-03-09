#ifndef Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED

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

/** @file ural/sequence/replace.hpp
 @brief Создание последовательностей, в которых элементы, удовлетворяющие
 определённым условиям, заменены заданным значением.
*/

#include <ural/sequence/adaptors/transform.hpp>
#include <ural/functional/replace.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор последовательности, заменяющий элементы, удовлетворяющие
    заданному условию на новое значение.
    @tparam Sequence тип последовательности
    @tparam Predicate тип унарного предиката
    @tparam T тип нового значения
    */
    template <class Sequence, class Predicate, class T>
    class replace_if_sequence
     : public sequence_adaptor<replace_if_sequence<Sequence, Predicate, T>,
                               transform_sequence<experimental::replace_if_function<Predicate, T>, Sequence>>
    {
        using Base = sequence_adaptor<replace_if_sequence<Sequence, Predicate, T>,
                                      transform_sequence<experimental::replace_if_function<Predicate, T>, Sequence>>;
    public:
        // Конструирование
        /** @brief Конструктор
        @param seq исходная последовательность
        @param pred условие замены
        @param new_value новое значение
        */
        replace_if_sequence(Sequence seq, Predicate pred, T new_value)
         : Base(replace_if_sequence::make_base(std::move(seq), std::move(pred),
                                               std::move(new_value)))
        {}

        // Адаптор
        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return Base::base().bases()[ural::_1];
        }

        Sequence && base() &&
        {
            return static_cast<Base &&>(*this).base().bases()[ural::_1];
        }
        //@}

        /** @brief Используемое условие
        @return Используемое условие
        */
        Predicate const & predicate() const
        {
            return Base::base().function().predicate();
        }

        /** @brief Новое значение
        @return Новое значение
        */
        T const & new_value() const
        {
            return Base::base().function().new_value();
        }

    private:
        friend Base;

        using Transformator = experimental::replace_if_function<Predicate, T>;

        template <class OtherSequence>
        replace_if_sequence<OtherSequence, Predicate, T>
        rebind_base(transform_sequence<Transformator, OtherSequence> seq) const
        {
            using Result = replace_if_sequence<OtherSequence, Predicate, T>;
            return Result(std::move(seq).bases()[ural::_1], this->predicate(),
                          this->new_value());
        }

        static transform_sequence<experimental::replace_if_function<Predicate, T>, Sequence>
        make_base(Sequence seq, Predicate pred, T new_value)
        {
            using Tr = experimental::replace_if_function<Predicate, T>;
            auto f = Tr(std::move(pred), std::move(new_value));
            return std::move(seq) | ::ural::experimental::transformed(std::move(f));
        }
    };

    /** @brief Адаптор последовательности, заменящий элементы, эквивалентные
    заданному значению, на новое значение.
    @tparam Sequence тип исходной последовательности
    @tparam T1 тип заменяемого значения
    @tparam T2 тип нового значения
    @tparam BinaryPredicate тип бинарного предиката, используемого для
    определения эквивалентности элементов, если не задан, то используется
    оператор "равно".
    */
    template <class Sequence, class T1, class T2 = T1, class BinaryPredicate = equal_to<>>
    class replace_sequence
     : public sequence_adaptor<replace_sequence<Sequence, T1, T2, BinaryPredicate>,
                               transform_sequence<experimental::replace_function<T1, T2, BinaryPredicate>, Sequence>>
    {
        using Base = sequence_adaptor<replace_sequence<Sequence, T1, T2, BinaryPredicate>,
                               transform_sequence<experimental::replace_function<T1, T2, BinaryPredicate>, Sequence>>;
    public:
        // Конструирование
        /** @brief Конструктор
        @param seq исходная последовательность
        @param pred условие замены
        @param new_value новое значение
        */
        replace_sequence(Sequence seq, T1 old_value, T2 new_value,
                         BinaryPredicate bin_pred)
         : Base(replace_sequence::make_base(std::move(seq),
                                            std::move(old_value),
                                            std::move(new_value),
                                            std::move(bin_pred)))
        {}

        // Адаптор
        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return Base::base().bases()[ural::_1];
        }

        Sequence && base() &&
        {
            return static_cast<Base &&>(*this).base().bases()[ural::_1];
        }
        //@}

        /** @brief Используемое условие
        @return Используемое условие
        */
        BinaryPredicate const & predicate() const
        {
            return Base::base().function().predicate();
        }

        /** @brief Заменяемое значение
        @return Заменяемое значение
        */
        T1 const & old_value() const
        {
            return Base::base().function().old_value();
        }

        /** @brief Новое значение
        @return Новое значение
        */
        T2 const & new_value() const
        {
            return Base::base().function().new_value();
        }

    private:
        friend Base;

        template <class OtherSequence>
        replace_sequence<OtherSequence, T1, T2, BinaryPredicate>
        rebind_base(OtherSequence s) const
        {
            using Seq = replace_sequence<OtherSequence, T1, T2, BinaryPredicate>;
            return Seq(std::move(s), this->old_value(), this->new_value(),
                       this->predicate());
        }

        static
        transform_sequence<experimental::replace_function<T1, T2, BinaryPredicate>, Sequence>
        make_base(Sequence seq, T1 old_value, T2 new_value, BinaryPredicate pred)
        {
            using Function = experimental::replace_function<T1, T2, BinaryPredicate>;
            auto f = Function(std::move(old_value), std::move(new_value),
                              std::move(pred));
            return std::move(seq) | ::ural::experimental::transformed(std::move(f));
        }
    };

    /// @brief Тип функционального объекта для создания @c replace_sequence
    struct make_replace_sequence_fn
    {
    public:
        /** @brief Создание последовательности, в которой элементы,
        эквивалентные заданному значению, заменены на другое значение.
        @param seq исходная последовательность
        @param old_value заменяемое значение
        @param new_value новое значение
        @param bin_pred бинарный предикат, если не указать его, будет
        использоваться оператор ==
        */
        template <class Sequenced, class T1, class T2,
                  class BinaryPredicate = equal_to<>>
        auto operator()(Sequenced && seq,
                        T1 old_value, T2 new_value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        {
            using T1R = typename reference_wrapper_to_reference<T1>::type;
            using T2R = typename reference_wrapper_to_reference<T2>::type;
            using Result = replace_sequence<SequenceType<Sequenced>,
                                            T1R, T2R,
                                            FunctionType<BinaryPredicate>>;
            return Result(::ural::sequence_fwd<Sequenced>(seq),
                          std::move(old_value),
                          std::move(new_value),
                          ::ural::make_callable(std::move(bin_pred)));

        }
    };

    /// @brief Тип функционального объекта для создания @c replace_if_sequence
    struct make_replace_if_sequence_fn
    {
    public:
        /** @brief Создание последовательности, в которой элементы,
        удовлетворяющие заданному предикату, заменены на другое значение.
        @param seq исходная последовательность
        @param pred предикат, определяющий, какие нужно заменить.
        @param new_value новое значение
        */
        template <class Sequenced, class Predicate, class T>
        auto operator()(Sequenced && seq, Predicate pred, T new_value) const
        {
            using TR = typename reference_wrapper_to_reference<T>::type;
            using Result = replace_if_sequence<SequenceType<Sequenced>,
                                               FunctionType<Predicate>, TR>;

            return Result(::ural::sequence_fwd<Sequenced>(seq),
                          ::ural::make_callable(std::move(pred)),
                          std::move(new_value));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c replace_sequence
        constexpr auto const & make_replace_sequence
            = odr_const<make_replace_sequence_fn>;

        /** @brief Функциональный объект для создания @c replace_if_sequence
        в конвейерном стиле.
        */
        constexpr auto const & replaced
            = odr_const<experimental::pipeable_maker<make_replace_sequence_fn>>;

        /// @brief Функциональный объект для создания @c replace_if_sequence
        constexpr auto const & make_replace_if_sequence
            = odr_const<make_replace_if_sequence_fn>;

        /** @brief Функциональный объект для создания @c replace_sequence
        в конвейерном стиле.
        */
        constexpr auto const & replaced_if
            = odr_const<experimental::pipeable_maker<make_replace_if_sequence_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
