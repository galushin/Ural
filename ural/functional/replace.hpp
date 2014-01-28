#ifndef Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED

/** @file ural/functional/replace.hpp
 @brief Функциональные объекты, заменяющие значения, удовлетворяющие
 определённым условиям.
 @todo Сохранять ссылки, а не значения
*/

#include <ural/tuple.hpp>

#include <ural/functional/cpp_operators.hpp>

namespace ural
{
    template <class Predicate, class T>
    class replace_if_functor
    {
    public:
        typedef T const & result_type;

        replace_if_functor(Predicate pred, T const & new_value)
         : members_{std::move(pred), new_value}
        {}

        result_type operator()(T const & x) const
        {
            if(this->predicate()(x))
            {
                return this->new_value();
            }
            else
            {
                return x;
            }
        }

        Predicate const & predicate() const
        {
            return members_.first();
        }

        result_type new_value() const
        {
            return members_.second();
        }

    private:
        boost::compressed_pair<Predicate, T> members_;
    };

    template <class Predicate, class T>
    replace_if_functor<Predicate, T>
    make_replace_if_functor(Predicate pred, T const & new_value)
    {
        return replace_if_functor<Predicate, T>(std::move(pred), new_value);
    }

    template <class T, class BinaryPredicate = ural::equal_to<T> >
    class replace_functor
    {
    public:
        typedef BinaryPredicate predicate_type;
        typedef T const & result_type;

        explicit replace_functor(T old_value, T new_value)
         : members_{std::move(old_value), std::move(new_value),
                    BinaryPredicate{}}
        {}

        explicit replace_functor(T old_value, T new_value, BinaryPredicate pred)
         : members_{std::move(old_value), std::move(new_value), std::move(pred)}
        {}

        T const & old_value() const
        {
            return members_[ural::_1];
        }

        T const & new_value() const
        {
            return members_[ural::_2];
        }

        predicate_type const & predicate() const
        {
            return members_[ural::_3];
        }

        result_type operator()(T const & x) const
        {
            if(this->predicate()(x, this->old_value()))
            {
                return this->new_value();
            }
            else
            {
                return x;
            }
        }

    private:
        // @todo Оптимизация размера
        // @todo Выразить через replace_if_functor
        ural::tuple<T, T, BinaryPredicate> members_;
    };

    template <class T, class BinaryPredicate>
    replace_functor<T, BinaryPredicate>
    make_replace_functor(T old_value, T new_value, BinaryPredicate pred);

    template <class T>
    replace_functor<T, ural::equal_to<T,T>>
    make_replace_functor(T old_value, T new_value)
    {
        return replace_functor<T, ural::equal_to<T,T>>(std::move(old_value),
                                                       std::move(new_value));
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
