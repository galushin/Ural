#ifndef Z_URAL_SEQUENCE_ASSUMED_INFINITE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ASSUMED_INFINITE_HPP_INCLUDED

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

/** @file ural/sequnece/adaptors/assumed_infinited.hpp
 @brief Адаптор, предпалагающий, что последовательность бесконечна, то есть
 не будет исчерпана в данном контексте.
*/

#include <ural/utility/pipeable.hpp>
#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/make.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Шаблон адаптора, предпалагающий, что последовательность
    бесконечна, то есть не будет исчерпана в данном контексте.
    @tparam Sequence базовая последовательность
    */
    template <class Sequence>
    class assumed_infinite_sequence
     : public cursor_adaptor<assumed_infinite_sequence<Sequence>, Sequence>
    {
        using Inherited = cursor_adaptor<assumed_infinite_sequence<Sequence>, Sequence>;

    public:
        // Создание, копирование, уничтожение, свойства
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit assumed_infinite_sequence(Sequence seq)
         : Inherited(std::move(seq))
        {}

        // Однопроходый курсор
        /** @brief Проверка исчерпания последовательности
        @return @b false
        */
        bool operator!() const
        {
            return false;
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть последовательности
        @return <tt> this->base().front() </tt>
        */
        TraversedFrontType<Sequence>
        traversed_front() const
        {
            return this->base().traversed_front();
        }
    };

     /** @brief Тип функционального объекта для создания адаптора
    последовательности, предпалагающий, что последовательность бесконечна,
    то есть не будет исчерпана в данном контексте.
    */
    struct assume_infinite_fn
    {
    public:
        /** @brief Создания @c assumed_infinite_sequence
        @param seq последовательность
        @return <tt> Result(ural::cursor_fwd<Sequence>(seq)) </tt>, где
        @c Result -- <tt> assumed_infinite_sequence<cursor_type_t<Sequence>> </tt>
        */
        template <class Sequence>
        assumed_infinite_sequence<cursor_type_t<Sequence>>
        operator()(Sequence && seq) const
        {
            using Result = assumed_infinite_sequence<cursor_type_t<Sequence>>;
            return Result(ural::cursor_fwd<Sequence>(seq));
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания адаптора
        последовательности, предпалагающий, что последовательность бесконечна,
        то есть не будет исчерпана в данном контексте.
        */
        constexpr auto const & assume_infinite = odr_const<assume_infinite_fn>;

        /** @brief Объект для создания адаптора последовательности,
        предпалагающий, что последовательность бесконечна, то есть не будет
        исчерпана в данном контексте, в функциональном стиле.
        */
        constexpr auto const & assumed_infinite
            = odr_const<experimental::pipeable<assume_infinite_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ASSUMED_INFINITE_HPP_INCLUDED
