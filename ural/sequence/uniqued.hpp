#ifndef Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED

namespace ural
{
    template <class Forward, class BinaryPredicate = ural::equal_to<> >
    class unique_sequence
     : sequence_base<unique_sequence<Forward, BinaryPredicate>>
    {
    public:
        // Типы
        typedef typename Forward::reference reference;
        typedef typename Forward::value_type value_type;

        // Конструкторы
        explicit unique_sequence(Forward in)
         : data_(std::move(in))
        {}

        // Адаптор последовательности
        Forward const & base() const
        {
            return this->data_.first();
        }

        BinaryPredicate const & predicate() const
        {
            return this->data_.second();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        reference operator*() const
        {
            return *this->base();
        }

        unique_sequence & operator++()
        {
            auto s = this->base();
            ++ s;

            data_.first() = ::ural::details::find(s, *this->base(),
                                                  not_fn(this->predicate()));

            return *this;
        }

    private:
        boost::compressed_pair<Forward, BinaryPredicate> data_;
    };

    template <class Forward>
    auto make_unique_sequence(Forward && in)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        typedef unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
            Result;

         return Result{sequence(std::forward<Forward>(in))};
    }

    struct uniqued_helper{};

    template <class Forward>
    auto operator|(Forward && in, uniqued_helper)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in));
    }

    constexpr auto uniqued = uniqued_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
