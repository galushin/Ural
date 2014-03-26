#ifndef Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
#define Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED

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

/** @file ural/numeric/adjacent_differences.hpp
 @brief Последовательность разностей соседних элементов
*/

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательность разностей соседних элементов базовой
    последовательности
    @tparam Input тип базовая последовательнсоть
    @tparam BinaryOperation тип бинарной операции, определяющей разность между
    элементами
    @todo Оптимизация размера
    */
    template <class Input, class BinaryOperation>
    class adjacent_differences_sequence
     : public sequence_base<adjacent_differences_sequence<Input, BinaryOperation>>
    {
    public:
        /// @brief Тип значения
        typedef typename Input::value_type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        typedef forward_traversal_tag traversal_tag;

        /** @brief Конструктор
        @param in исходная последовательность
        @param op операция, используемая для вычисления разности
        @post <tt> this->base() == in </tt>
        @post <tt> this->operation() == add </tt>
        */
        explicit adjacent_differences_sequence(Input in, BinaryOperation op)
         : members_{std::move(in), std::move(op), {}, {}}
        {
            if(!!*this)
            {
                members_[ural::_3] = *in;
                members_[ural::_4] = members_[ural::_3];
            }
        }

        /** @brief Проверка исчерпания последовательности
        @return <tt> !this->base() </tt>.
        */
        bool operator!() const
        {
            return !this->base();
        }

        reference front() const
        {
            assert(!!*this);
            return members_[ural::_3].value();
        }

        void pop_front()
        {
            auto old_value = std::move(*members_[ural::_4]);
            ++ members_[ural::_1];

            if(!!*this)
            {
                members_[ural::_4] = *this->base();
                members_[ural::_3] = this->operation()(*members_[ural::_4],
                                                       std::move(old_value));
            }
        }

        Input const & base() const
        {
            return members_[ural::_1];
        }

        BinaryOperation const & operation() const
        {
            return members_[ural::_2];
        }

    private:
        typedef ural::optional<value_type> Optional_value;

        ural::tuple<Input, BinaryOperation, Optional_value, Optional_value> members_;
    };

    /** @brief Создание последовательности разностей соседних элементов
    @param in исходная последовательность
    @param sub операция, определяющая разность
    */
    template <class Input, class BinaryOperation>
    auto adjacent_differences(Input && in, BinaryOperation sub)
    -> adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                     decltype(make_functor(std::move(sub)))>
    {
        typedef adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                              decltype(make_functor(std::move(sub)))> Result;
        return Result{sequence(std::forward<Input>(in)),
                      make_functor(std::move(sub))};
    }

    /** @brief Создание последовательности разностей соседних элементов
    @param in исходная последовательность
    @return <tt> adjacent_differences(std::forward<Input>(in), ural::minus<>{});
            </tt>
    */
    template <class Input>
    auto adjacent_differences(Input && in)
    -> adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                     ural::minus<>>
    {
        return adjacent_differences(std::forward<Input>(in), ural::minus<>{});
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
