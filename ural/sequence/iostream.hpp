#ifndef Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED
#define Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED

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

/** @file ural/sequence/iostream.hpp
 @brief Последовательности на основе потоков ввода и/или вывода
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательность, записывающая элементы в поток вывода
    @tparam IStream тип потока ввода
    @tparam T тип элементов последовательности
    @todo Возможность задавать разделитель

    К сожалению, поддерживать типы без конструктора без параметров, в общем
    случае, нельзя. Дело в том, что не существует стандартного механизма
    инициализации из потока ввода. Лучшее, что мы можем сделать --- это
    предоставить возможность задавать начальное значение.
    */
    template <class IStream = use_default, class T = use_default>
    class istream_sequence
     : public sequence_base<istream_sequence<IStream, T>>
    {
    public:
        // Типы
        /// @brief Тип потока ввода
        typedef typename default_helper<IStream, std::istream>::type
            istream_type;

        /// @brief Тип значения
        typedef typename default_helper<T, char>::type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип расстояния
        typedef size_t distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param is ссылка на поток ввода
        */
        explicit istream_sequence(istream_type & is)
         : is_(is)
         , value_()
        {
            this->init();
        }

        /** @brief Конструктор (для типов без конструктора без аргументов)
        @param is ссылка на поток ввода
        @param init_value начальное значение
        */
        explicit istream_sequence(istream_type & is, T init_value)
         : is_(is)
         , value_(std::move(init_value))
        {
            this->init();
        }

        istream_sequence(istream_sequence const & ) = delete;
        istream_sequence(istream_sequence && ) = default;

        istream_sequence & operator=(istream_sequence const & ) = delete;
        istream_sequence & operator=(istream_sequence && ) = default;

        // Однопроходная последовательность
        /** @brief Проверка исчерпания
        @return @b true, если последовательность исчерпана, иначе @b --- false
        */
        bool operator!() const
        {
            return !is_.get();
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        @pre <tt> !!*this </tt>
        */
        reference front() const
        {
            return value_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !!*this </tt>
        */
        void pop_front()
        {
            assert(is_.get());
            is_.get() >> value_;
        }

    private:
        void init()
        {
            assert(is_.get());

            is_.get() >> value_;
        }

    private:
        std::reference_wrapper<istream_type> is_;
        T value_;
    };

    /** @brief Создание последовательности на основе потока ввода
    @tparam T тип элементов
    @param is поток ввода
    @return <tt> istream_sequence<T, IStream>(is) </tt>
    */
    template <class T, class IStream>
    istream_sequence<IStream, T>
    make_istream_sequence(IStream & is)
    {
        return istream_sequence<IStream, T>(is);
    }

    /** @brief Вспомогательный класс для определения типа разделителя
    @tparam Stream тип потока ввода/вывода
    @tparam Delimeter тип разделителя
    */
    template <class Stream, class Delimeter>
    struct default_delimeter_helper
     : public declare_type<Delimeter>
    {};

    /** @brief Специализация для синтеза типа разделителя по умолчанию
    @tparam Stream тип потока ввода/вывода
    */
    template <class Stream>
    struct default_delimeter_helper<Stream, use_default>
     : public declare_type<std::basic_string<typename Stream::char_type,
                                             typename Stream::traits_type>>
    {};

    /** @brief Последовательность для потока вывода
    @tparam OStream Тип потока вывода
    @tparam T тип выводимых объектов
    @tparam Delimeter тип разделителя
    */
    template <class OStream = use_default,
              class T  = use_default,
              class Delimeter = use_default>
    class ostream_sequence
     : public sequence_base<ostream_sequence<OStream, T, Delimeter>>
    {
    public:
        // Типы
        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип потока вывода
        typedef typename default_helper<OStream, std::ostream>::type
            ostream_type;

        /// @brief Тип разделителя
        typedef typename default_delimeter_helper<ostream_type, Delimeter>::type
            delimeter_type;

        /// @brief Категория итератора
        typedef std::output_iterator_tag iterator_category;

        /// @brief Тип значения
        typedef void value_type;

        /// @brief Тип расстояния
        typedef void difference_type;

        /// @brief Тип указателя
        typedef void pointer;

        /// @brief Тип ссылки
        typedef void reference;

        // Конструктор
        /** @brief Конструктор
        @param os поток вывода
        */
        explicit ostream_sequence(ostream_type & os)
         : data_{os}
        {}

        /** @brief Конструктор
        @param os поток вывода
        @param delim разделитель
        */
        explicit ostream_sequence(ostream_type & os, delimeter_type delim)
         : data_{os, std::move(delim)}
        {}

        // Однопроходная последовательность
        /** @brief Провекра исчерпания последовательности
        @return @b false.
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return <tt> *this </tt>
        @note Если вместо оператора * определить фунцию @c front, то возникнет
        ошибка, так как общее определение оператора * для последовательностей
        использует тип ссылки
        */
        ostream_sequence const & operator*() const
        {
            return *this;
        }

        /// @brief Переход к следующему элементу. Ничего не делает.
        void pop_front()
        {}

        //@{
        /** @brief Оператор присваивания
        @param x записываемый объект
        */
        void operator=(T const & x) const
        {
            data_.first().get() << x << data_.second();
        }

        template <class U>
        typename std::enable_if<!std::is_same<T, U>::value && std::is_same<T, use_default>::value>::type
        operator=(U const & x) const
        {
            data_.first().get() << x << data_.second();
        }
        //@}

        /** @brief Удалённый оператор присваивания с @c use_default
        @c use_delete --- это тэг, который не предназначен для создания
        объектов.
        */
        void operator=(use_default) const = delete;

    private:
        boost::compressed_pair<std::reference_wrapper<ostream_type>, Delimeter> data_;
    };

    /** @brief Тип используемый, когда формально требуется вывести объектв в
    поток, но ничего выводить фактически выводить не нужно.
    */
    struct no_delimeter
    {};

    /** @brief Оператор вывода для @c no_delimeter
    @param os поток вывода
    @return @c os
    */
    template <class OStream>
    OStream & operator<<(OStream & os, no_delimeter)
    {
        return os;
    }

    /** @brief Создание последовательности на основе потока вывода с явным
    указанием типа записываемых объектов c разделителем
    @tparam T тип записываемых элементов
    @param os поток вывода
    @param delim разделитель
    @return <tt> ostream_sequence<OStream, T, Delimeter>(os, std::move(delim)) </tt>
    */
    template <class T, class OStream, class Delimeter>
    ostream_sequence<OStream, T, Delimeter>
    make_ostream_sequence(OStream & os, Delimeter delim)
    {
        return ostream_sequence<OStream, T, Delimeter>(os, std::move(delim));
    }

    /** @brief Создание последовательности на основе потока вывода без явного
    указания типа записываемых объектов с разделителем
    @param os поток вывода
    @param delim разделитель
    @return <tt> ostream_sequence<OStream, use_default, Delimeter>(os, std::move(delim)) </tt>
    */
    template <class OStream, class Delimeter>
    ostream_sequence<OStream, use_default, Delimeter>
    make_ostream_sequence(OStream & os, Delimeter delim)
    {
        return ostream_sequence<OStream, use_default, Delimeter>(os, std::move(delim));
    }

    /** @brief Создание последовательности на основе потока вывода с явным
    указанием типа записываемых объектов и без разделителя
    @tparam T тип записываемых элементов
    @param os поток вывода
    @return <tt> ostream_sequence<OStream, T, no_delimeter>(os) </tt>
    */
    template <class T, class OStream>
    ostream_sequence<OStream, T, no_delimeter>
    make_ostream_sequence(OStream & os)
    {
        return ostream_sequence<OStream, T, no_delimeter>(os);
    }

    /** @brief Создание последовательности на основе потока вывода без явного
    указания типа записываемых объектов и без разделителя
    @param os поток вывода
    @return <tt> ostream_sequence<OStream, use_default, no_delimeter>(os) </tt>
    */
    template <class OStream>
    ostream_sequence<OStream, use_default, no_delimeter>
    make_ostream_sequence(OStream & os)
    {
        return ostream_sequence<OStream, use_default, no_delimeter>(os);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED
