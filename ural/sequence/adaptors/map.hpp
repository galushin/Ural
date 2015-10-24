#ifndef Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MAP_HPP_INCLUDED

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

/** @file ural/sequence/map.hpp
 @brief Адапторы последовательностей для ассоциативных контейнеров
*/

#include <ural/sequence/adaptors/transform.hpp>
#include <ural/utility.hpp>
#include <utility>

namespace ural
{
    /** @brief Адаптор последовательности
    @tparam Sequence базовая последовательность, элементы которой являются
    кортеже-подобными.
    @tparam Index индекс элемента значений базовой последовательности
    */
    template <class Sequence, size_t Index>
    class elemenents_sequence
     : public sequence_adaptor<elemenents_sequence<Sequence, Index>,
                               transform_sequence<tuple_get<Index>, Sequence>>
    {
        using Base = sequence_adaptor<elemenents_sequence<Sequence, Index>,
                               transform_sequence<tuple_get<Index>, Sequence>>;
    public:
        /** @brief Конструктор
        @param seq базовая последовательность
        @post <tt> this->base() == seq </tt>
        */
        explicit elemenents_sequence(Sequence seq)
         : Base(ural::make_transform_sequence(tuple_get<Index>{}, std::move(seq)))
        {}

        //@{
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Sequence const & base() const &
        {
            return Base::base().bases()[ural::_1];
        }

        Sequence && base() &&
        {
            return static_cast<Base &&>(*this).base().bases()[ural::_1];
        }
        //@}

    private:
        friend Base;

        using Impl_sequence = transform_sequence<tuple_get<Index>, Sequence>;

        elemenents_sequence rebind_base(Impl_sequence seq) const
        {
            return elemenents_sequence(std::move(seq).bases()[ural::_1]);
        }
    };
    /** @brief Тип функционального объекта для создания последовательностей
    ключей и отображаемых значений ассоциативного контейнера.
    @tparam Index индекс последовательности: 0 --- ключи, 1 --- отображаемые
    значения
    */
    template <size_t Index>
    struct elements_sequence_fn
    {
        /** @brief Создание последовательностей ключей или отображаемых значений
        отображений ассоциативного контейнера.
        @param seq (под)последовательность элементов ассоциативного контейнера
        */
        template <class Sequenced>
        auto operator()(Sequenced && seq) const
        {
            using Result = elemenents_sequence<SequenceType<Sequenced>, Index>;

            return Result(::ural::sequence_fwd<Sequenced>(seq));
        }
    };

    namespace
    {
        /** @brief Объект-тэг для создания последовательности ключей ассоциативного
        контейнера */
        constexpr auto const & map_keys
            = odr_const<pipeable<elements_sequence_fn<0>>>;

        /** @brief Объект-тэг для создания последовательности отоброжаемых значений
        ассоциативного контейнера */
        constexpr auto const & map_values
            = odr_const<pipeable<elements_sequence_fn<1>>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
