#ifndef Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED

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

/** @file ural/functional/compare_by.hpp
 @brief Функциональный объект, сравнивающий свои аргументы по некоторому их
 свойству, а также вспомогательные функции.
*/

#include <ural/tuple.hpp>
#include <ural/functional/cpp_operators.hpp>

namespace ural
{
    /** @brief Функциональный объект, сравнивающий свои аргументы по некоторому
    их свойству.
    @tparam UnaryFunction тип унарного фукнционального объекта
    @tparam Compare тип бинарного предиката
    @todo Проверить, что является пустым, если UnaryFunction и Compare -- пустые
    */
    template <class UnaryFunction, class Compare = ural::less<> >
    class comparer_by
     : UnaryFunction
     , Compare
    {
    public:
        /// @brief Тип функции сравнения (для свойства)
        typedef Compare compare_type;

        /// @brief Тип возвращаемого значения
        typedef bool result_type;

        /** @brief Конструктор
        @param f функциональный объект, задающая свойство, по которому
        сравниваются аргументы.
        @post <tt> this->transformation() == f</tt>
        @post <tt> this->compare() == compare_type{} </tt>
        */
        constexpr explicit comparer_by(UnaryFunction f)
         : comparer_by{std::move(f), compare_type{}}
        {}

        /** @brief Конструктор
        @param f функциональный объект, задающая свойство, по которому
        сравниваются аргументы.
        @param cmp функциональный объект, сравнивающий свойства объектов.
        @post <tt> this->transformation() == f</tt>
        @post <tt> this->compare() == cmp </tt>
        */
        constexpr explicit comparer_by(UnaryFunction f, Compare cmp)
         : UnaryFunction(std::move(f))
         , Compare(std::move(cmp))
        {}

        /** @brief Вычисление значения
        @param x первый аргумент
        @param y второй аргумент
        @return <tt> this->compare()(this->transformation()(x),
                                     this->transformation()(y)) </tt>.
        */
        template <class T1, class T2>
        constexpr bool operator()(T1 const & x, T2 const & y) const
        {
            return this->compare()(this->transformation()(x),
                                   this->transformation()(y));
        }

        /** @brief Функциональный объект, задающая свойство, по которому
        сравниваются аргументы.
        @return Функциональный объект, задающая свойство, по которому
        сравниваются аргументы.
        */
        constexpr UnaryFunction const & transformation() const
        {
            return *this;
        }

        /** @brief Функциональный объект, сравнивающий свойства объектов.
        @return Функциональный объект, сравнивающий свойства объектов.
        */
        constexpr Compare const & compare() const
        {
            return *this;
        }
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.compare() == y.compare()
                 && x.transformation() == y.transformation() </tt>
    */
    template <class F, class Tr>
    bool operator==(comparer_by<F, Tr> const & x, comparer_by<F, Tr> const & y)
    {
        return x.compare() == y.compare()
            && x.transformation() == y.transformation();
    }

    /** @brief Создание функционального объекта, сравнивающего объекты по
    заданному свойству
    @param f функциональный объект, задающая свойство, по которому
    сравниваются аргументы.
    @param cmp функциональный объект, сравнивающий свойства объектов.
    */
    template <class UnaryFunction, class Compare>
    constexpr auto compare_by(UnaryFunction f, Compare cmp)
    -> comparer_by<decltype(make_functor(std::move(f))),
                   decltype(make_functor(std::move(cmp)))>
    {
        typedef comparer_by<decltype(make_functor(std::move(f))),
                   decltype(make_functor(std::move(cmp)))> Functor;
        return Functor(make_functor(std::move(f)), make_functor(std::move(cmp)));
    }

    /** @brief Создание функционального объекта, сравнивающего объекты по
    заданному свойству
    @param f функциональный объект, задающая свойство, по которому
    сравниваются аргументы.
    */
    template <class UnaryFunction>
    constexpr auto compare_by(UnaryFunction f)
    -> comparer_by<decltype(make_functor(std::move(f)))>
    {
        typedef comparer_by<decltype(make_functor(std::move(f)))> Functor;
        return Functor(make_functor(std::move(f)));
    }

    /** @brief Создание функционального объекта, проверяющего равенство двух
    объектов по заданному свойству
    @param f функциональный объект, задающая свойство, по которому
    сравниваются аргументы.
    */
    template <class UnaryFunction>
    auto equal_by(UnaryFunction f)
    -> decltype(compare_by(std::move(f), ural::equal_to<>{}))
    {
        return compare_by(std::move(f), ural::equal_to<>{});
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED
