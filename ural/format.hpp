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
 @todo os << delimeted(seq, ", ") << "\n";
*/

#include <ural/algorithm.hpp>
#include <ural/sequence/to.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/by_line.hpp>

#include <cassert>
#include <vector>
#include <sstream>
#include <sstream>

namespace ural
{
    /** @brief Вывод в поток элементов последовательности, разделённых заданным
    объектом.
    @param os поток вывода
    @param seq последовательность
    @param delim разделител
    @return @c os
    @todo Унифицировать с @c describe
    */
    template <class OStream, class Sequence, class Delim>
    OStream &
    write_delimeted(OStream & os, Sequence && seq, Delim const & delim)
    {
        auto s = sequence(std::forward<Sequence>(seq));

        if(!s)
        {
            return os;
        }

        os << *s;
        ++ s;

        for(; !!s; ++ s)
        {
            os << delim << *s;
        }

        return os;
    }

    /** @brief Класс-характеристика для потоков ввода и вывода
    @tparam Stream поток ввода или вывода
    */
    template <class Stream>
    struct stream_traits
    {
        /// @brief Тип символов
        typedef typename Stream::char_type char_type;

        /// @brief Класс-характеристика
        typedef typename Stream::traits_type traits_type;

        /// @brief Соответствующий класс строк
        typedef typename std::basic_string<char_type, traits_type> string_type;
    };

    /** @brief Вывод таблицы в поток
    @param os поток вывода
    @tparam table таблица
    */
    template <class OStream, class Table>
    OStream & write_table(OStream && os, Table const & table)
    {
        for(auto & row : table)
        {
            ural::write_delimeted(os, row, "\t") << "\n";
        }
        return os;
    }

    template <class String>
    class basic_istringstream
     : public std::basic_istringstream<typename String::value_type,
                                       typename String::traits_type>
    {
        typedef std::basic_istringstream<typename String::value_type,
                                         typename String::traits_type> Base;

    public:
        basic_istringstream(String const & s)
         : Base(s)
        {}

        basic_istringstream(String && s);
    };

    /** @brief Функциональный объект, преобразующий из строки в заданный тип
    @tparam String тип строки
    @tparam T тип, в который осуществляется преобразование.
    */
    template <class String, class T>
    class from_string_policy
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef T result_type;

        /** @brief Преобразование
        @param s преобразуемая строка
        @return Значение, записанное в строке @c s.
        */
        result_type operator()(String const & s) const
        {
            ural::basic_istringstream<String> is(s);
            T reader;
            is >> reader;
            assert(is.eof());
            return reader;
        }
    };

    /** @brief Специализация для преобразования строки в саму себя
    @tparam String тип строки
    @todo Можно ли возвращать по константной ссылке
    */
    template <class String>
    class from_string_policy<String, String>
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef String result_type;

        /** @brief Преобразование
        @param s преобразуемая строка
        @return @c s
        */
        result_type operator()(String const & s) const
        {
            return s;
        }
    };

    /** @brief Преобразования из строки в значение
    @tparam T тип, в который осуществляется преобразование.
    @tparam String тип строки
    @param s строка
    @return from_string_policy<Ch, Tr, T>{}(s)
    */
    template <class T, class String>
    T from_string(String const & s)
    {
        return from_string_policy<String, T>{}(s);
    }

    /** @brief Ввод однородной таблицы (матрицы) из потока
    @tparam T тип элементов таблицы
    @param is входной поток
    @return считанную таблицу
    */
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
            typedef typename stream_traits<input_stream_type>::string_type
                String;

            String const & str = *seq;
            ural::basic_istringstream<String> str_is(str);
            str_is >> std::ws;

            auto cell_seq = ural::by_line(str_is, '\t');

            from_string_policy<String, T> constexpr converter{};

            auto row = cell_seq | ural::transformed(converter)
                       | ural::to_container<std::vector>{};

            result.push_back(std::move(row));
        }

        return result;
    }
}
// namespace ural

#endif
// Z_URAL_FORMAT_HPP_INCLUDED
