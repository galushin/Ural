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
    /** @brief Тип функционального объекта чтения из потока ввода с помощью
    функции-члена get
    */
    struct istream_get_reader
    {
    public:
        /** @brief Чтение из потока
        @param is поток ввода
        @param var переменная, в которую считывается значение
        */
        template <class IStream, class T>
        void operator()(IStream & is, T & var) const
        {
            assert(is);

            var = is.get();
        }
    };

    /** @brief Тип функционального объекта чтения из потока ввода с помощью
    оператора >>
    */
    struct istream_extractor_reader
    {
    public:
        /** @brief Чтение из потока
        @param is поток ввода
        @param var переменная, в которую считывается значение
        */
        template <class IStream, class T>
        void operator()(IStream & is, T & var) const
        {
            assert(is);

            is >> var;
        }
    };

    /** @brief Последовательность, записывающая элементы в поток вывода
    @tparam IStream тип потока ввода
    @tparam T тип элементов последовательности
    @tparam Reader функция чтения, по умолчанию используется
    @c istream_exctractor_reader, использующий оператор >>.
    @todo Возможность задавать разделитель

    К сожалению, поддерживать типы без конструктора без параметров, в общем
    случае, нельзя. Дело в том, что не существует стандартного механизма
    инициализации из потока ввода. Лучшее, что мы можем сделать --- это
    предоставить возможность задавать начальное значение.
    */
    template <class IStream = use_default,
              class T = use_default,
              class Reader = use_default>
    class istream_sequence
     : public sequence_base<istream_sequence<IStream, T, Reader>>
    {
        using Base_type = experimental::DefaultedType<IStream, std::istream &>;
        using Reader_type = experimental::DefaultedType<Reader, istream_extractor_reader>;

        static_assert(std::is_empty<Reader_type>::value, "or we must store it!");

    public:
        // Типы
        /// @brief Тип потока ввода
        typedef typename std::remove_reference<Base_type>::type istream_type;

        /// @brief Тип значения
        using value_type = experimental::DefaultedType<T, char>;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Категория курсора
        using cursor_tag = finite_single_pass_cursor_tag;

        /// @brief Тип расстояния
        typedef typename istream_type::off_type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param is ссылка на поток ввода
        */
        explicit istream_sequence(Base_type is)
         : is_(static_cast<Base_type &&>(is))
         , value_()
        {
            this->read();
        }

        /** @brief Конструктор (для типов без конструктора без аргументов)
        @param is ссылка на поток ввода
        @param init_value начальное значение
        */
        explicit istream_sequence(Base_type is, T init_value)
         : is_(static_cast<Base_type &&>(is))
         , value_(std::move(init_value))
        {
            this->read();
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
            return !this->get_istream();
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
            return this->read();
        }

    private:
        istream_type const & get_istream() const
        {
            return is_;
        }

        istream_type & mutable_istream()
        {
            return is_;
        }

        void read()
        {
            Reader_type{}(this->mutable_istream(), this->value_);
        }

    private:
        typedef typename std::conditional<std::is_reference<Base_type>::value,
                                          std::reference_wrapper<istream_type>,
                                          istream_type>::type Holder;

        Holder is_;
        T value_;
    };

    /** @brief Создание последовательности на основе потока ввода
    @tparam T тип элементов
    @param is поток ввода
    @return <tt> istream_sequence<T, IStream>(is) </tt>
    */
    template <class T, class IStream>
    istream_sequence<IStream, T>
    make_istream_sequence(IStream && is)
    {
        return istream_sequence<IStream, T>(is);
    }

    /** @brief Вспомогательный класс для определения типа разделителя
    @tparam Stream тип потока ввода/вывода
    @tparam delimiter тип разделителя
    */
    template <class Stream, class delimiter>
    struct default_delimiter_helper
     : public declare_type<delimiter>
    {};

    /** @brief Специализация для синтеза типа разделителя по умолчанию
    @tparam Stream тип потока ввода/вывода
    */
    template <class Stream>
    struct default_delimiter_helper<Stream, use_default>
     : public declare_type<std::basic_string<typename Stream::char_type,
                                             typename Stream::traits_type>>
    {};

    /** @brief Последовательность для потока вывода
    @tparam OStream Тип потока вывода
    @tparam T тип выводимых объектов
    @tparam delimiter тип разделителя
    */
    template <class OStream = use_default,
              class T  = use_default,
              class Delimiter = use_default>
    class ostream_sequence
     : public sequence_base<ostream_sequence<OStream, T, Delimiter>>
    {
        using Base_type = experimental::DefaultedType<OStream, std::ostream>;

    public:
        // Типы
        /// @brief Категория курсора
        using cursor_tag = output_cursor_tag;

        /// @brief Тип потока вывода
        typedef typename std::remove_reference<Base_type>::type ostream_type;

        /// @brief Тип разделителя
        typedef typename default_delimiter_helper<ostream_type, Delimiter>::type
            delimiter_type;

        /// @brief Категория итератора
        typedef std::output_iterator_tag iterator_category;

        /// @brief Тип значения
        typedef void value_type;

        /// @brief Тип расстояния
        typedef typename ostream_type::off_type difference_type;

        /// @brief Тип указателя
        typedef void pointer;

        /// @brief Тип ссылки
        typedef void reference;

        // Конструктор
        /** @brief Конструктор
        @param os поток вывода
        */
        explicit ostream_sequence(Base_type os)
         : data_(static_cast<Base_type &&>(os))
        {}

        /** @brief Конструктор
        @param os поток вывода
        @param delim разделитель
        */
        explicit ostream_sequence(Base_type os, delimiter_type delim)
         : data_(static_cast<Base_type &&>(os), std::move(delim))
        {}

        /** @brief Константная ссылка на используемый поток вывода
        @return Константная ссылка на используемый поток вывода
        */
        ostream_type const & stream() const
        {
            return this->data_[ural::_1];
        }

        delimiter_type const & delimiter() const
        {
            return this->data_[ural::_2];
        }

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
        ostream_sequence & operator*()
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
        void operator=(T const & x)
        {
            this->mutable_ostream() << x << this->delimiter();
        }

        template <class U>
        typename std::enable_if<!std::is_same<T, U>::value && std::is_same<T, use_default>::value>::type
        operator=(U const & x)
        {
            this->mutable_ostream() << x << this->delimiter();
        }
        //@}

        /** @brief Удалённый оператор присваивания с @c use_default
        @c use_delete --- это тэг, который не предназначен для создания
        объектов.
        */
        void operator=(use_default) const = delete;

    private:
        ostream_type & mutable_ostream()
        {
            return data_[ural::_1];
        }

        typedef typename std::conditional<std::is_reference<Base_type>::value,
                                          std::reference_wrapper<ostream_type>,
                                          ostream_type>::type Holder;
        ural::tuple<Holder, delimiter_type> data_;
    };

    /** @brief Тип используемый, когда формально требуется вывести объектв в
    поток, но ничего выводить фактически выводить не нужно.
    */
    struct no_delimiter
    {};

    /** @brief Оператор вывода для @c no_delimiter
    @param os поток вывода
    @return @c os
    */
    template <class OStream>
    OStream & operator<<(OStream & os, no_delimiter)
    {
        return os;
    }

    /** @brief Создание последовательности на основе потока вывода с явным
    указанием типа записываемых объектов c разделителем
    @tparam T тип записываемых элементов
    @param os поток вывода
    @param delim разделитель, если не указать этот параметр, то вывод будет
    производится без разделителя
    @return <tt> ostream_sequence<OStream, T, delimiter>(os, std::move(delim)) </tt>
    */
    template <class T, class OStream, class delimiter = no_delimiter>
    auto make_ostream_sequence(OStream && os, delimiter delim = delimiter{})
    {
        typedef ostream_sequence<OStream, T, delimiter> Product;
        return Product(std::forward<OStream>(os), std::move(delim));
    }

    /** @brief Создание последовательности на основе потока вывода без явного
    указания типа записываемых объектов с разделителем
    @param os поток вывода
    @param delim разделитель, если не указать этот параметр, то вывод будет
    производится без разделителя
    @return <tt> ostream_sequence<OStream, use_default, delimiter>(os, std::move(delim)) </tt>
    */
    template <class OStream, class delimiter = no_delimiter>
    auto make_ostream_sequence(OStream && os, delimiter delim = delimiter{})
    {
        typedef ostream_sequence<OStream, use_default, delimiter> Product;
        return Product(std::forward<OStream>(os), std::move(delim));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED
