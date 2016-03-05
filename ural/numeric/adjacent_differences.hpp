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
namespace experimental
{
    /** @brief Последовательность разностей соседних элементов базовой
    последовательности
    @tparam Input тип базовая последовательнсоть
    @tparam BinaryOperation тип бинарной операции, определяющей разность между
    элементами
    */
    template <class Input, class BinaryOperation>
    class adjacent_differences_sequence
     : public sequence_base<adjacent_differences_sequence<Input, BinaryOperation>>
    {
    public:
        /// @brief Тип значения
        typedef ValueType<Input> value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Категория курсора
        using cursor_tag = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Input> distance_type;

        /** @brief Конструктор
        @param in исходная последовательность
        @param op операция, используемая для вычисления разности
        @post <tt> this->base() == in </tt>
        @post <tt> this->operation() == add </tt>
        */
        explicit adjacent_differences_sequence(Input in, BinaryOperation op)
         : members_(std::move(in), std::move(op), {}, {})
        {
            if(!!*this)
            {
                members_[ural::_3] = *in;
                members_[ural::_4] = members_[ural::_3];
            }
        }

        // Прямая последовательность
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

        // Прямая последовательность
        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        adjacent_differences_sequence<TraversedFrontType<Input>, BinaryOperation>
        traversed_front() const
        {
            using Result = adjacent_differences_sequence<TraversedFrontType<Input>, BinaryOperation>;
            return Result(this->base().traversed_front(), this->operation());
        }

        // Адаптор последовательности
        //@{
        /** @brief Исходная последовательность
        @return Текущее состояние входной последовательности
        */
        Input const & base() const &
        {
            return members_[ural::_1];
        }

        Input && base() &&
        {
            return ::ural::experimental::get(std::move(this->members_), ural::_1);
        }
        //@}

        /** @brief Операция, используемая для вычисления разности
        @return Операция, используемая для вычисления разности
        */
        BinaryOperation const & operation() const
        {
            return members_[ural::_2];
        }

    private:
        typedef ural::optional<value_type> Optional_value;

        ural::tuple<Input, BinaryOperation, Optional_value, Optional_value> members_;
    };

    /** @brief Тип функционального объекта для создания последовательности
    разностей соседних элементов исходной последовательности
    */
    class adjacent_differences_fn
    {
    public:
        /** @brief Создание последовательности разностей соседних элементов
        @param in исходная последовательность
        @return <tt> adjacent_differences(std::forward<Input>(in), ural::minus<>{});
                </tt>
        */
        template <class Input>
        auto operator()(Input && in) const
        -> adjacent_differences_sequence<decltype(::ural::sequence_fwd<Input>(in)),
                                         ural::minus<>>
        {
            return (*this)(std::forward<Input>(in), ural::minus<>{});
        }

        /** @brief Создание последовательности разностей соседних элементов
        @param in исходная последовательность
        @param sub операция, определяющая разность
        */
        template <class Input, class BinaryOperation>
        auto operator()(Input && in, BinaryOperation sub) const
        -> adjacent_differences_sequence<decltype(::ural::sequence_fwd<Input>(in)),
                                         decltype(make_callable(std::move(sub)))>
        {
            typedef adjacent_differences_sequence<decltype(::ural::sequence_fwd<Input>(in)),
                                                  decltype(make_callable(std::move(sub)))> Result;
            return Result(::ural::sequence_fwd<Input>(in),
                          make_callable(std::move(sub)));
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания последовательности
        разностей соседних элементов исходной последовательности
        */
        constexpr auto const & adjacent_differences = odr_const<adjacent_differences_fn>;

        /** @brief Объект для создания @c adjacent_differences_sequences
        в конвейерном стиле.
        */
        constexpr auto const & adjacent_differenced
            = odr_const<pipeable<adjacent_differences_fn>>;

        /** @brief Объект для создания @c adjacent_differences_sequences
        с заданной операцией, используемой для вычисления разностей, в
        конвейерном стиле.
        */
        constexpr auto const & adjacent_differenced_with
            = odr_const<pipeable_maker<adjacent_differences_fn>>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
