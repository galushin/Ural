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

namespace ural
{
    /**
    @brief Последовательность, записывающая элементы в поток вывода
    @todo Возможность задавать начальное значение: для типов без конструктора
    без параметров
    К сожалению, поддерживать типы без конструктора без параметров, в общем
    случае, нельзя. Дело в том, что не существует стандартного механизма
    инициализации из потока ввода. Лучшее, что мы можем сделать --- это
    предоставить возможность задавать начальное значение.
    */
    template <class T, class IStream = std::istream>
    class istream_sequence
     : public sequence_base<istream_sequence<T, IStream>>
    {
    public:
        // Типы
        typedef T value_type;
        typedef T const & reference;
        typedef single_pass_traversal_tag traversal_tag;

        // Конструкторы
        explicit istream_sequence(IStream & is)
         : is_(is)
         , value_{}
        {
            if(is_.get())
            {
                is_.get() >> value_;
            }
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !is_.get();
        }

        reference front() const
        {
            return value_;
        }

        void pop_front()
        {
            assert(is_.get());
            is_.get() >> value_;
        }

    private:
        std::reference_wrapper<IStream> is_;
        T value_;
    };

    template <class T, class IStream>
    istream_sequence<T, IStream>
    make_istream_sequence(IStream & is)
    {
        return istream_sequence<T, IStream>(is);
    }

    template <class OStream, class Delimeter>
    struct default_delimeter_helper
     : public declare_type<Delimeter>
    {};

    template <class OStream>
    struct default_delimeter_helper<OStream, use_default>
     : public declare_type<std::basic_string<typename OStream::char_type,
                                             typename OStream::traits_type>>
    {};

    template <class T, class U>
    struct ostream_sequence_enable
     : std::enable_if<std::is_same<T, use_default>::value, void>
    {};

    /**
    @brief Последовательность для потока вывода
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
        typedef single_pass_traversal_tag traversal_tag;

        typedef typename default_helper<OStream, std::ostream>::type
            ostream_type;

        typedef typename default_delimeter_helper<ostream_type, Delimeter>::type
            delimeter_type;

        // Конструктор
        explicit ostream_sequence(ostream_type & os)
         : data_{os}
        {}

        explicit ostream_sequence(ostream_type & os, Delimeter delim)
         : data_{os, std::move(delim)}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return false;
        }

        ostream_sequence const & operator*() const
        {
            return *this;
        }

        //@{
        void operator=(T const & x) const
        {
            data_.first().get() << x << data_.second();
        }

        template <class U>
        typename ostream_sequence_enable<T, U>::type
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

        void pop_front()
        {}

    private:
        boost::compressed_pair<std::reference_wrapper<ostream_type>, Delimeter> data_;
    };

    struct no_delimeter
    {};

    template <class OStream>
    OStream & operator<<(OStream & os, no_delimeter)
    {
        return os;
    }

    template <class T, class OStream, class Delimeter>
    ostream_sequence<OStream, T, Delimeter>
    make_ostream_sequence(OStream & os, Delimeter delim)
    {
        return ostream_sequence<OStream, T, Delimeter>(os, std::move(delim));
    }

    template <class OStream, class Delimeter>
    ostream_sequence<OStream, use_default, Delimeter>
    make_ostream_sequence(OStream & os, Delimeter delim)
    {
        return ostream_sequence<OStream, use_default, Delimeter>(os, std::move(delim));
    }

    template <class T, class OStream>
    ostream_sequence<OStream, T, no_delimeter>
    make_ostream_sequence(OStream & os)
    {
        return ostream_sequence<OStream, T, no_delimeter>(os);
    }

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
