#ifndef Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED

/** @file ural/sequence/filtered.hpp
 @brief Последовательность элементов базовой последовательности, удовлетворяющих
 заданному предикату.
 @todo Более подробные тесты
*/

#include <boost/compressed_pair.hpp>

namespace ural
{
    template <class Sequence, class Predicate>
    class filter_sequence
     : public sequence_base<filter_sequence<Sequence, Predicate>>
    {
    public:
        // Типы
        typedef typename Sequence::reference reference;
        typedef typename Sequence::value_type value_type;

        // @todo Усилить категорию обхода
        typedef typename std::common_type<typename Sequence::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        // Конструкторы
        explicit filter_sequence(Sequence seq, Predicate pred)
         : data_{std::move(seq), std::move(pred)}
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
            return *data_.first();
        }

        void pop_front()
        {
            ++ data_.first();
            this->seek();
        }

        // Адаптор последовательности
        Predicate const & predicate() const
        {
            return data_.second();
        }

        Sequence const & base() const
        {
            return data_.first();
        }

    private:
        void seek()
        {
            for(;!!data_.first() && !this->predicate()(*data_.first());
                ++ data_.first())
            {}
        }

    private:
        boost::compressed_pair<Sequence, Predicate> data_;
    };

    template <class Sequence, class Predicate>
    auto make_filter_sequence(Sequence && seq, Predicate pred)
    -> filter_sequence<decltype(sequence(std::forward<Sequence>(seq))),
                       decltype(make_functor(std::move(pred)))>
    {
        typedef decltype(sequence(std::forward<Sequence>(seq))) Seq;
        typedef decltype(make_functor(std::move(pred))) Pred;
        typedef filter_sequence<Seq, Pred> Result;

        return Result{sequence(std::forward<Sequence>(seq)),
                      make_functor(std::move(pred))};
    }

    template <class Predicate>
    struct filtered_helper
    {
        Predicate predicate;
    };

    template <class Sequence, class Predicate>
    auto operator|(Sequence && seq, filtered_helper<Predicate> helper)
    -> decltype(make_filter_sequence(std::forward<Sequence>(seq),
                                     helper.predicate))
    {
        return make_filter_sequence(std::forward<Sequence>(seq),
                                    helper.predicate);
    }

    template <class Predicate>
    auto filtered(Predicate pred)
    -> filtered_helper<decltype(make_functor(std::move(pred)))>
    {
        typedef filtered_helper<decltype(make_functor(std::move(pred)))> Helper;
        return Helper{make_functor(std::move(pred))};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
