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
*/

#include <ural/type_traits.hpp>
#include <ural/defs.hpp>

#include <cstddef>

namespace ural
{
namespace meta
{
    /** @brief Вставка нового элемента в начало контейнера типов
    @tparam Container контейнер типов
    @tparam Value тип, который должне быть добавлен
    */
    template <class Container, class Value>
    struct push_front;

    /** @brief Специализация для контейнеров на основе шаблонов классов с
    переменным количеством аргументов
    @tparam Container шаблон контейнера типа
    @tparam Args типы, уже находящиеся в контейнере типов
    @tparam Value тип, который должне быть добавлен
    */
    template <template <class...> class Container, class Value, class... Args>
    struct push_front<Container<Args...>, Value>
     : declare_type<Container<Value, Args...>>
    {};

    /** Удаление первого элемента из контейнера типов. Если @c Container
    не содержит элементов или не является контейнером типов, то возникнте ошибка
    компиляции. Если вместо этого нужно вернуть другой тип, то следует
    воспользоваться шаблоном @c pop_front_or.
    @brief Удаление первого элемента из контейнера типов.
    @tparam Container контейнер типов
    */
    template <class Container>
    struct pop_front
     : declare_type<typename Container::tail>
    {};

    /** Удаление первого элемента из контейнера типов. Если @c Container
    не содержит элементов или не является контейнером типов, то результатом
    будет @c Default
    @brief Удаление первого элемента из контейнера типов.
    @tparam Container контейнер типов
    @tparam Default результат для случая, если @c Container пуст или не является
    контейнером типов
    */
    template <class Container, class Default>
    struct pop_front_or
    {
    private:
        template <class U>
        static Default
        pop_front_helper(...);

        template <class U>
        static typename U::tail
        pop_front_helper(void*);

    public:
        /// @brief Тип-результат
        typedef decltype(pop_front_helper<Container>(nullptr)) type;
    };

    /** @brief Первый элемент контейнера типов.
    @tparam Container контейнер типов
    */
    template <class Container>
    struct front
     : declare_type<typename Container::head>
    {};

    /** @brief Первый элемент контейнера типов. Если @c Container не является
    непустым контейнером типов, то результатом будет @c Default
    @tparam Container контейнер типов
    */
    template <class Container, class Default = Container>
    struct front_or
    {
    private:
        template <class U>
        static Default front_helper(...);

        template <class U>
        static typename U::head
        front_helper(void*);

    public:
        /// @brief Тип-результат
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
