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
    /** @brief Адаптор курсора, содержащий все элементы исходной
    последовательности до первого элемента, эквивалентного заданному значению.
    @tparam Input тип базового курсора
    @tparam Value тип искомого значения
    @tparam BinaryPredicate бинарный предикат
    @todo Возможность не пропускать элемент, равный заданному значению
    @todo выразить через более общий (until_sequence)
    @todo Использовать для последовательности на основе C-строк
    */
    template <class Input,
              class Value = value_type_t<Input>,
              class BinaryPredicate = ::ural::equal_to<>>
    class delimit_cursor
     : public cursor_adaptor<delimit_cursor<Input, Value, BinaryPredicate>,
                               Input, BinaryPredicate>
    {

        using Inherited = cursor_adaptor<delimit_cursor, Input, BinaryPredicate>;

    public:
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return <tt> x.base() == y.base() && x.relation() == y.relation()
                     && x.delimiter() == y.delimiter() </tt>
        */
        friend bool operator==(delimit_cursor const & x, delimit_cursor const & y)
        {
            return x.base() == y.base()
                   && x.relation() == y.relation()
                   && x.delimiter() == y.delimiter();
        }

        // Типы
        /// @brief Категория курсора
        using traversal_tag = common_type_t<finite_forward_cursor_tag,
                                         typename Input::cursor_tag>;

        // Создание, копирование, присваивание
        /** @brief Конструктор
        @param in базовый курсор
        @param value искомое значение
        @param bin_pred бинарный предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->delimiter() == value </tt>
        @post <tt> this->relation() == bin_pred </tt>
        */
        delimit_cursor(Input in, Value value, BinaryPredicate bin_pred)
         : Inherited(std::move(in), std::move(bin_pred))
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
        function_type_t<BinaryPredicate> const & relation() const
        {
            return Inherited::payload();
        }

        // Однопроходый курсор
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base() || this->relation()(*this->base(), this->delimiter());
        }

        // Прямой курсор

    private:
        friend Inherited;

        template <class OtherCursor>
        delimit_cursor<OtherCursor, Value, BinaryPredicate>
        rebind_base(OtherCursor cur) const
        {
            using Result = delimit_cursor<OtherCursor, Value, BinaryPredicate>;
            return Result(std::move(cur), this->delimiter(), this->relation());
        }

        Value value_;
    };

    namespace details
    {
        struct make_delimit_cursor_fn
        {
            /** @brief Создание адаптора курсора последовательности, содержащей
            все элементы исходной последовательности до первого элемента,
            эквивалентного заданному значению.
            @param in входная последовательность
            @param value искомое значение
            @param bin_pred бинарный предикат
            @return <tt> Seq(::ural::cursor_fwd<Sequence>(in), std::move(value),
                             ural::make_callable(std::move(bin_pred))) </tt>, где
            @c Seq есть <tt> delimit_cursor<cursor_type_t<Sequence>, Value, BinaryPredicate> </tt>
            */
            template <class Sequence, class Value,
                      class BinaryPredicate = ural::equal_to<>>
            delimit_cursor<cursor_type_t<Sequence>, Value,
                             function_type_t<BinaryPredicate>>
            operator()(Sequence && in, Value value,
                       BinaryPredicate bin_pred = BinaryPredicate()) const
            {
                using Seq = delimit_cursor<cursor_type_t<Sequence>, Value,
                                           function_type_t<BinaryPredicate>>;
                return Seq(::ural::cursor_fwd<Sequence>(in),
                           std::move(value),
                           make_callable(std::move(bin_pred)));
            }
        };
    }
    // namespace details

    namespace
    {
        constexpr auto const & make_delimit_cursor
            = odr_const<details::make_delimit_cursor_fn>;

        constexpr auto const & delimited =
            odr_const<experimental::pipeable_maker<details::make_delimit_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_DELIMIT_HPP_INCLUDED
