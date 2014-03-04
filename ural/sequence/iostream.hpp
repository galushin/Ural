#ifndef Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED
#define Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED

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

    /**
    @brief Последовательность для потока вывода
    @tparam OStream Тип потока вывода
    @tparam T тип выводимых объектов
    @tparam Delimeter тип разделителя
    @todo параметр по умолчанию OStream
    @todo параметр по умолчанию Delimeter
    @todo параметр по умолчанию T
    */
    template <class OStream, class T, class Delimeter>
    class ostream_sequence
     : public sequence_base<ostream_sequence<OStream, T, Delimeter>>
    {
    public:
        // Типы
        typedef single_pass_traversal_tag traversal_tag;

        // Конструктор
        explicit ostream_sequence(OStream & os)
         : data_{os}
        {}

        explicit ostream_sequence(OStream & os, Delimeter delim)
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

        void operator=(T const & x) const
        {
            data_.first().get() << x << data_.second();
        }

        void pop_front()
        {}

    private:
        boost::compressed_pair<std::reference_wrapper<OStream>, Delimeter> data_;
    };

    /** @brief Специализация с выводом типа выводимых объектов
    @tparam OStream Тип потока вывода
    @tparam Delimeter тип разделителя
    */
    template <class OStream, class Delimeter>
    class ostream_sequence<OStream, auto_tag, Delimeter>
     : public sequence_base<ostream_sequence<OStream, auto_tag, Delimeter>>
    {
    public:
        // Типы
        typedef single_pass_traversal_tag traversal_tag;

        // Конструктор
        explicit ostream_sequence(OStream & os)
         : data_{os}
        {}

        explicit ostream_sequence(OStream & os, Delimeter delim)
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

        template <class T>
        void operator=(T const & x) const
        {
            data_.first().get() << x << data_.second();
        }

        void pop_front()
        {}

    private:
        boost::compressed_pair<std::reference_wrapper<OStream>, Delimeter> data_;
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
    ostream_sequence<OStream, auto_tag, Delimeter>
    make_ostream_sequence(OStream & os, Delimeter delim)
    {
        return ostream_sequence<OStream, auto_tag, Delimeter>(os, std::move(delim));
    }

    template <class T, class OStream>
    ostream_sequence<OStream, T, no_delimeter>
    make_ostream_sequence(OStream & os)
    {
        return ostream_sequence<OStream, T, no_delimeter>(os);
    }

    template <class OStream>
    ostream_sequence<OStream, auto_tag, no_delimeter>
    make_ostream_sequence(OStream & os)
    {
        return ostream_sequence<OStream, auto_tag, no_delimeter>(os);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_IOSTREAM_HPP_INCLUDED
