#ifndef Z_URAL_DEFS_HPP_INCLUDED
#define Z_URAL_DEFS_HPP_INCLUDED

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

/** @file ural/defs.hpp
 @brief Определения основных типов, используемых библиотекой
*/

#include <type_traits>

/// @brief Макрос для предотвращения макро-подстановки при объявлении функции
#define URAL_PREVENT_MACRO_SUBSTITUTION

namespace ural
{
    /** Пустой базовый класс. Используется, когда формально требуется указать
    базовый класс, но никакой базовый класс на самом деле не нужен.
    @brief Пустой базовый класс.
    */
    struct empty_type{};

    template <class... Types>
    struct typelist
    {};

    /** Класс-тэг обозначающий отсутствие типа. Используется, например, как
    пустой список типов
    @brief Класс-тэг обозначающий отсутствие типа.
    */
    typedef typelist<> null_type;

    template <class Head, class... Others>
    struct typelist<Head, Others...>
    {
        /// @brief Первый элемент списка типов
        typedef Head head;

        /// @brief Остальные элементы списка типов
        typedef typelist<Others...> tail;
    };

    /** @brief Тип-тэг, обозначающий, что нужно использовать значение по
    умолчанию
    */
    struct use_default{};

    /** @brief Тип-тэг, обозначающий, что тип параметров должен быть выведен
    по фактическим аргументам
    */
    struct auto_tag{};

    /** @brief Тип-тэг, обозначающий, что используется конструкция, в которой
    предусловие должно быть выполнено пользователем.
    */
    struct unsafe_tag{};

    /** @brief Тип-тэг, обозначающий, что используется конструкция, в которой
    предусловие должно быть проверено самой библиотекой. Является антонимом к
    @c unsafe_tag. Предоставляется для облегчения обобщённого программирования,
    в частности, чтобы можно было менять поведение операций за счёт typedef
    */
    struct safe_tag{};

    /** @brief Тип-тэг, обозначающий, что остальные аргументы должны
    использоваться как аргументы конструктора для создания некоторого объекта.
    */
    class in_place_t{};

    constexpr in_place_t inplace{};

    /** Если @c T совпадает с @c use_default, то результат @c --- Default,
    иначе --- @c T.
    @brief Класс-характеристика для определения значения шаблонного
    параметра
    @tparam T тип
    @tparam Default тип, используемый по-умолчанию
    */
    template <class T, class Default>
    struct default_helper
     : std::conditional<std::is_same<T, use_default>::value, Default, T>
    {};

    /** @brief Шаблонный синоним для @c default_helper
    @tparam T тип
    @tparam D тип, который нужно использовать, если @c T совпадает с
    @c use_default
    */
    template <class T, class D>
    using DefaultedType = typename default_helper<T, D>::type;

    /** @brief Вспомогательный класс, которому можно присвоить значение
    заданного типа
    @tparam T тип присваемоего значения
    */
    template <class T>
    struct value_consumer
    {
        /// @brief Оператор присваивания
        void operator=(T const &)
        {}
    };

    namespace
    {
        /** @brief Шаблон переменной для определения глобальных констант,
        которые не нарушают "Правило одного определения" (ODR).
        */
        template <class T>
        constexpr auto const && odr_const = T{};
    }
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
