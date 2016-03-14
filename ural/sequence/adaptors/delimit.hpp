#ifndef Z_URAL_SEQUENCE_DELIMIT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_DELIMIT_HPP_INCLUDED

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

/** @file ural/sequence/delimit.hpp
 @brief Адапторы последовательностей, содержащие все элементы исходной
 последовательности до первого элемента, эквивалентного заданному значению.
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор последовательности, содержащие все элементы исходной
    последовательности до первого элемента, эквивалентного заданному значению.
    @tparam Sequence тип исходной последовательности
    @tparam Value тип искомого значения
    @tparam BinaryPredicate бинарный предикат
    @todo Возможность не пропускать элемент, равный заданному значению
    @todo выразить через более общий (until_sequence)
    @todo Использовать для последовательности на основе C-строк
    */
    template <class Sequence,
              class Value = ValueType<Sequence>,
              class BinaryPredicate = ::ural::equal_to<>>
    class delimit_sequence
     : public sequence_adaptor<delimit_sequence<Sequence, Value, BinaryPredicate>,
                               Sequence, BinaryPredicate>
    {

        using Inherited = sequence_adaptor<delimit_sequence, Sequence, BinaryPredicate>;

    public:
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return <tt> x.base() == y.base() && x.relation() == y.relation()
                     && x.delimiter() == y.delimiter() </tt>
        */
        friend bool operator==(delimit_sequence const & x, delimit_sequence const & y)
        {
            return x.base() == y.base()
                   && x.relation() == y.relation()
                   && x.delimiter() == y.delimiter();
        }

        // Типы
        /// @brief Категория курсора
        using traversal_tag = CommonType<finite_forward_cursor_tag,
                                         typename Sequence::cursor_tag>;

        // Создание, копирование, присваивание
        /** @brief Конструктор
        @param seq базовая последовательность
        @param value искомое значение
        @param bin_pred бинарный предикат
        @post <tt> this->base() == seq </tt>
        @post <tt> this->delimiter() == value </tt>
        @post <tt> this->relation() == bin_pred </tt>
        */
        delimit_sequence(Sequence seq, Value value, BinaryPredicate bin_pred)
         : Inherited(std::move(seq), std::move(bin_pred))
         , value_(std::move(value))
        {}

        // Адаптор
        /** @brief Значение, на котором нужно остановиться
        @return Константная ссылка на значение, на котором нужно остановиться
        */
        Value const & delimiter() const
        {
            return this->value_;
        }

        /** @brief Отношение, определяющее эквивалентность значению, на котором
        нужно остановиться.
        @return Отношение, определяющее эквивалентность значению, на котором
        нужно остановиться.
        */
        FunctionType<BinaryPredicate> const & relation() const
        {
            return Inherited::payload();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base() || this->relation()(*this->base(), this->delimiter());
        }

        // Прямая последовательность

    private:
        friend Inherited;

        template <class OtherSequence>
        delimit_sequence<OtherSequence, Value, BinaryPredicate>
        rebind_base(OtherSequence s) const
        {
            using Result = delimit_sequence<OtherSequence, Value, BinaryPredicate>;
            return Result(std::move(s), this->delimiter(), this->relation());
        }

        Value value_;
    };

    namespace details
    {
        struct make_delimit_sequence_fn
        {
            /** @brief Создание адаптор последовательности, содержащие все элементы
            исходной последовательности до первого элемента, эквивалентного заданному
            значению
            @param in входная последовательность
            @param value искомое значение
            @param bin_pred бинарный предикат
            @return <tt> Seq(::ural::sequence_fwd<Sequenced>(in), std::move(value),
                             ural::make_callable(std::move(bin_pred))) </tt>, где
            @c Seq есть <tt> delimit_sequence<SequenceType<Sequenced>, Value, BinaryPredicate> </tt>
            */
            template <class Sequenced, class Value,
                      class BinaryPredicate = ural::equal_to<>>
            delimit_sequence<SequenceType<Sequenced>, Value,
                             FunctionType<BinaryPredicate>>
            operator()(Sequenced && in, Value value,
                       BinaryPredicate bin_pred = BinaryPredicate()) const
            {
                using Seq = delimit_sequence<SequenceType<Sequenced>, Value,
                                             FunctionType<BinaryPredicate>>;
                return Seq(::ural::sequence_fwd<Sequenced>(in),
                           std::move(value),
                           make_callable(std::move(bin_pred)));
            }
        };
    }
    // namespace details

    namespace
    {
        constexpr auto const & make_delimit_sequence
            = odr_const<details::make_delimit_sequence_fn>;

        constexpr auto const & delimited =
            odr_const<experimental::pipeable_maker<details::make_delimit_sequence_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_DELIMIT_HPP_INCLUDED
