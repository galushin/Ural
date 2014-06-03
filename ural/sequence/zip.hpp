#ifndef Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED

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

/** @file ural/sequence/zip.hpp
 @brief Последовательность кортежей соответствующих элементов
 последовательностей
*/

#include <ural/tuple.hpp>
#include <ural/sequence/transform.hpp>

namespace ural
{
    /** @brief Функциональный объект для создания кортежей из пачки аргументов
    */
    class make_tuple_functor
    {
    public:
        template <class... Args>
        constexpr auto operator()(Args &&... args) const
        -> decltype(std::make_tuple(args...))
        {
            return std::make_tuple(args...);
        }
    };

    template <class... Inputs>
    using zip_sequence = transform_sequence<make_tuple_functor, Inputs...>;

    /** @brief Создание последовательности соответствующих элементов кортежей
    @param ins базовые последовательности
    */
    template <class... Inputs>
    auto make_zip_sequence(Inputs && ... ins)
    -> zip_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
    {
        typedef zip_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
            Result;
        return Result(make_tuple_functor{},
                      sequence(std::forward<Inputs>(ins))...);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
