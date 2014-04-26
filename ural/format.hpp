#ifndef Z_URAL_FORMAT_HPP_INCLUDED
#define Z_URAL_FORMAT_HPP_INCLUDED

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

/** @file ural/format.hpp
 @brief Средства форматированного ввода/вывода
 @todo Оптимизация
*/

#include <ural/sequence/make.hpp>
#include <ural/sequence/by_line.hpp>

#include <vector>

namespace ural
{
    template <class OStream, class Table>
    OStream & write_table(OStream & os, Table const & table)
    {
        for(auto & row : table)
        {
            auto s = ural::sequence(row);

            if(!!s)
            {
                os << *s;
                ++ s;

                for(; !!s; ++ s)
                {
                    os << "\t" << *s;
                }
            }

            os << "\n";
        }
        return os;
    }

    template <class T, class IStream>
    std::vector<std::vector<T>>
    read_table(IStream & is)
    {
        std::vector<std::vector<T>> result;

        auto seq = ural::by_line(is, '\n');

        for(; !!seq; ++ seq)
        {
            typedef typename IStream::char_type char_type;
            typedef typename IStream::traits_type traits_type;
            std::basic_string<char_type, traits_type> const & str = *seq;
            std::basic_istringstream<char_type, traits_type> str_is(str);

            std::vector<T> row;
            T cell;

            while(str_is >> std::ws >> cell >> std::ws)
            {
                row.push_back(std::move(cell));
            }

            result.push_back(std::move(row));
        }

        return result;
    }
}
// namespace ural

#endif
// Z_URAL_FORMAT_HPP_INCLUDED
