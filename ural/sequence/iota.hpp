#ifndef Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED
#define Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED

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

/** @file ural/sequence/iota.hpp
 @brief Адаптор последовательности, возвращающий в качестве значения исходную
 последовательность
*/

namespace ural
{
    template <class Incrementable>
    class iota_sequence
     : public sequence_base<iota_sequence<Incrementable>>
    {
    public:
        typedef typename Incrementable::traversal_tag traversal_tag;
        typedef Incrementable value_type;
        typedef value_type const & reference;

        explicit iota_sequence(Incrementable s)
         : base_{std::move(s)}
        {}

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
        Incrementable base_;
    };

    template <class Incrementable>
    iota_sequence<Incrementable>
    make_iota_sequence(Incrementable x)
    {
        return iota_sequence<Incrementable>{std::move(x)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED
