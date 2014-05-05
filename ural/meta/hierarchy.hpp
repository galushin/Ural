#ifndef Z_URAL_META_HIERARCHY_HPP_INCLUDED
#define Z_URAL_META_HIERARCHY_HPP_INCLUDED

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

/** @file ural/meta/hierarchy.hpp
 @brief Средства генерации иерархий
*/

namespace ural
{
namespace meta
{
    /** @brief Наследование от всех элементов списка типов
    @tparam Container список типов
    */
    template <class Container>
    struct inherit_from
     : public Container::head
     , public inherit_from<typename Container::tail>
    {
    public:
        constexpr inherit_from() = default;

        constexpr inherit_from(inherit_from &) = default;
        constexpr inherit_from(inherit_from &&) = default;
        constexpr inherit_from(inherit_from const &) = default;

        template <class A1, class... As>
        constexpr inherit_from(A1 && a1, As && ... as)
         : Container::head{std::forward<A1>(a1)}
         , inherit_from<typename Container::tail>{std::forward<As>(as)...}
        {}

        ~inherit_from() = default;
    };

    template <>
    struct inherit_from<null_type>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_HIERARCHY_HPP_INCLUDED
