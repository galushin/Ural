#ifndef Z_URAL_NUMERIC_MATRIX_HPP_INCLUDED
#define Z_URAL_NUMERIC_MATRIX_HPP_INCLUDED

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

/** @file ural/numeric/matrix.hpp
 @brief Матрица для численных приложений
*/

#include <ural/functional/compare_by.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <algorithm>

namespace ural
{
    /** @brief Класс матрицы
    @tparam T тип элементов матрицы
    @todo Настройка порядка хранения (по строкам или по столбцам)
    @todo Настройка хранилища, по умолчанию --- вектор
    @todo Отказаться от реализации через boost::numeric::ublas::matrix
    */
    template <class T>
    class matrix
     : public boost::numeric::ublas::matrix<T>
    {
        typedef boost::numeric::ublas::matrix<T> Base;
    public:
        // Типы
        typedef T value_type;
        typedef typename Base::difference_type difference_type;
        typedef typename Base::size_type size_type;
        typedef typename Base::reference reference;
        typedef typename Base::const_reference const_reference;

        typedef const boost::numeric::ublas::matrix_reference<const matrix> const_closure_type;
        typedef boost::numeric::ublas::matrix_reference<matrix> closure_type;

        typedef typename Base::storage_category storage_category;
        typedef typename Base::vector_temporary_type vector_temporary_type;
        typedef typename Base::orientation_category orientation_category;

        typedef typename Base::iterator1 iterator1;
        typedef typename Base::const_iterator1 const_iterator1;
        typedef typename Base::iterator2 iterator2;
        typedef typename Base::const_iterator2 const_iterator2;

        // Конструкторы
        using Base::Base;

        /** @brief Конструктор
        @param xs список элементов матрицы
        @post <tt> this->size1() ==  xs.size() </tt>
        @post <tt> this->size2() ==  max{xs[i].size()} </tt>
        */
        matrix(std::initializer_list<std::initializer_list<T>> xs)
         : Base{xs.size(), matrix::max_size(xs.begin(), xs.end())}
        {
            // @todo Убедиться, что все строки одинаковой длинны
            for(size_type row = 0; row != this->size1(); ++ row)
            for(size_type col = 0; col != this->size2(); ++ col)
            {
                Base::operator()(row, col) = xs.begin()[row].begin()[col];
            }
        }

    private:
        template <class It>
        static size_type max_size(It first, It last)
        {
            typedef std::initializer_list<T> Row;

            auto r = std::max_element(first, last, ural::compare_by(&Row::size));

            assert(r != last);

            return r->size();
        }
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MATRIX_HPP_INCLUDED
