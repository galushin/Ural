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

namespace ural
{
    /** @note Длина последовательности меньше заданной --- это ошибка или
    нормальная ситуация?
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
         : impl_{std::move(seq), std::move(count)}
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
            // @todo Проверить, что счётчик не исчерпан
            ++ impl_.first();
            -- impl_.second();
        }

        // Последовательность производного доступа

        // Адаптор последовательности
        Sequence const & base() const
        {
            return impl_.first();
        }

        Size const & count() const
        {
            return impl_.second();
        }

    private:
        boost::compressed_pair<Sequence, Size> impl_;
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
