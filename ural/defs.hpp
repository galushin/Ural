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

#include <ural/type_traits.hpp>

/// @brief Макрос для предотвращения макро-подстановки при объявлении функции
#define URAL_PREVENT_MACRO_SUBSTITUTION

namespace ural
{
    /** Пустой базовый класс. Используется, когда формально требуется указать
    базовый класс, но никакой базовый класс на самом деле не нужен.
    @brief Пустой базовый класс.
    */
    struct empty_type{};

    /** Класс-тэг обозначающий отсутствие типа. Используется, например, как
    пустой список типов
    @brief Класс-тэг обозначающий отсутствие типа. Исполь
    */
    class null_type{};

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

    /** @brief Тип функционального объекта для функции @c empty и аналогичной
    функциональности
    @todo Найти лучшее место для empty_fn и empty
    */
    class empty_fn
    {
    private:
        template <class Container>
        static bool empty_impl(Container const & x,
                               declare_type<decltype(x.empty())> *)
        {
            return x.empty();
        }

        template <class Container>
        static bool empty_impl(Container const & x, ...)
        {
            return x.size() == 0;
        }

        template <class T, std::size_t N>
        static bool empty_impl(T (&)[N], std::nullptr_t )
        {
            return N == 0;
        }

    public:
        /** @brief Оператор вызова функции
        @param x контейнер @c STL, "почти контейнер" или встроенный массив
        @return Если @c x имеет функцию-член @c empty, то возвращает
        <tt> x.empty </tt>, если @c x -- встроенный C-массив известного размера
        @c N то возвращает @c N, в остальных случаях возвращает
        <tt> x.size() == 0 </tt>
        */
        template <class Container>
        bool operator()(Container const & x) const
        {
            return this->empty_impl(x, nullptr);
        }
    };

    auto constexpr empty = empty_fn{};
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
