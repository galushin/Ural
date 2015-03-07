#ifndef Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
#define Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED

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

/** @file ural/sequence/uniqued.hpp
 @brief Последовательность неповторяющихся соседних элементов
*/

#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
    /** @brief Последовательность неповторяющихся соседних элементов базовой
    последовательности
    @tparam Input Тип базовой последовательности
    @tparam BinaryPredicat бинарный предикат, определяющий, совпадают ли два
    последовательных элемента.
    */
    template <class Input, class BinaryPredicate = ural::equal_to<> >
    class unique_sequence
     : public sequence_base<unique_sequence<Input, BinaryPredicate>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Input::reference reference;

        /// @brief Тип значения
        typedef typename Input::value_type value_type;

        /// @brief Категория обхода
        typedef typename ural::common_tag<typename Input::traversal_tag,
                                          forward_traversal_tag>::type traversal_tag;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Input::distance_type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == BinaryPredicate{} </tt>
        */
        explicit unique_sequence(Input in)
         : cur_{std::move(in)}
         , eq_{}
        {}

        /** @brief Конструктор
        @param in входная последовательность
        @param pred используемый предикат
        @post <tt> this->base() == in </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        explicit unique_sequence(Input in, BinaryPredicate pred)
         : cur_{std::move(in)}
         , eq_(std::move(pred))
        {}

        // Адаптор последовательности
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Input const & base() const
        {
            return this->cur_;
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return this->eq_;
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            assert(!!*this);
            return *this->base();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            assert(!!*this);
            return this->pop_front_impl(traversal_tag{});
        }

    private:
        // @todo Вынести, чтобы уменьшить раздувание кода?
        void pop_front_impl(single_pass_traversal_tag)
        {
            auto cur_value = *cur_;
            ++ cur_;

            cur_ = find_fn{}(cur_, cur_value, not_fn(this->predicate()));
        }

        void pop_front_impl(forward_traversal_tag)
        {
            /* В большинстве случаев последовательность дешевле копировать, чем
            объекты, поэтому если последовательность позволяет многократный
            обход, то лучше копировать её, а не элемент
            */
            Input old_cur_ = cur_;
            ++ cur_;

            cur_ = find_fn{}(cur_, *old_cur_, not_fn(this->predicate()));
        }

    private:
        Input cur_;
        BinaryPredicate eq_;
    };

    /** @brief Функция создания @c unique_sequence
    @param in входная последовательность
    @param pred бинарный предикат
    */
    template <class Forward, class BinaryPredicate>
    auto make_unique_sequence(Forward && in, BinaryPredicate pred)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in))),
                        decltype(make_functor(std::move(pred)))>
    {
        typedef unique_sequence<decltype(sequence(std::forward<Forward>(in))),
                        decltype(make_functor(std::move(pred)))> Seq;
        return Seq(sequence(std::forward<Forward>(in)), make_functor(std::move(pred)));
    }

    /** @brief Функция создания @c unique_sequence
    @param in входная последовательность
    @return <tt> unique_sequence<Seq>(sequence_fwd<Forward>(in)) </tt>, где
    @c Seq -- <tt> unique_sequence<decltype(sequence_fwd<Forward>(in))> </tt>
    */
    template <class Forward>
    auto make_unique_sequence(Forward && in)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        typedef unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
            Result;

         return Result{sequence(std::forward<Forward>(in))};
    }

    /** @brief Тип вспомогательного объека для создания @c unique_sequence
    с заданным предикатом в конвейерном стиле
    @tparam Predicate унарный предикат
    */
    template <class Predicate>
    class uniqued_helper_custom
    {
    public:
        /// @brief Используемый предикат
        Predicate predicate;
    };

    /** @brief Тип вспомогательного объекта для создания @c unique_sequence
    в конвейрном стиле
    */
    struct uniqued_helper
    {
    public:
        /** @brief Создание вспомогательного объекта, хранящего предикат
        @param pred предикат
        */
        template <class Predicate>
        auto operator()(Predicate pred) const
        -> uniqued_helper_custom<decltype(make_functor(std::move(pred)))>
        {
            return {std::move(pred)};
        }
    };

    /** @brief Создание @c unique_sequence в конвейерном стиле
    @param in входная последовательность
    @return <tt> make_unique_sequence(std::forward<Forward>(in)) </tt>
    */
    template <class Forward>
    auto operator|(Forward && in, uniqued_helper)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in)))>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in));
    }

    /** @brief Создание @c unique_sequence в конвейерном стиле
    @param in входная последовательность
    @param helper объект, хранящий бинарный предикат
    @return <tt> make_unique_sequence(std::forward<Forward>(in), helper.predicate) </tt>
    */
    template <class Forward, class Predicate>
    auto operator|(Forward && in, uniqued_helper_custom<Predicate> helper)
    -> unique_sequence<decltype(sequence(std::forward<Forward>(in))), Predicate>
    {
        return ::ural::make_unique_sequence(std::forward<Forward>(in),
                                            helper.predicate);
    }

    /// @brief Функциональный объект, создающий @c unique_sequence
    constexpr auto uniqued = uniqued_helper{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_UNIQUED_HPP_INCLUDED
