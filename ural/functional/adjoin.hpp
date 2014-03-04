#ifndef Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED

#include <ural/utility.hpp>
#include <ural/functional/make_functor.hpp>

namespace ural
{
    template <class... Fs>
    class adjoin_functor_type
    {
    public:
        explicit adjoin_functor_type(Fs... fs)
         : functors_{std::move(fs)...}
        {}

        template <class... Args>
        auto operator()(Args && ... args) const
        -> ural::tuple<decltype(std::declval<Fs>()(args...))...>
        {
            // @todo Можно ли реализовать без псевдо-рекурсии
            typedef ural::tuple<decltype(std::declval<Fs>()(args...))...> R;
            return call_impl(declare_type<R>{},
                             std::forward_as_tuple(std::forward<Args>(args)...),
                             ural::_1, placeholder<sizeof...(Fs)>{});
        }

    private:
        template <class R, class Tuple, size_t last, class... Rs>
        R call_impl(declare_type<R> rd, Tuple && t,
                    placeholder<last>, placeholder<last>, Rs && ... rs) const
        {
            return R(std::forward<Rs>(rs)...);
        }

        template <class R, class Tuple, size_t first, size_t last, class... Rs>
        R call_impl(declare_type<R> rd, Tuple && t,
                    placeholder<first>, placeholder<last> stop,
                    Rs && ... rs) const
        {
            return call_impl(rd, std::forward<Tuple>(t),
                             placeholder<first+1>{}, stop,
                             std::forward<Rs>(rs)...,
                             apply(std::get<first>(functors_), std::forward<Tuple>(t)));
        }

    private:
        ural::tuple<Fs...> functors_;
    };

    template <class... Fs>
    auto adjoin_functors(Fs... fs)
    -> adjoin_functor_type<decltype(ural::make_functor(std::move(fs)))...>
    {
        typedef adjoin_functor_type<decltype(ural::make_functor(std::move(fs)))...>
            Functor;
        return Functor{ural::make_functor(std::move(fs))...};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED
