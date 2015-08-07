#ifndef Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED
#define Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED

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

/** @file ural/sequence/simo.hpp
 @brief Адаптор, производящий вывод сразу в несколько последовательностей вывода
*/

#include <ural/sequence/make.hpp>

namespace ural
{
    /** @ingroup Sequences
    @brief Адаптор, производящий вывод сразу в несколько последовательностей
    вывода
    @todo устранить дублирование с zip_sequence
    */
    template <class... Outputs>
    class simo_sequence_t
     : public sequence_base<simo_sequence_t<Outputs...>>
    {
        typedef tuple<Outputs...> Bases_tuple;
    public:
        // Типы
        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип расстояния
        typedef CommonType<difference_type<Outputs>...> distance_type;

        /// @brief Тип ссылки
        typedef simo_sequence_t & reference;

        // Конструкторы, присваивание
        /** @brief Конструктор
        @param outs базовые последовательности
        @post <tt> this->bases() == make_tuple(std::move(outs)...) </tt>
        */
        simo_sequence_t(Outputs... outs)
         : bases_(std::move(outs)...)
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return ural::tuples::any_of(this->bases(), ural::logical_not<>{});
        }

        /** @brief Передний элемент
        @return Ссылка на первый элемент последовательности
        @pre <tt> !*this == true </tt>
        */
        reference operator*()
        {
            return *this;
        }

        /** @brief Вывод значения в последовательность
        @param value выводимое значение
        @return *this
        */
        template <class T>
        void operator=(T const & value)
        {
            ural::tuples::for_each(this->mutable_bases(),
                                   [&value](auto & s) { *s = value; });
        }

        /** @brief Переход к следующему элементу в передней части
        @pre <tt> !*this == true </tt>
        */
        void pop_front()
        {
            assert(!!*this);

            ural::tuples::for_each(this->mutable_bases(), ural::pop_front);
        }

        // Адаптор последовательности
        //@{
        /** @brief Доступ к кортежу базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        Bases_tuple const & bases() const &
        {
            return this->bases_;
        }

        Bases_tuple && bases() &&
        {
            return this->bases_;
        }
        //@}

    private:
        Bases_tuple & mutable_bases()
        {
            return this->bases_;
        }

        Bases_tuple bases_;
    };

    /** @brief Функция создания адаптора, производящего вывод сразу в несколько
    последовательностей вывода
    @param outs выходные последовательноти
    @return <tt> S(ural::sequence_fwd<Outputs>(outs)...) </tt>, где
    @c S --- <tt> simo_sequence_t<SequenceType<Outputs>...> </tt>.
    */
    template <class... Outputs>
    auto simo_sequence(Outputs && ... outs)
    -> simo_sequence_t<SequenceType<Outputs>...>
    {
        typedef simo_sequence_t<SequenceType<Outputs>...> Result;
        return Result(ural::sequence_fwd<Outputs>(outs)...);
    }
}
// namespace ural

#endif
// Z_URAL_SIMO_SEQUENCE_HPP_INCLUDED
