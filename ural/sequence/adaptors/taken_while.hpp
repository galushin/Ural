#ifndef Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED

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

/** @file ural/sequence/adaptors/taken_while.hpp
 @brief Адаптор последовательности, возвращающий наибольший префикс, все
 элементы которого удовлетворяют заданному предикату.
*/

#include <ural/functional/make_callable.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор курсора, возвращающий наибольший префикс, все элементы
    которого удовлетворяют заданному предикату.
    @tparam Input тип базового курсора
    @tparam Predicate тип унарного предиката
    */
    template <class Input, class Predicate>
    class taken_while_cursor
     : public cursor_adaptor<taken_while_cursor<Input, Predicate>,
                             Input, Predicate>
    {
        using Inherited = cursor_adaptor<taken_while_cursor, Input, Predicate>;

    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag
            = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        // Создание, копирование, уничтожение, свойства
        /** @brief Конструктор
        @param in базовый курсор
        @param pred предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        taken_while_cursor(Input in, Predicate pred)
         : Inherited(std::move(in), std::move(pred))
        {}

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return Inherited::payload();
        }

        // Однопроходный курсор
        /** @brief Проверка исчерпания
        @return <tt> !this->base() || !this->predicate()(*this->base()) </tt>
        */
        bool operator!() const
        {
            return !this->base() || !this->predicate()(*this->base());
        }

    private:
        friend Inherited;

        template <class OtherCursor>
        taken_while_cursor<OtherCursor, Predicate>
        rebind_base(OtherCursor cur) const
        {
            using Result = taken_while_cursor<OtherCursor, Predicate>;
            return Result(std::move(cur), this->predicate());
        }
    };

    /// @brief Функциональный объект для создания @c taken_while_cursor
    class make_taken_while_fn
    {
    public:
        /** @brief Создание @c taken_while_cursor
        @param seq базовая последовательность
        @param Predicate унарный предикат
        */
        template <class Sequence, class Predicate>
        taken_while_cursor<cursor_type_t<Sequence>, function_type_t<Predicate>>
        operator()(Sequence && seq, Predicate pred) const
        {
            using Result = taken_while_cursor<cursor_type_t<Sequence>,
                                                function_type_t<Predicate>>;
            return Result(ural::cursor_fwd<Sequence>(seq),
                          ural::make_callable(std::move(pred)));
        }
    };

    namespace
    {
        /// @brief Функциональный объект создания @c taken_while_cursor
        constexpr auto const & make_taken_while
            = odr_const<make_taken_while_fn>;

        /** @brief Функциональный объект создания @c taken_while_cursor
        в конвейерном стиле.
        */
        constexpr auto const & taken_while
            = odr_const<experimental::pipeable_maker<make_taken_while_fn>>;

    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_WHILE_HPP_INCLUDED
