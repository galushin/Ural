#ifndef Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED

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

/** @file ural/sequence/filtered.hpp
 @brief Последовательность элементов базовой последовательности, удовлетворяющих
 заданному предикату.
*/

#include <ural/sequence/adaptors/remove.hpp>
#include <ural/algorithm/core.hpp>

#include <boost/compressed_pair.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Курсор последовательности элементов базового курсора,
    удовлетворяющих заданному предикату.
    @tparam Input входная последовательность
    @tparam Predicate унарный предикат
    */
    template <class Input, class Predicate>
    class filter_cursor
     : public cursor_adaptor<filter_cursor<Input, Predicate>,
                               remove_if_cursor<Input, not_function<Predicate>>>
    {
        using Base = cursor_adaptor<filter_cursor,
                                    remove_if_cursor<Input, not_function<Predicate>>>;

    public:
        // Конструкторы
        /** @brief Конструктор
        @param in базовый курсор
        @param pred предикат
        @post <tt> this->base() == seq </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit filter_cursor(Input in, Predicate pred)
         : Base{ural::experimental::make_remove_if_cursor(std::move(in), ural::not_fn(std::move(pred)))}
        {}

        // Однопроходый курсор
        /** @brief Пройденная часть последовательности
        @return Пройденная часть последовательности
        */
        filter_cursor<TraversedFrontType<Input>, Predicate>
        traversed_front() const
        {
            using Result = filter_cursor<TraversedFrontType<Input>, Predicate>;
            return Result(Base::base().traversed_front().base(),
                          this->predicate());
        }

        /** @brief Полная последовательность (включая пройденные части)
        @return Полная последовательность
        */
        filter_cursor original() const;

        // Адаптор курсора
        /** @brief Предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return Base::base().predicate().target();
        }

        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Input const & base() const &;

        Input && base() &&
        {
            return static_cast<Base&&>(*this).base().base();
        }
    };

    /// @brief Тип функционального объекта для создания @c filter_cursor
    class make_filter_cursor_fn
    {
    public:
        /** @brief Функция создания @c filter_cursor
        @param seq исходная последовательность
        @param pred унарный предикат
        @return <tt> Seq(::ural::cursor_fwd<Sequence>(seq), make_callable(std::move(pred))) </tt>,
        где @c Seq --- это экземпляр @c filter_cursor с подходящими шаблонными
        параметрами
        */
        template <class Sequence, class Predicate>
        filter_cursor<cursor_type_t<Sequence>, function_type_t<Predicate>>
        operator()(Sequence && seq, Predicate pred) const
        {
            using Result = filter_cursor<cursor_type_t<Sequence>, function_type_t<Predicate>>;

            return Result(::ural::cursor_fwd<Sequence>(seq),
                          ::ural::make_callable(std::move(pred)));
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c filter_cursor
        constexpr auto const & make_filter_cursor
            = odr_const<make_filter_cursor_fn>;

        /** @brief Функциональный объект для создания @c filter_cursor
        в конвейерном стиле
        */
        constexpr auto const & filtered
            = odr_const<experimental::pipeable_maker<make_filter_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
