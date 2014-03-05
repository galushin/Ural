#ifndef Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED

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

#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
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
// Z_URAL_SEQUENCE_REMOVE_HPP_INCLUDED
