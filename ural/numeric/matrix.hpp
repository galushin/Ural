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

#include <ural/utility.hpp>
#include <ural/sequence/base.hpp>
#include <ural/functional/compare_by.hpp>

#include <boost/numeric/ublas/matrix_proxy.hpp>
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
            for(size_type row = 0; row != this->size1(); ++ row)
            {
                assert(xs.begin()[row].size() == this->size2());
                for(size_type col = 0; col != this->size2(); ++ col)
                {
                    Base::operator()(row, col) = xs.begin()[row].begin()[col];
                }
            }
        }

    private:
        template <class It>
        static size_type max_size(It first, It last)
        {
            if(first == last)
            {
                return size_type{0};
            }

            typedef std::initializer_list<T> Row;

            auto r = std::max_element(first, last, ural::compare_by(&Row::size));

            assert(r != last);

            return r->size();
        }
    };

    /** @brief Итератор элементов главной диагонали матрицы
    @tparam Matrix тип матрицы
    */
    template <class Matrix>
    class diagonal_iterator
    {
    public:
        /// @brief Категория итератора
        typedef std::random_access_iterator_tag iterator_category;

    private:
    };

    /** @brief Адаптор для главной диагонали матрицы
    @tparam Matrix тип матрицы
    */
    template <class Matrix>
    class diagonal_adaptor
     : public boost::numeric::ublas::vector_expression<diagonal_adaptor<Matrix>>
    {
    public:
        // Типы
        /// @brief Тип размера
        typedef typename Matrix::size_type size_type;

        /// @brief Тип значений
        typedef typename Matrix::value_type value_type;

        /// @brief Тип ссылки
        typedef typename Matrix::const_reference const_reference;

        /// @brief Тип итератора для чтения
        typedef diagonal_iterator<Matrix const> const_iterator;

        // Конструкторы
        /** @brief Конструктор
        @param x ссылка на матрицу
        @post <tt> this->size() == std::min(x.size1(), x.size2()) </tt>
        */
        diagonal_adaptor(Matrix & x)
         : ref_{x}
         , size_{std::min(x.size1(), x.size2())}
        {}

        // Свойства
        /** @brief Размер
        @return Количество элементов на диагонали
        */
        size_type size() const
        {
            return this->size_;
        }

        /** @brief Доступ к элементу
        @param index индекс
        @return элемент матрицы, номер строки и столбца которого равны @c index
        */
        const_reference operator()(size_type index) const
        {
            assert(index < this->size());

            return ref_.get()(index, index);
        }

    private:
        std::reference_wrapper<Matrix> ref_;
        size_type size_;
    };

    /** @brief Главная диагональ матрицы
    @param a матрица
    @return Вектор, содержащий диагональные элементы матрицы @c a
    */
    template <class Matrix>
    diagonal_adaptor<Matrix const>
    diag(boost::numeric::ublas::matrix_expression<Matrix> const & a)
    {
        return diagonal_adaptor<Matrix const>{a};
    }

    /** @brief Последовательность строк матрицы
    @tparam Matrix тип матрицы
    */
    template <class Matrix>
    class matrix_by_rows_sequence
     : public ural::sequence_base<matrix_by_rows_sequence<Matrix>>
    {
        typedef typename Matrix::size_type size_type;
    public:
        // Типы
        /// @brief Категория обхода
        typedef ural::forward_traversal_tag traversal_tag;

        /// @brief Тип значения
        typedef boost::numeric::ublas::matrix_row<Matrix> value_type;

        /// @brief Тип, возвращаемый операциями разыменования
        typedef value_type reference;

        // Конструкторы
        /** @brief Конструктор
        @param x матрица
        */
        explicit matrix_by_rows_sequence(Matrix & x)
         : m_(x)
         , end_(x.size1())
         , row_(0)
        {}

        /** @brief Конструктор
        @param x матрица
        @param nrows количество строк, которые нужно взять
        */
        explicit matrix_by_rows_sequence(Matrix & x, size_type nrows)
         : m_(x)
         , end_(nrows)
         , row_(0)
        {}

        // Однопроходная последовательность
        /** @brief Текущий передний элемент последовательности
        @return Текущий передний элемент последовательности
        @pre <tt> !!*this </tt>
        */
        reference front() const
        {
            assert(!!*this);
            return value_type(m_, row_.value());
        }

        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return row_.value() == end_;
        }

        /** @brief Переход к следующему элементу последовательности
        @pre <tt> !!*this </tt>
        @return <tt> *this </tt>
        */
        matrix_by_rows_sequence & pop_front()
        {
            assert(!!*this);
            ++ row_.value();
            return *this;
        }

        // Прямая последовательность
        /** @brief Пройденная передняя часть последовательности
        */
        matrix_by_rows_sequence traversed_front() const
        {
            return matrix_by_rows_sequence(m_, row_.old_value());
        }

    private:
        // @todo Настройка структуры
        std::reference_wrapper<Matrix> m_;
        size_type end_;
        with_old_value<size_type> row_;
    };

    /** @brief Создание последовательности строк матрицы
    @param x матрица
    @return <tt> matrix_by_rows_sequence<Matrix>(std::move(x)) </tt>
    */
    template <class Matrix>
    matrix_by_rows_sequence<Matrix>
    matrix_by_rows(Matrix & x)
    {
        return matrix_by_rows_sequence<Matrix>(x);
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MATRIX_HPP_INCLUDED
