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

#include <boost/numeric/ublas/triangular.hpp>

namespace ural
{
    template <class M, class Type>
    struct make_triangular_matrix
    {
    private:
        typedef typename M::value_type Value;

    public:
        typedef boost::numeric::ublas::triangular_matrix<Value, Type> type;
    };

    /** @brief QR-разложение матрицы: представление матрицы в виде произведения
    ортогональной и верхне-треугольной матрицы.
    @param A разлагаемая матрица
    @return <tt> make_tuple(Q, R) </tt>, где @c Q --- ортогональная матрица,
    @c R --- верхняя треугольная матрица, причём <tt> Q R == A </tt>.
    @todo настройка метода вычисления
    @todo Должна ли матрица быть квадратной?
    @todo Настройка операции скалярного произведения
    @todo Вторая компонента должна быть треугольной матрицей
    */
    template <class Matrix>
    ural::tuple<Matrix, typename make_triangular_matrix<Matrix, boost::numeric::ublas::upper>::type>
    QR_decomposition(Matrix Q)
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
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MATRIX_DECOMPOSITION_HPP_INCLUDED
