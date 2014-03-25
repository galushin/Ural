#ifndef Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FILTERED_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file ural/sequence/filtered.hpp
 @brief Последовательность элементов базовой последовательности, удовлетворяющих
 заданному предикату.
*/

#include <ural/algorithm/details/algo_base.hpp>

#include <boost/compressed_pair.hpp>

namespace boost
{
    // @todo Вынести в отдельный файл
    template <class T1, class T2>
    constexpr bool operator==(compressed_pair<T1, T2> const & x,
                              compressed_pair<T1, T2> const & y)
    {
        return x.first() == y.first()
                && x.second() == y.second();
    }
}
// namespace boost

namespace ural
{
    /** @brief Последовательность элементов базовой последовательности,
    удовлетворяющих заданному предикату.
    @todo Оптимизация размера
    */
    template <class Sequence, class Predicate>
    class filter_sequence
     : public sequence_base<filter_sequence<Sequence, Predicate>>
    {
        friend bool operator==(filter_sequence const & x,
                               filter_sequence const & y)
        {
            return x.data_ == y.data_;
        }

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
            data_.first()
                = ::ural::details::find_if(data_.first(), this->predicate());
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
