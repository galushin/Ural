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
    @todo параметр по умолчанию OStream
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

    template <class Input, class Predicate>
    class remove_if_sequence
     : public sequence_base<remove_if_sequence<Input, Predicate>>
    {
    public:
        // Типы
        typedef typename Input::reference reference;
        typedef typename Input::value_type value_type;

        // @todo Усилить категорию обхода
        typedef typename std::common_type<typename Input::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        // Конструкторы
        remove_if_sequence(Input input, Predicate pred)
         : members_{std::move(input), std::move(pred)}
        {
            this->seek();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            return this->base().front();
        }

        void pop_front()
        {
            members_.first().pop_front();
            this->seek();
        }

        // Адаптор последовательности
        Input const & base() const
        {
            return members_.first();
        }

        Predicate const & predicate() const
        {
            return members_.second();
        }

    private:
        void seek()
        {
            members_.first()
                =  ::ural::details::find_if_not(this->base(), this->predicate());
        }

    private:
        boost::compressed_pair<Input, Predicate> members_;
    };

    template <class Input, class Predicate>
    auto make_remove_if_sequence(Input && in, Predicate pred)
    -> remove_if_sequence<decltype(sequence(std::forward<Input>(in))),
                          decltype(make_functor(std::move(pred)))>
    {
        typedef remove_if_sequence<decltype(sequence(std::forward<Input>(in))),
                                   decltype(make_functor(std::move(pred)))> Sequence;
        return Sequence(sequence(std::forward<Input>(in)), make_functor(std::move(pred)));
    }

    /**
    @todo Выразить через remove_if_sequence
    @todo Вынести в отдельный файл
    */
    template <class Input, class T, class BinaryPredicate>
    class remove_sequence
     : public sequence_base<remove_sequence<Input, T, BinaryPredicate>>
    {
    public:
        // Типы
        typedef typename Input::reference reference;
        typedef typename Input::value_type value_type;

        // @todo Усилить категорию обхода
        typedef typename std::common_type<typename Input::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        // Конструкторы
        explicit remove_sequence(Input in, T const & value, BinaryPredicate pred)
         : members_{std::move(in), value, std::move(pred)}
        {
            this->seek();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !members_[ural::_1];
        }

        reference front() const
        {
            return members_[ural::_1].front();
        }

        void pop_front()
        {
            ++ members_[ural::_1];
            this->seek();
        }

        // Адаптор последовательности
        T const & removed_value() const
        {
            return members_[ural::_2];
        }

        BinaryPredicate const & predicate() const
        {
            return members_[ural::_3];
        }

    private:
        void seek()
        {
            // @todo Заменить на алгоритм
            for(; !!members_[ural::_1]; ++ members_[ural::_1])
            {
                if(!this->predicate()(*members_[ural::_1], this->removed_value()))
                {
                    return;
                }
            }
        }

    private:
        ural::tuple<Input, T, BinaryPredicate> members_;
    };

    template <class Input, class T, class BinaryPredicate>
    auto make_remove_sequence(Input && in, T const & value, BinaryPredicate pred)
    -> remove_sequence<decltype(sequence(std::forward<Input>(in))), T,
                       decltype(make_functor(std::move(pred)))>
    {
        typedef remove_sequence<decltype(sequence(std::forward<Input>(in))), T,
                       decltype(make_functor(std::move(pred)))> Sequence;
        return Sequence(sequence(std::forward<Input>(in)), value,
                        make_functor(std::move(pred)));
    }

    template <class Input, class T>
    auto make_remove_sequence(Input && in, T const & value)
    -> decltype(make_remove_sequence(std::forward<Input>(in), value, ural::equal_to<>{}))
    {
        return make_remove_sequence(std::forward<Input>(in), value, ural::equal_to<>{});
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
