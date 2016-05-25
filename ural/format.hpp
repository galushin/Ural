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
#include <ural/format/stream_traits.hpp>
#include <ural/sequence/adaptors/transform.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/by_line.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/sequence/iostream.hpp>
#include <ural/sequence/to.hpp>

#include <cassert>
#include <vector>
#include <sstream>
#include <sstream>

namespace ural
{
inline namespace v0
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
    write_separated(OStream & os, Sequence && seq, Delim const & delim)
    {
        auto s = ural::cursor_fwd<Sequence>(seq);

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

    template <class Cursor, class Separator>
    class separated_helper
    {
    public:
        /** @brief Конструктор
        @param seq Курсор
        @param delim разделитель
        @post <tt> this->cursor == seq </tt>
        @post <tt> this->separator == delim </tt>
        */
        separated_helper(Cursor cur, Separator delim)
         : cursor(std::move(cur))
         , separator(std::move(delim))
        {}

        /// @brief Последовательность
        Cursor cursor;

        /// @brief Разделитель
        Separator separator;
    };

    template <class OStream, class Seq, class Separator>
    OStream & operator<<(OStream & os, separated_helper<Seq, Separator> && d)
    {
        return ural::write_separated(os, std::move(d.cursor),
                                     std::move(d.separator));
    }

    template <class Sequence, class Separator>
    auto separated(Sequence && seq, Separator separator)
    -> separated_helper<decltype(::ural::cursor_fwd<Sequence>(seq)), Separator>
    {
        return {::ural::cursor_fwd<Sequence>(seq), std::move(separator)};
    }

    /** @brief Вывод таблицы в поток
    @param os поток вывода
    @param table таблица
    @return <tt> std::forward<OStream>(os) </tt>
    */
    template <class OStream, class Table>
    OStream write_table(OStream && os, Table const & table)
    {
        for(auto & row : table)
        {
            ural::write_separated(os, row, "\t") << "\n";
        }

        return std::forward<OStream>(os);
    }

    template <class String>
    class basic_istringstream
     : public std::basic_istringstream<ValueType<String>,
                                       typename String::traits_type>
    {
        typedef std::basic_istringstream<ValueType<String>,
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

        for(auto seq = ::ural::experimental::by_line(is, '\n'); !!seq && !seq.front().empty(); ++ seq)
        {
            typedef typename stream_traits<input_stream_type>::string_type
                String;

            ural::basic_istringstream<String> str_is(*seq);
            str_is >> std::ws;

            from_string_policy<String, T> constexpr converter{};

            auto row = ::ural::experimental::by_line(str_is, '\t')
                     | ::ural::experimental::transformed(converter)
                     | ::ural::experimental::to_container<std::vector>{};

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

    namespace
    {
        /// @brief Функциональный объект преобразования в <tt> std::string </tt>
        constexpr auto & to_string = odr_const<to_string_function<char>>;

        /// @brief Функциональный объект преобразования в <tt> std::wstring </tt>s
        constexpr auto & to_wstring = odr_const<to_string_function<wchar_t>>;
    }
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_FORMAT_HPP_INCLUDED
