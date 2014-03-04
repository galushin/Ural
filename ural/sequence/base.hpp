#ifndef Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_BASE_HPP_INCLUDED

/** @file ural/sequence/base.hpp
 @brief Базовый класс для последовательностей (CRTP)
*/

#include <ural/defs.hpp>
#include <utility>

namespace ural
{
    struct single_pass_traversal_tag
    {};

    struct forward_traversal_tag
     : single_pass_traversal_tag
    {};

    struct bidirectional_traversal_tag
     : forward_traversal_tag
    {};

    struct random_access_traversal_tag
     : forward_traversal_tag
    {};

    single_pass_traversal_tag
    decl_common_type(single_pass_traversal_tag, single_pass_traversal_tag);

    forward_traversal_tag
    decl_common_type(forward_traversal_tag, forward_traversal_tag);

    bidirectional_traversal_tag
    decl_common_type(bidirectional_traversal_tag, bidirectional_traversal_tag);

    random_access_traversal_tag
    decl_common_type(random_access_traversal_tag, random_access_traversal_tag);

    template <class... Types>
    struct common_tag;

    template <class T>
    struct common_tag<T>
     : declare_type<T>
    {};

    template <class T1, class T2>
    struct common_tag<T1, T2>
     : declare_type<decltype(decl_common_type(std::declval<T1>(), std::declval<T2>()))>
    {};

    template <class Head, class... Tail>
    struct common_tag<Head, Tail...>
    {
    private:
        typedef typename common_tag<Tail...>::type tail_common_type;

    public:
        typedef typename common_tag<Head, tail_common_type>::type type;
    };

    template <class S>
    typename S::traversal_tag
    make_traversal_tag(S const & )
    {
        return typename S::traversal_tag{};
    }

    /** @brief Базовый класс для последовательностей (CRTP)
    @tparam Seq тип последовательности-наследника
    */
    template <class Seq, class Base = ural::empty_type>
    class sequence_base
     : public Base
    {
        friend Seq sequence(Seq s)
        {
            return s;
        }

        friend Seq operator++(Seq & seq, int)
        {
            Seq seq_old = seq;
            ++ seq;
            return seq_old;
        }

    protected:
        /** @brief Конструктор
        @param args список аргументов для конструктора базового класса
        */
        template <class... Args>
        sequence_base(Args && ... args)
         : Base(std::forward<Args>(args)...)
        {}

        sequence_base() = default;

        ~ sequence_base() = default;
    };

    template <class Seq, class Base>
    Seq & operator++(sequence_base<Seq, Base> & s)
    {
        auto & r = static_cast<Seq&>(s);
        r.pop_front();
        return r;
    }

    template <class Seq, class Base>
    typename Seq::reference
    operator*(sequence_base<Seq, Base> const & s)
    {
        return static_cast<Seq const&>(s).front();
    }

    template <class Sequence>
    Sequence shrink_front(Sequence s)
    {
        s.shrink_front();
        return s;
    }

    namespace details
    {
        template <class Sequence>
        typename Sequence::distance_type
        size(Sequence const & s, single_pass_traversal_tag)
        {
            typename Sequence::distance_type n{0};

            for(auto in = s; !!in; ++ in)
            {
                ++ n;
            }

            return n;
        }

        template <class Sequence>
        typename Sequence::distance_type
        size(Sequence const & s, random_access_traversal_tag)
        {
            return s.size();
        }

        template <class Sequence>
        void advance(Sequence & s, typename Sequence::distance_type n,
                     single_pass_traversal_tag)
        {
            for(; n > 0; -- n)
            {
                ++ s;
            }
        }

        template <class Sequence>
        void advance(Sequence & s, typename Sequence::distance_type n,
                     random_access_traversal_tag)
        {
            s += n;
        }
    }

    template <class Sequence>
    typename Sequence::distance_type
    size(Sequence const & s)
    {
        return ::ural::details::size(s, ural::make_traversal_tag(s));
    }

    template <class Sequence>
    void advance(Sequence & s, typename Sequence::distance_type n)
    {
        return ::ural::details::advance(s, std::move(n),
                                        ural::make_traversal_tag(s));
    }

    template <class Sequence>
    Sequence next(Sequence s, typename Sequence::distance_type n = 1)
    {
        ::ural::advance(s, n);
        return s;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_BASE_HPP_INCLUDED
