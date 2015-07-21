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

#include <ural/utility/pipeable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, содержащие все элементы исходной
    последовательности до первого элемента, эквивалентного заданному значению.
    @tparam Sequence тип исходной последовательности
    @tparam Value тип искомого значения
    @tparam BinaryPredicate бинарный предикат
    @todo Возможность не пропускать элемент, равный заданному значению
    @todo выразить через более общий (until_sequence)
    @todo Ввести шаблон адаптора последовательности и выразить через него
    @todo Использовать для последовательности на основе C-строк
    */
    template <class Sequence,
              class Value = ValueType<Sequence>,
              class BinaryPredicate = ::ural::equal_to<>>
    class delimit_sequence
     : public sequence_base<delimit_sequence<Sequence, Value, BinaryPredicate>>
    {
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

    public:
        // Типы
        /// @brief Тип значения
        using value_type = ValueType<Sequence>;

        /// @brief Тип ссылки
        using reference = ReferenceType<Sequence>;

        /// @brief Тип расстояния
        using distance_type = DifferenceType<Sequence>;

        /// @brief Категория обхода
        using traversal_tag = CommonType<forward_traversal_tag,
                                         typename Sequence::traversal_tag>;

        /// @brief Тип указателя
        using pointer = typename Sequence::pointer;

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
         : seq_(std::move(seq))
         , value_(std::move(value))
         , eq_(std::move(bin_pred))
        {}

        // Адаптор
        /** @brief Базовая последовательность
        @return Константная ссылка на базовую последовательность
        */
        Sequence const & base() const
        {
            return this->seq_;
        }

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
            return this->eq_;
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !seq_ || this->relation()(*seq_, this->delimiter());
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            assert(!!*this);
            return this->base().front();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            assert(!!*this);
            return seq_.pop_front();
        }

        // Прямая последовательность
        /** @brief Пройденная часть последовательности
        @return <tt> this->base().traversed_front() </tt>
        */
        Sequence traversed_front() const
        {
            return this->base().traversed_front();
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            return this->seq_.shrink_front();
        }

    private:
        Sequence seq_;
        Value value_;
        FunctionType<BinaryPredicate> eq_;
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
            delimit_sequence<SequenceType<Sequenced>, Value, BinaryPredicate>
            operator()(Sequenced && in, Value value,
                       BinaryPredicate bin_pred = BinaryPredicate()) const
            {
                using Seq = delimit_sequence<SequenceType<Sequenced>, Value, BinaryPredicate>;
                return Seq(::ural::sequence_fwd<Sequenced>(in),
                           std::move(value),
                           std::move(bin_pred));
            }
        };
    }
    // namespace details

    namespace
    {
        constexpr auto const & make_delimit_sequence
            = odr_const<details::make_delimit_sequence_fn>;

        constexpr auto const & delimited =
            odr_const<pipeable_maker<details::make_delimit_sequence_fn>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_DELIMIT_HPP_INCLUDED
