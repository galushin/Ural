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

/** @file ural/sequence/remove.hpp
 @brief Адаптор последовательности, исключающий элементы базовой
 последовательности, удовлетворяющие некоторому условию
*/

#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
    /** Адаптор последовательности, пропускающий элементы, удовлетворяющие
    предикату.
    Если базовая последовательность является однопроходной, то
    данная последовательность так же будет однопроходной. В остальных случаях
    категория обхода --- прямая (многопроходная).
    Категория произвольного доступа не может быть реализована, так как размер
    последовательности невозможно узнать, не посетив все элементы.
    Категория двусторонней последовательности требует дополнительных затрат
    при конструировании, которые не нужны (?) при типичном использовании данного
    класса.
    @brief Адаптор последовательности, пропускающий элементы, удовлетворяющие
    предикату
    @tparam Input тип входной последовательности
    @tparam Predicate тип унарного предиката
    @todo Рассмотреть целесообразность реализации возможностей двунаправленной
    последовательности
    @todo Решить: алгоритмы реализуются через последовательности или
    последовательности с использованием алгоритмов
    */
    template <class Input, class Predicate>
    class remove_if_sequence
     : public sequence_base<remove_if_sequence<Input, Predicate>>
    {
    friend bool operator==(remove_if_sequence const & x,
                           remove_if_sequence const & y)
    {
        return x.members_ == y.members_;
    }
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Input::reference reference;

        /// @brief Тип значения
        typedef typename Input::value_type value_type;

        /// @brief Категория обхода
        typedef typename std::common_type<typename Input::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Input::distance_type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param input входая последовательность
        @param pred предикат
        @post <tt> this->base() == input </tt>
        @post <tt> this->predicate() == pred </tt>
        */
        remove_if_sequence(Input input, Predicate pred)
         : members_{std::move(input), std::move(pred)}
        {
            this->seek();
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
            return this->base().front();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            ::ural::get(members_, ::ural::_1).pop_front();

            this->seek();
        }

        // Адаптор последовательности
        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Input const & base() const
        {
            return ural::get(members_, ural::_1);
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return ural::get(members_, ural::_2);
        }

    private:
        void seek()
        {
            ural::get(members_, ural::_1)
                =  ::ural::details::find_if_not(this->base(), this->predicate());
        }

    private:
        tuple<Input, Predicate> members_;
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

    /** @brief Адаптор последовательности, исключающий элементы базовой
    последовательности, равные заданному значению
    @tparam Input Тип базовой последовательности
    @tparam T тип значения
    @tparam BinaryPredicate тип бинарного предиката, задающего равенство.
    */
    template <class Input, class T, class BinaryPredicate>
    class remove_sequence
     : public sequence_base<remove_sequence<Input, T, BinaryPredicate>>
    {
    public:
        // Типы
        /// @brief Тип ссылки
        typedef typename Input::reference reference;

        /// @brief Тип значения
        typedef typename Input::value_type value_type;

        /// @brief Категория обхода
        typedef typename std::common_type<typename Input::traversal_tag,
                                          forward_traversal_tag>::type
            traversal_tag;

        /// @brief Тип указателя
        typedef typename Input::pointer pointer;

        /// @brief Тип расстояния
        typedef typename Input::distance_type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param in базовая последовательность
        @param value пропускаемое значение
        @param pred бинарный предикат
        */
        explicit remove_sequence(Input in, T const & value, BinaryPredicate pred)
         : members_{std::move(in), value, std::move(pred)}
        {
            this->seek();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return !members_[ural::_1];
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return members_[ural::_1].front();
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            ++ members_[ural::_1];
            this->seek();
        }

        // Адаптор последовательности
        /** @brief Пропускаемое значение
        @return Пропускаемое значение
        */
        T const & removed_value() const
        {
            return members_[ural::_2];
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        BinaryPredicate const & predicate() const
        {
            return members_[ural::_3];
        }

    private:
        void seek()
        {
            members_[ural::_1]
                = find_fn{}(std::move(members_[ural::_1]),
                            std::cref(this->removed_value()),
                            ural::not_fn(this->predicate()));
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
