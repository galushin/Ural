#ifndef Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED

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

/** @file ural/sequence/all_tuples.hpp
 @brief Последовательность всех кортежей
 @todo Устранить дублирование с @c transform_sequence
*/

#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>

namespace ural
{
    template <class... Inputs>
    class all_tuples_sequence
     : public sequence_base<all_tuples_sequence<Inputs...>>
    {
    public:
        // Типы
        typedef tuple<typename Inputs::reference...> reference;

        // Конструкторы
        explicit all_tuples_sequence(Inputs... ins)
         : bases_{std::move(ins)...}
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return !bases_[ural::_1];
        }

        reference front() const
        {
            auto f = [this](Inputs const & ... args)->reference
                     { return this->deref(args...); };

            return apply(f, bases_);
        }

        void pop_front()
        {
            this->pop_front_impl(ural::_1);
        }

    private:
        bool pop_front_impl(placeholder<sizeof...(Inputs)>)
        {
            return true;
        }

        void pop_front_impl(placeholder<0>)
        {
            bool was_wrap = pop_front_impl(placeholder<1>{});

            if(was_wrap)
            {
                ++ bases_[ural::_1];
            }
        }

        template <size_t I>
        bool pop_front_impl(placeholder<I> first)
        {
            bool was_wrap = pop_front_impl(placeholder<I+1>{});

            if(was_wrap)
            {
                ++ bases_[first];
            }
            else
            {
                return false;
            }

            if(!bases_[first])
            {
                bases_[first] = bases_[first].traversed_front();
                return true;
            }
            else
            {
                return false;
            }
        }

        reference deref(Inputs const & ... ins) const
        {
            return reference((*ins)...);
        }

    private:
        tuple<Inputs...> bases_;
    };

    template <class... Inputs>
    auto make_all_tuples_sequence(Inputs && ... ins)
    -> all_tuples_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
    {
        typedef all_tuples_sequence<decltype(sequence(std::forward<Inputs>(ins)))...>
            Result;
        return Result{sequence(std::forward<Inputs>(ins))...};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_TUPLE_HPP_INCLUDED
