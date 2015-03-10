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
    /** @brief Последовательность кортежей значений базовых последовательностей
    @tparam Inputs базовые последовательности
    */
    template <class... Inputs>
    class zip_sequence
     : public sequence_base<zip_sequence<Inputs...>>
    {
        typedef transform_sequence<make_tuple_functor, Inputs...> Impl_seq;
    public:
        // Типы
        /// @brief Тип значения
        typedef typename Impl_seq::value_type value_type;

        /// @brief Тип ссылки
        typedef typename Impl_seq::reference reference;

        /// @brief Тип указателя
        typedef typename Impl_seq::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Impl_seq::distance_type distance_type;

        /// @brief Категория обхода
        typedef typename Impl_seq::traversal_tag traversal_tag;

        // Конструкторы
        /** @brief Конструктор
        @param ins список входных последовательностей
        @post <tt> this->bases() == make_functor(ins...) </tt>
        */
        zip_sequence(Inputs... ins)
         : impl_{make_tuple_functor{}, std::move(ins)...}
        {}

        /** @brief Кортеж базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        tuple<Inputs...> const &
        bases() const
        {
            return this->impl_.bases();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return !impl_;
        }

        /** @brief Текущий элемент
        @pre <tt> !!*this </tt>
        @return Текущий элемент
        */
        reference front() const
        {
            return impl_.front();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !!*this </tt>
        */
        void pop_front()
        {
            impl_.pop_front();
        }

    private:
        Impl_seq impl_;
    };

    /** @brief Создание последовательности соответствующих элементов кортежей
    @param ins базовые последовательности
    */
    template <class... Inputs>
    auto make_zip_sequence(Inputs && ... ins)
    -> zip_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
    {
        typedef zip_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
            Result;
        return Result(sequence(std::forward<Inputs>(ins))...);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
