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

#include <ural/format/stream_traits.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/to.hpp>
#include <ural/sequence/iostream.hpp>
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
    @todo Поддержка временных объектов
    */
    template <class OStream, class Sequence, class Delim>
    OStream &
    write_delimeted(OStream & os, Sequence && seq, Delim const & delim)
    {
        auto s = ::ural::sequence_fwd<Sequence>(seq);

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

    template <class Sequence, class Delimeter>
    class delimeted_helper
    {
    public:
        delimeted_helper(Sequence seq, Delimeter delim)
         : sequence(std::move(seq))
         , delimeter(std::move(delim))
        {}

        Sequence sequence;
        Delimeter delimeter;
    };

    template <class OStream, class Seq, class Delim>
    OStream & operator<<(OStream & os, delimeted_helper<Seq, Delim> && d)
    {
        return ural::write_delimeted(os, std::move(d.sequence),
                                     std::move(d.delimeter));
    }

    template <class Sequence, class Delimeter>
    auto delimeted(Sequence && seq, Delimeter delim)
    -> delimeted_helper<decltype(::ural::sequence_fwd<Sequence>(seq)), Delimeter>
    {
        return {::ural::sequence_fwd<Sequence>(seq), std::move(delim)};
    }

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
            assert(!!is);
            return reader;
        }
    };

    /** @brief Специализация для преобразования строки в саму себя
    @tparam String тип строки
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
    @todo Можно ли возвращать по константной ссылке, когда T == String
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
    @todo Параметризация разделителей
    */
    template <class T, class IStream>
    std::vector<std::vector<T>>
    read_table(IStream && is)
    {
        typedef typename std::remove_reference<IStream>::type input_stream_type;

        // @todo Проверка концепций
        std::vector<std::vector<T>> result;

        for(auto seq = ural::by_line(is, '\n'); !!seq && !seq.front().empty(); ++ seq)
        {
            typedef typename stream_traits<input_stream_type>::string_type
                String;

            ural::basic_istringstream<String> str_is(*seq);
            str_is >> std::ws;

            from_string_policy<String, T> constexpr converter{};

            auto row = ural::by_line(str_is, '\t')
                     | ural::transformed(converter)
                     | ural::to_container<std::vector>{};

            result.push_back(std::move(row));
        }

        return result;
    }

namespace details
{
    template <class OStream, class T>
    decltype(std::declval<OStream>().str())
    to_string_impl(T const & x)
    {
        OStream os;
        os << x;
        return os.str();
    }
}
//namespace details

    /** @brief Функциональный объект преобразования в строку
    @tparam Char тип символов
    @tparam Traits класс характеристик символов
    @todo Оптимизированные перегрузки для отдельных типов, например, для
    целых чисел
    */
    template <class Char, class Traits = std::char_traits<Char>>
    class to_string_function
    {
    public:
        /** @brief Выполнение преобразования в строку
        @param x преобразуемое значение
        @return Строка, содержащая представление @c x
        */
        template <class T>
        std::basic_string<Char, Traits>
        operator()(T const & x) const
        {
            std::basic_ostringstream<Char, Traits> os;
            os << x;
            return os.str();
        }
    };

    /// @brief Функциональный объект преобразования в <tt> std::string </tt>
    auto constexpr to_string = to_string_function<char>{};

    /// @brief Функциональный объект преобразования в <tt> std::wstring </tt>s
    auto constexpr to_wstring = to_string_function<wchar_t>{};
}
// namespace ural

#endif
// Z_URAL_FORMAT_HPP_INCLUDED
