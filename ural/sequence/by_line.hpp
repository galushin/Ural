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

#include <ural/format/stream_traits.hpp>
#include <ural/sequence/base.hpp>

#include <string>

namespace ural
{
    /// @brief Перечисление "Сохранять ли разделитель в конце строки)
    enum class keep_delimiter
    {
        no = 0,
        yes = 1
    };

    /** @brief Последовательность строк потока ввода
    @tparam IStream поток ввода
    */
    template <class IStream>
    class by_line_sequence
     : public sequence_base<by_line_sequence<IStream>>
    {
    public:
        // Типы
        /// @brief Тип потока ввода
        typedef typename std::remove_reference<IStream>::type stream_type;

        /// @brief Тип символа
        typedef typename stream_traits<stream_type>::char_type char_type;

        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип значения
        typedef typename stream_traits<stream_type>::string_type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Тип расстояния
        typedef typename stream_type::off_type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param is поток ввода
        @param delimiter символ-разделитель
        @param kd флаг, показывающий, нужно ли сохранять символ-разделитель в
        конце строки.
        */
        explicit by_line_sequence(IStream && is,
                                  char_type delimiter = char_type('\n'),
                                  keep_delimiter kd = keep_delimiter::no)
         : is_{std::forward<IStream>(is)}
         , delim_(std::move(delimiter))
         , kd_{kd}
        {
            this->seek();
        }

        by_line_sequence(by_line_sequence const &) = default;
        by_line_sequence(by_line_sequence &&) = default;

        // Однопроходная последовательность
        /** @brief Провекра исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->stream();
        }

        /** @brief Текущий элемент
        @return Ссылка на текущую строку
        @pre <tt> !*this == false </tt>
        */
        reference front() const
        {
            return reader_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            this->seek();
        }

    private:
        void seek()
        {
            using std::getline;
            getline(this->stream_ref(), reader_, delim_);

            if(kd_ == keep_delimiter::yes && this->stream_ref().eof() == false)
            {
                reader_.push_back(delim_);
            }
        }

        IStream & stream_ref()
        {
            return this->is_;
        }

        IStream const & stream() const
        {
            return this->is_;
        }

    private:
        typedef typename std::conditional<std::is_reference<IStream>::value,
                                          std::reference_wrapper<stream_type>,
                                          IStream>::type Holder;
        Holder is_;
        value_type reader_;
        char_type delim_;
        keep_delimiter kd_;
    };

    /** @brief Создание последовательности, читающей поток ввода построчно
    @param is ссылка на поток ввода
    @return <tt> by_line_sequence<IStream>(is) </tt>
    */
    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream && is)
    {
        return by_line_sequence<IStream>(std::forward<IStream>(is));
    }

    /** @brief Создание последовательности, читающей поток ввода блоками,
    разделёнными заданным символом.
    @param is ссылка на поток ввода
    @param delimiter символ-разделитель
    @return <tt> by_line_sequence<IStream>(is, std::move(delimiter)) </tt>
    */
    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream && is,
            typename std::remove_reference<IStream>::type::char_type delimiter)
    {
        return by_line_sequence<IStream>(std::forward<IStream>(is),
                                         std::move(delimiter));
    }

     /** @brief Создание последовательности, читающей поток ввода блоками,
    разделёнными заданным символом.
    @param is ссылка на поток ввода
    @param delimiter символ-разделитель
    @param kd показывает, нужно ли сохранять символы разделители
    @return <tt> by_line_sequence<IStream>(is, std::move(delimiter), kd) </tt>
    */
    template <class IStream>
    by_line_sequence<IStream>
    by_line(IStream && is,
            typename std::remove_reference<IStream>::type::char_type delimiter,
            keep_delimiter kd)
    {
        return by_line_sequence<IStream>(std::forward<IStream>(is),
                                         std::move(delimiter), kd);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BY_LINE_HPP_INCLUDED
