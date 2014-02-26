#ifndef Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED
#define Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED

/** @file ural/sequence/iota.hpp
 @brief Адаптор последовательности, возвращающий в качестве значения исходную
 последовательность
 @todo Усилить категорию обхода, если это возможно
*/

namespace ural
{
    template <class Incrementable>
    class iota_sequence
     : public sequence_base<iota_sequence<Incrementable>>
    {
    public:
        typedef single_pass_traversal_tag traversal_tag;
        typedef Incrementable value_type;
        typedef value_type const & reference;

        explicit iota_sequence(Incrementable s)
         : base_{std::move(s)}
        {}

        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            return this->base();
        }

        void pop_front()
        {
            ++ base_;
        }

        // Адаптор последовательности
        reference base() const
        {
            return this->base_;
        }

    private:
        Incrementable base_;
    };

    template <class Incrementable>
    iota_sequence<Incrementable>
    make_iota_sequence(Incrementable x)
    {
        return iota_sequence<Incrementable>{std::move(x)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_IOTA_HPP_INCLUDED
