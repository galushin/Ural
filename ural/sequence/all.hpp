#ifndef Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_HPP_INCLUDED

/** @file ural/sequence/all.hpp
 @brief Все последовательности
*/

#include <iterator>

#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/filtered.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/sequence/replace.hpp>
#include <ural/sequence/reversed.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/sequence/taken.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/uniqued.hpp>

namespace ural
{
    /**
    @todo параметр по умолчанию IStream
    */
    template <class T, class IStream>
    class istream_sequence
     : public sequence_base<istream_sequence<T, IStream>>
    {
    public:
        // Типы
        typedef T value_type;
        typedef T const & reference;

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
        // @todo Поддерживать ли типы без конструктора без параметров
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
    @todo параметр по умолчанию OStream
    @todo параметр по умолчанию T
    */
    template <class OStream, class T>
    class ostream_sequence
     : public sequence_base<ostream_sequence<OStream, T>>
    {
    public:
        // Конструктор
        explicit ostream_sequence(OStream & is)
         : is_{is}
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
            is_.get() << x;
        }

        void pop_front()
        {}

    private:
        std::reference_wrapper<OStream> is_;
    };

    template <class OStream>
    class ostream_sequence<OStream, auto_tag>
     : public sequence_base<ostream_sequence<OStream, auto_tag>>
    {
    public:
        // Конструктор
        explicit ostream_sequence(OStream & is)
         : is_{is}
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
            is_.get() << x;
        }

        void pop_front()
        {}

    private:
        std::reference_wrapper<OStream> is_;
    };

    template <class T, class OStream>
    ostream_sequence<OStream, T>
    make_ostream_sequence(OStream & is)
    {
        return ostream_sequence<OStream, T>(is);
    }

    template <class OStream>
    ostream_sequence<OStream, auto_tag>
    make_ostream_sequence(OStream & is)
    {
        return ostream_sequence<OStream, auto_tag>(is);
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
