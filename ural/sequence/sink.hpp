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
    /** @brief Шаблон класса функционального объекта, который принимает один
    аргумент и ничего не делает.
    @tparam T тип аргумента. Если этот тип совпадает с @c auto_tag, то
    приниматься будет любой тип.
    */
    template <class T = auto_tag>
    class sink_functor
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef void result_type;

        //@{
        /// @brief Оператор применения функционального объекта
        constexpr void
        operator()(typename boost::call_traits<T>::param_type &) const
        {}

        template <class Arg>
        typename std::enable_if<!std::is_same<Arg, T>::value && std::is_same<T, auto_tag>::value>::type
        operator()(Arg const & arg) const
        {}
        //@}
    };

    /** @brief Последовательность вывода, которая просто игнорирует передаваемое
    значение.
    @tparam T тип элементов, которые можно помещать в последовательность. Если
    этот тип совпадает с @c auto_tag, то приниматься будет любой тип.
    */
    template <class T = auto_tag>
    using sink_sequence = ural::function_output_sequence<sink_functor<T>>;
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SINK_HPP_INCLUDED
