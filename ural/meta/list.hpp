#ifndef Z_URAL_META_LIST_HPP_INCLUDED
#define Z_URAL_META_LIST_HPP_INCLUDED

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

/** @file ural/meta/list.hpp
 @brief Спиоск типов
 @todo Обобщить алгоритмы на любые контейнеры типов
*/

#include <ural/type_traits.hpp>
#include <ural/defs.hpp>

#include <cstddef>

namespace ural
{
namespace meta
{
    // Вставка нового элемента в начало
    template <class Container, class Value>
    struct push_front;

    template <class Value, template<class...> class Container, class... Args>
    struct push_front<Container<Args...>, Value>
     : declare_type<Container<Value, Args...>>
    {};

    // Удаление первого элемента
    template <class Container>
    struct pop_front
    {
    private:
        template <class U>
        static null_type pop_front_helper(...);

        template <class U>
        static typename U::tail
        pop_front_helper(void*);

    public:
        typedef decltype(pop_front_helper<Container>(nullptr)) type;
    };

    // Первый элемент
    template <class Container>
    struct front
    {
    private:
        template <class U>
        static U front_helper(...);

        template <class U>
        static typename U::head
        front_helper(void*);

    public:
        typedef decltype(front_helper<Container>(nullptr)) type;
    };

    // at
    /** @brief Доступ по индексу
    @tparam Container тип контейнера типов
    @tparam Index номер элемента
    @pre @c Index меньше количества элементов в контейнере
    */
    template <class Container, size_t Index>
    struct at
     : at<typename Container::tail, Index - 1>
    {};

    /** @brief Специализация для <tt> Index == 0 </tt>
    @tparam Container тип контейнера типов
    */
    template <class Container>
    struct at<Container, 0u>
     : declare_type<typename Container::head>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_LIST_HPP_INCLUDED
