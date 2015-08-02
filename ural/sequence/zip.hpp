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

namespace ural
{
    /** @brief Последовательность кортежей значений базовых последовательностей
    @tparam Inputs базовые последовательности
    */
    template <class... Inputs>
    class zip_sequence
     : public sequence_base<zip_sequence<Inputs...>>
    {
        typedef tuple<Inputs...> Bases_tuple;

    public:
        friend bool operator==(zip_sequence const & x, zip_sequence const & y)
        {
            return x.bases() == y.bases();
        }

        // Типы
        /// @brief Тип значения
        using value_type = tuple<ValueType<Inputs>...>;

        /// @brief Тип ссылки
        using reference = tuple<ReferenceType<Inputs>...>;

        /// @brief Тип указателя
        using pointer = void;

        /// @brief Тип расстояния
        using distance_type = CommonType<DifferenceType<Inputs>...>;

        /// @brief Категория обхода
        using traversal_tag
            = typename common_tag<typename Inputs::traversal_tag...>::type;

        // Конструкторы
        /** @brief Конструктор
        @param ins список входных последовательностей
        @post <tt> this->bases() == make_callable(ins...) </tt>
        */
        zip_sequence(Inputs... ins)
         : bases_{std::move(ins)...}
        {}

        //@{
        /** @brief Кортеж базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        tuple<Inputs...> const &
        bases() const &
        {
            return this->bases_;
        }

        tuple<Inputs...> && bases() &&
        {
            return std::move(this->bases_);
        }
        //@}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return ural::tuples::any_of(this->bases(), ural::logical_not<>{});
        }

        /** @brief Текущий элемент
        @pre <tt> !!*this </tt>
        @return Текущий элемент
        */
        reference front() const
        {
            auto f = [this](Inputs const & ... args)->reference
                     { return this->deref(args...); };

            return ::ural::apply(f, this->bases());
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !!*this </tt>
        */
        void pop_front()
        {
            ural::tuples::for_each(this->mutable_bases(), ural::pop_front);
        }

        void shrink_front();

        // Последовательность произвольного доступа
        /** @brief Количество элементов последовательности
        @return Минимальный из размеров базовых последовательностей
        */
        distance_type size() const
        {
            // @todo Обобщить, реализовать без псевдо-рекурсии
            return this->size_impl(this->bases()[ural::_1].size(), ural::_2);
        }

    private:
        Bases_tuple & mutable_bases()
        {
            return this->bases_;
        }

        reference deref(Inputs const & ... ins) const
        {
            return reference{(*ins)...};
        }

        distance_type
        size_impl(distance_type result, placeholder<sizeof...(Inputs)>) const
        {
            return result;
        }

        template <size_t index>
        distance_type
        size_impl(distance_type current, placeholder<index> p) const
        {
            return this->size_impl(std::min(current, this->bases()[p].size()),
                                   placeholder<index+1>());
        }


    private:
        Bases_tuple bases_;
    };

    /** @brief Создание последовательности соответствующих элементов кортежей
    @param ins базовые последовательности
    */
    template <class... Inputs>
    auto make_zip_sequence(Inputs && ... ins)
    -> zip_sequence<decltype(::ural::sequence_fwd<Inputs>(ins))...>
    {
        typedef zip_sequence<decltype(::ural::sequence_fwd<Inputs>(ins))...>
            Result;
        return Result(::ural::sequence_fwd<Inputs>(ins)...);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ZIP_HPP_INCLUDED
