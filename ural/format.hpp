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
 @todo istringstream для произвольных строк, не обязательно basic_string<>
*/

#include <ural/algorithm.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/by_line.hpp>

#include <cassert>
#include <vector>
#include <sstream>
#include <sstream>

namespace ural
{
    template <class OStream, class Table>
    OStream & write_table(OStream && os, Table const & table)
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

    template <class Ch, class Tr, class T>
    class from_string_policy
    {
    public:
        typedef T result_type;
        result_type operator()(std::basic_string<Ch, Tr> const & s) const
        {
            std::basic_istringstream<Ch, Tr> is(s);
            T reader;
            is >> reader;
            assert(is.eof());
            return reader;
        }
    };

    template <class Ch, class Tr>
    class from_string_policy<Ch, Tr, std::basic_string<Ch, Tr>>
    {
    public:
        typedef std::basic_string<Ch, Tr> result_type;

        result_type operator()(std::basic_string<Ch, Tr> const & s) const
        {
            return s;
        }
    };

    template <class T, class Ch, class Tr>
    T from_string(std::basic_string<Ch, Tr> const & s)
    {
        return from_string_policy<Ch, Tr, T>{}(s);
    }

    template <class T, class IStream>
    std::vector<std::vector<T>>
    read_table(IStream && is)
    {
        typedef typename std::remove_reference<IStream>::type input_stream_type;

        // @todo Проверка концепций
        std::vector<std::vector<T>> result;

        auto seq = ural::by_line(is, '\n');

        for(; !!seq; ++ seq)
        {
            typedef typename input_stream_type::char_type char_type;
            typedef typename input_stream_type::traits_type traits_type;
            std::basic_string<char_type, traits_type> const & str = *seq;
            std::basic_istringstream<char_type, traits_type> str_is(str);
            str_is >> std::ws;

            auto cell_seq = ural::by_line(str_is, '\t');

            std::vector<T> row;

            from_string_policy<char_type, traits_type, T> constexpr converter{};

            ural::copy(ural::make_transform_sequence(converter, cell_seq),
                       row | ural::back_inserter);

            result.push_back(std::move(row));
        }

        return result;
    }
}
// namespace ural

#endif
// Z_URAL_FORMAT_HPP_INCLUDED
