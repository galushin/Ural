#ifndef Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED

/** @file ural/functional/compose.hpp
 @brief Функциональный объект, реализующий композицию двух функциональных
 объектов
*/

namespace ural
{
    // Композиция функциональных объектов
    template <class UnaryFunctor, class Functor>
    class compose_functor
     : boost::compressed_pair<decltype(ural::make_functor(std::declval<UnaryFunctor>())),
                              decltype(ural::make_functor(std::declval<Functor>()))>
    {
    public:
        typedef decltype(ural::make_functor(std::declval<UnaryFunctor>()))
            first_functor_type;

        typedef decltype(ural::make_functor(std::declval<Functor>()))
            second_functor_type;

    private:
        typedef boost::compressed_pair<first_functor_type, second_functor_type>
            Base;

    public:
        explicit compose_functor(UnaryFunctor f1, Functor f2)
         : Base{std::move(f1), std::move(f2)}
        {}

        first_functor_type const & first_functor() const
        {
            return Base::first();
        }

        second_functor_type const & second_functor() const
        {
            return Base::second();
        }

        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(std::declval<first_functor_type>()(std::declval<second_functor_type>()(std::forward<Args>(args)...)))
        {
            return this->first_functor()(this->second_functor()(std::forward<Args>(args)...));
        }
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED
