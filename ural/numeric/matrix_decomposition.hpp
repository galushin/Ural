#ifndef Z_URAL_NUMERIC_MATRIX_DECOMPOSITION_HPP_INCLUDED
#define Z_URAL_NUMERIC_MATRIX_DECOMPOSITION_HPP_INCLUDED

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

/** @file ural/numeric/matrix_decomposition.hpp
 @brief Разложения матриц
*/

#include <ural/math.hpp>

#include <boost/numeric/ublas/triangular.hpp>

namespace ural
{
    /** @brief Класс-характеристика преобразующая тип матрицы в соответствующий
    тип треугольной матрицы.
    @param M исходная матрица
    @param Type вид треугольной матрицы (верхняя, нижняя)
    */
    template <class M, class Type>
    struct make_triangular_matrix
    {
    private:
        typedef typename M::value_type Value;

    public:
        /// @brief Тип-результат
        typedef boost::numeric::ublas::triangular_matrix<Value, Type> type;
    };

    /** @brief Функциональный объект вычисления скалярного (внутреннего)
    произведения.
    */
    class inner_prod_functor
    {
    public:
        /** @brief Оператор применения функционального объекта
        @param e1 левый операнд
        @param e2 правый операнд
        @return <tt>inner_prod(std::forward<E1>(e1), std::forward<E1>(e2))</tt>
        */
        template <class E1, class E2>
        auto operator()(E1 && e1, E2 && e2) const
        -> decltype(inner_prod(std::forward<E1>(e1), std::forward<E1>(e2)))
        {
            return inner_prod(std::forward<E1>(e1), std::forward<E1>(e2));
        }
    };

    /** @brief QR-разложение матрицы: представление матрицы в виде произведения
    ортогональной и верхне-треугольной матрицы.
    @param A разлагаемая матрица
    @param inner_prod операция скалярного произведения
    @return <tt> make_tuple(Q, R) </tt>, где @c Q --- ортогональная матрица,
    @c R --- верхняя треугольная матрица, причём <tt> Q R == A </tt>.
    @todo настройка метода вычисления
    */
    template <class Matrix, class InnerProduct>
    tuple<Matrix, typename make_triangular_matrix<Matrix, boost::numeric::ublas::upper>::type>
    QR_decomposition(Matrix Q, InnerProduct inner_prod)
    {
        assert(Q.size1() == Q.size2());
        typename make_triangular_matrix<Matrix, boost::numeric::ublas::upper>::type
            R(Q.size1(), Q.size2());

        for(size_t i = 0; i != Q.size2(); ++ i)
        {
            auto qi = column(Q, i);

            for(size_t j = 0; j != i; ++ j)
            {
                auto qj = column(Q, j);

                R(j, i) = inner_prod(qi, qj);

                qi -= R(j, i) * qj;
            }

            R(i, i) = norm_2(qi);

            assert(R(i, i) != 0);

            qi /= R(i, i);
        }

        return std::make_tuple(std::move(Q), std::move(R));
    }

    /** @brief QR-разложение матрицы: представление матрицы в виде произведения
    ортогональной и верхне-треугольной матрицы.
    @param A разлагаемая матрица
    @return <tt> make_tuple(Q, R) </tt>, где @c Q --- ортогональная матрица,
    @c R --- верхняя треугольная матрица, причём <tt> Q R == A </tt>.
    */
    template <class Matrix>
    tuple<Matrix, typename make_triangular_matrix<Matrix, boost::numeric::ublas::upper>::type>
    QR_decomposition(Matrix Q)
    {
        return QR_decomposition(std::move(Q), inner_prod_functor{});
    }

    template <class Matrix>
    typename make_triangular_matrix<Matrix, boost::numeric::ublas::lower>::type
    matrix_lower_trianle(Matrix const & A)
    {
        // @todo Ослабить это требование
        assert(A.size1() == A.size2());

        typedef boost::numeric::ublas::lower Lower;
        typedef typename make_triangular_matrix<Matrix, Lower>::type Result;
        Result L(A.size1(), A.size2());

        for(size_t i = 0; i != A.size1(); ++ i)
        for(size_t j = 0; j != i+1; ++ j)
        {
            L(i, j) = A(i, j);
        }

        return L;
    }

    /** @brief Разложение Холецкого
    @param A исходная матрица
    @return Такая матрица @c L, что <tt> L * trans(L) == A </tt>
    @todo Можно ли использовать память исходной матрицы
    @todo Можно ли выделить полезные алгоритмы?
    */
    template <class SymMatrix>
    typename make_triangular_matrix<SymMatrix, boost::numeric::ublas::lower>::type
    cholesky_decomposition(SymMatrix const & A)
    {
        assert(A.size1() == A.size2());

        auto L = matrix_lower_trianle(A);

        for(size_t i = 0; i != A.size1(); ++ i)
        {
            for(size_t j = 0; j != i; ++ j)
            {
                for(size_t k = 0; k != j; ++ k)
                {
                    L(i, j) -= L(i, k) * L(j, k);
                }

                L(i, j) /= L(j, j);

                using ural::square;
                L(i, i) -= square(L(i, j));
            }

            assert(L(i, i) >= 0);

            using std::sqrt;
            L(i, i) = sqrt(L(i, i));
        }

        return L;
    }

    /** @brief QR-алгоритм нахождения собственных чисел и векторов
    @param A матрица
    @param max_iter максимальное число итераций
    @return кортеж <tt> {L, V} </tt>, где @c L --- матрица собственных чисел, а
    @c V --- матрица собственных векторов
    @todo Дополнительные критерии остановки
    */
    template <class Matrix>
    tuple<Matrix, Matrix>
    qr_eigenvectors(Matrix A, size_t max_iter, double)
    {
        typedef typename Matrix::value_type Value;
        auto const dim = A.size1();

        assert(A.size2() == dim);

        Matrix V = boost::numeric::ublas::identity_matrix<Value>(dim);

        for(auto n = max_iter; n > 0; -- n)
        {
            auto qr = ural::QR_decomposition(std::move(A));

            A = prod(qr[ural::_2], qr[ural::_1]);
            V = prod(V, qr[ural::_1]);
        }

        return std::make_tuple(std::move(A), std::move(V));
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MATRIX_DECOMPOSITION_HPP_INCLUDED
