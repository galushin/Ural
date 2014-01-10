#ifndef Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED

#include <ural/tuple.hpp>

#include <ural/functional/cpp_operators.hpp>

namespace ural
{
    template <class T, class BinaryPredicate = ural::equal_to<T> >
    class replace_functor
    {
    public:
        typedef BinaryPredicate predicate_type;

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

        T const & operator()(T const & x) const
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
