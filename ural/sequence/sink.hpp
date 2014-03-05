#ifndef Z_URAL_SEQUENCE_SINK_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SINK_HPP_INCLUDED

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

/** @file ural/sequence/sink.hpp
 @brief Последовательность вывода, которая просто игнорирует передаваемое
 значение.
*/

namespace ural
{
    template <class T = auto_tag>
    class sink_functor
    {
    public:
        void operator()(typename boost::call_traits<T>::param_type &) const
        {}
    };

    template <>
    class sink_functor<auto_tag>
    {
    public:
        template <class T>
        void operator()(T &&) const
        {}
    };

    template <class T = auto_tag>
    using sink_sequence = ural::function_output_sequence<sink_functor<T>>;
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SINK_HPP_INCLUDED
