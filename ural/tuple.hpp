#ifndef Z_URAL_TUPLE_HPP_INCLUDED
#define Z_URAL_TUPLE_HPP_INCLUDED

/** @file ural/tuple.hpp
 @brief Кортежи --- гетерогенный контейнер фиксированного на этапе компиляции
 размера.
*/

#include <tuple>

#include <ural/placeholders.hpp>

namespace ural
{
    /** @brief Кортеж
    @tparam Ts типы элементов кортежа
    */
    template <class... Ts>
    class tuple
     : public std::tuple<Ts...>
    {
        typedef std::tuple<Ts...> Base;
    public:
        /** Инициализирует каждый элемент с помощью соответствующих
        конструкторов без аргументов.
        @brief Конструктор без аргументов
        */
        constexpr tuple()
         : Base{}
        {}

        /** Инициализирует каждый элемент с помощью конструктора копирования
        на основе соответсвующего аргумента из @c args
        @brief Конструктор
        @param args аргументы
        */
        constexpr explicit tuple(Ts const & ... args)
         : Base(args...)
        {}

        //@{
        /** @brief Доступ к элементам по "статическому индексу"
        @tparam Index номер элемента
        @return Ссылка на элемент с номером @c Index
        */
        template <size_t Index>
        constexpr typename std::tuple_element<Index, Base>::type const &
        operator[](ural::placeholder<Index>) const
        {
            return std::get<Index>(*this);
        }

        template <size_t Index>
        typename std::tuple_element<Index, Base>::type &
        operator[](ural::placeholder<Index>)
        {
            return std::get<Index>(*this);
        }
        //@}
    };
}
// namespace ural

#endif
// Z_URAL_TUPLE_HPP_INCLUDED
