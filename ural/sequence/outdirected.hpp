#ifndef Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED

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

/** @file ural/sequence/outdirected.hpp
 @brief Адаптор последовательности, возвращающий в качестве значения исходную
 последовательность
*/

namespace ural
{
    /** @brief Адаптор последовательности, возвращающий в качестве значения
    исходную последовательность
    @tparam Sequence последовательность
    */
    template <class Sequence>
    class outdirected_sequence
     : public sequence_base<outdirected_sequence<Sequence>>
    {
    public:
        // Типы
        typedef typename Sequence::traversal_tag traversal_tag;
        typedef Sequence value_type;
        typedef value_type const & reference;

        // Конструктор
        explicit outdirected_sequence(Sequence s)
         : base_{std::move(s)}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            return this->base();
        }

        void pop_front()
        {
            ++ base_;
        }

        // Адаптор последовательности
        reference base() const
        {
            return this->base_;
        }

    private:
        Sequence base_;
    };

    template <class Sequence>
    outdirected_sequence<Sequence>
    make_outdirected_sequence(Sequence x)
    {
        return outdirected_sequence<Sequence>{std::move(x)};
    }

    struct outdirected_helper {};
    auto constexpr outdirected = outdirected_helper{};

    template <class Sequence>
    outdirected_sequence<Sequence>
    operator|(Sequence seq, outdirected_helper)
    {
        return make_outdirected_sequence(std::move(seq));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_OUTDIRECTED_HPP_INCLUDED
