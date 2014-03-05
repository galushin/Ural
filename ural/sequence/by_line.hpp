#ifndef Z_URAL_SEQUENCE_BY_LINE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BY_LINE_HPP_INCLUDED

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

/** @file ural/sequence/by_line.hpp
 @brief Последовательность строк потока ввода
*/

#include <ural/sequence/base.hpp>

#include <string>

namespace ural
{
    enum class keep_delimeter
    {
        no = 0,
        yes = 1
    };

    template <class IStream>
    class by_line_sequence
     : public sequence_base<by_line_sequence<IStream>>
    {
        typedef typename IStream::char_type char_type;
    public:
        // Типы
        typedef single_pass_traversal_tag traversal_tag;
        typedef std::basic_string<char_type> value_type;
        typedef value_type const & reference;

        // Конструкторы
        explicit by_line_sequence(IStream & is,
                                  char_type delimeter = char_type('\n'),
                                  keep_delimeter kd = keep_delimeter::no)
         : is_{is}
         , delim_(std::move(delimeter))
         , kd_{kd}
        {
            this->seek();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !is_.get();
        }

        reference front() const
        {
            return reader_;
        }

        void pop_front()
        {
            this->seek();
        }

    private:
        void seek()
        {
            getline(is_.get(), reader_, delim_);

            if(kd_ == keep_delimeter::yes && is_.get().eof() == false)
            {
                reader_.push_back(delim_);
            }
        }

    private:
        std::reference_wrapper<IStream> is_;
        value_type reader_;
        char_type delim_;
        keep_delimeter kd_;
    };

    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream & is)
    {
        return by_line_sequence<IStream>(is);
    }

    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream & is, typename IStream::char_type delimeter)
    {
        return by_line_sequence<IStream>(is, std::move(delimeter));
    }

    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream & is, typename IStream::char_type delimeter,
            keep_delimeter kd)
    {
        return by_line_sequence<IStream>(is, std::move(delimeter), kd);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BY_LINE_HPP_INCLUDED
