#ifndef Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED

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

/** @file ural/sequence/outdirected.hpp
 @brief Адаптор курсора последовательности, возвращающий в качестве значения
 текущего элемента исходную последовательность
*/

#include <ural/sequence/adaptor.hpp>
#include <ural/utility/pipeable.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Адаптор последовательности, возвращающий в качестве значения
    исходную последовательность
    @tparam Cursor базовый курсор
    */
    template <class Cursor>
    class outdirected_cursor
     : public cursor_adaptor<outdirected_cursor<Cursor>, Cursor>
    {
        using Inherited = cursor_adaptor<outdirected_cursor, Cursor>;

        template <class U>
        static size_t
        decl_distance_type(...);

        template <class U>
        static decltype(std::declval<U>() - std::declval<U>())
        decl_distance_type(void *);

        template <class U>
        static DifferenceType<U>
        decl_distance_type(declare_type<Cursor> *);

    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = typename Cursor::cursor_tag;

        /// @brief Тип значения
        using value_type = Cursor;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type pointer;

        /// @brief Тип расстояния
        using distance_type = decltype(decl_distance_type<Cursor>(nullptr));

        // Конструктор
        /** @brief Конструктор
        @param s базовая последовательность
        @post <tt> this->base() == s </tt>
        */
        explicit outdirected_cursor(Cursor s)
         : Inherited(std::move(s))
        {}

        // Однопроходый курсор
        /** @brief Текущий элемент
        @return <tt> this->base() </tt>
        */
        reference front() const
        {
            return this->base();
        }

    private:
        friend Inherited;

        template <class OtherCursor>
        outdirected_cursor<OtherCursor>
        rebind_base(OtherCursor s) const
        {
            return outdirected_cursor<OtherCursor>(std::move(s));
        }
    };

    /// @brief Тип функционального объекта для создания @c outdirected_cursor
    class make_outdirected_cursor_fn
    {
    public:
        /** Функция создания @c outdirected_cursor. Данная функция не
        преобразует свой аргумент в последовательность, прежде чем применять
        адаптор. Это связано с тем, что основное предназначение этой функции ---
        преобразовывать типы с оператором ++ в последовательности.
        @brief Функция создания @c outdirected_cursor
        @return <tt> outdirected_cursor<Sequence>{std::move(x)} </tt>
        */
        template <class Sequence>
        auto operator()(Sequence x) const
        {
            return outdirected_cursor<Sequence>{std::move(x)};
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c outdirected_cursor
        constexpr auto const & make_outdirected_cursor
            = odr_const<make_outdirected_cursor_fn>;

        /// Объект для создания @c outdirected_cursor в конвейерном стиле.
        constexpr auto const & outdirected
            = odr_const<experimental::pipeable<make_outdirected_cursor_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
