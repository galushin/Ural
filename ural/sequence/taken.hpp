#ifndef Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
#define Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED

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

/** @file ural/sequence/taken.hpp
 @brief Адаптер последовательности, ограничивающий базовую последовательность
 заданным числом элементов
*/

#include <ural/utility.hpp>

namespace ural
{
    /** @brief Адаптор последовательности, ограничивающий длину базовой
    последовательности
    @tparam Sequence тип последовательности
    @tparam Size тип количества элементов, которые должны быть взяты из базовой
    последовательности
    */
    template <class Sequence, class Size>
    class take_sequence
     : public sequence_base<take_sequence<Sequence, Size>>
    {
    public:
        // Типы
        typedef typename Sequence::reference reference;
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::distance_type distance_type;
        typedef typename Sequence::traversal_tag traversal_tag;

        // Создание, копирование
        explicit take_sequence(Sequence seq, Size count)
         : impl_(std::move(seq), Count_type{std::move(count)})
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base() || this->count() == 0;
        }

        reference front() const
        {
            return *this->base();
        }

        void pop_front()
        {
            assert(this->count() > 0);

            ++ impl_[ural::_1];
            -- ural::get(impl_[ural::_2]);
        }

        // Прямая последовательность
        take_sequence traversed_front() const
        {
            return take_sequence(this->base().traversed_front(),
                                 this->init_count() - this->count());
        }

        // Последовательность производного доступа

        // Адаптор последовательности
        Sequence const & base() const
        {
            return impl_[ural::_1];
        }

        Size const & count() const
        {
            return ural::get(impl_[ural::_2]);
        }

    private:
        Size const & init_count() const
        {
            return impl_[ural::_2].old_value();
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        typedef typename std::conditional<is_forward, with_old_value<Size>, Size>::type
            Count_type;

        ural::tuple<Sequence, Count_type> impl_;
    };

    template <class Size>
    struct taken_helper
    {
        Size count;
    };

    template <class Sequence, class Size>
    auto operator|(Sequence && seq, taken_helper<Size> helper)
    -> take_sequence<decltype(sequence(std::forward<Sequence>(seq))), Size>
    {
        typedef take_sequence<decltype(sequence(std::forward<Sequence>(seq))), Size> Result;
        return Result{sequence(std::forward<Sequence>(seq)), helper.count};
    }

    template <class Size>
    taken_helper<Size> taken(Size n)
    {
        return {n};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_TAKEN_HPP_INCLUDED
