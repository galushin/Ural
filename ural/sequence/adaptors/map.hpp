#ifndef Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MAP_HPP_INCLUDED

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

/** @file ural/sequence/map.hpp
 @brief Адапторы последовательностей для ассоциативных контейнеров
*/

#include <ural/sequence/adaptors/transform.hpp>
#include <ural/utility.hpp>
#include <utility>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор курсора, выделяющего элементы значений-кортежей базового
    курсора.
    @tparam Cursor базовый курсор, элементы которого являются кортеже-подобными.
    @tparam Index индекс элемента значений базового курсора
    */
    template <class Cursor, size_t Index>
    class elements_cursor
     : public cursor_adaptor<elements_cursor<Cursor, Index>,
                             transform_cursor<tuple_get<Index>, Cursor>>
    {
        using Base = cursor_adaptor<elements_cursor<Cursor, Index>,
                                    transform_cursor<tuple_get<Index>, Cursor>>;
    public:
        /** @brief Конструктор
        @param cur базовый курсор
        @post <tt> this->base() == cur </tt>
        */
        explicit elements_cursor(Cursor cur)
         : Base(::ural::experimental::make_transform_cursor(tuple_get<Index>{}, std::move(cur)))
        {}

        //@{
        /** @brief Базовый курсор
        @return Базовый курсор
        */
        Cursor const & base() const &
        {
            return Base::base().bases()[ural::_1];
        }

        Cursor && base() &&
        {
            return static_cast<Base &&>(*this).base().bases()[ural::_1];
        }
        //@}

    private:
        friend Base;

        template <class OtherCursor>
        elements_cursor<OtherCursor, Index>
        rebind_base(transform_cursor<tuple_get<Index>, OtherCursor> cur) const
        {
            return elements_cursor<OtherCursor, Index>(cur.bases()[ural::_1]);
        }
    };
    /** @brief Тип функционального объекта для создания курсора ключей и
    отображаемых значений ассоциативного контейнера.
    @tparam Index индекс элемента, в частности: 0 --- ключи, 1 --- отображаемые
    значения.
    */
    template <size_t Index>
    struct elements_cursor_fn
    {
        /** @brief Создание курсора ключей или отображаемых значений
        ассоциативного контейнера.
        @param seq (под)последовательность элементов ассоциативного контейнера
        */
        template <class Sequence>
        auto operator()(Sequence && seq) const
        {
            using Result = elements_cursor<cursor_type_t<Sequence>, Index>;

            return Result(::ural::cursor_fwd<Sequence>(seq));
        }
    };

    namespace
    {
        /** @brief Объект-тэг для создания курсора последовательности ключей
        ассоциативного контейнера */
        constexpr auto const & map_keys
            = odr_const<experimental::pipeable<elements_cursor_fn<0>>>;

        /** @brief Объект-тэг для создания курсора последовательности
        отоброжаемых значений ассоциативного контейнера */
        constexpr auto const & map_values
            = odr_const<experimental::pipeable<elements_cursor_fn<1>>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
