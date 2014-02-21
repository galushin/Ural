#ifndef Z_URAL_DEFS_HPP_INCLUDED
#define Z_URAL_DEFS_HPP_INCLUDED

/** @file ural/defs.hpp
 @brief Определения основных типов, используемых библиотекой
*/

#define URAL_PREVENT_MACRO_SUBSTITUTION

namespace ural
{
    struct empty_type{};
    class null_type{};
    struct use_default{};
    struct auto_tag{};

    /** @brief Класс, содержащий @b typedef-объявление типа
    @tparam T тип
    */
    template <class T>
    struct declare_type
    {
        /// @brief Объявляемый тип
        typedef T type;
    };

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

    /** @brief Оператор "равно" для пустых типов
    @return true
    */
    template <class T>
    constexpr typename std::enable_if<std::is_empty<T>::value, bool>::type
    operator==(T const &, T const &)
    {
        return true;
    }

    /** @brief Естественное определение оператора "не равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x == y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator!=(T1 const & x, T2 const & y)
    {
        return !(x == y);
    }

    /** @brief Естественное определение оператора "больше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> y < x </tt>
    */
    template <class T1, class T2>
    constexpr bool operator>(T1 const & x, T2 const & y)
    {
        return y < x;
    }

    /** @brief Естественное определение оператора "меньше либо равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x > y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator<=(T1 const & x, T2 const & y)
    {
        return !(x > y);
    }

    /** @brief Естественное определение оператора "больше либо равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x < y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator>=(T1 const & x, T2 const & y)
    {
        return !(x < y);
    }

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
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
