#ifndef Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED
#define Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED

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

/** @file ural/numeric/interpolation.hpp
 @brief Интерполяция
*/

namespace ural
{
    template <class X = double, class Y = X>
    class newton_polynomial
    {
    public:
        // Типы
        typedef size_t size_type;
        typedef Y result_type;
        typedef X argument_type;
        typedef typename boost::call_traits<argument_type>::param_type param_type;

        // Конструкторы

        // Добавление новых значений
        void update(X const & x, Y const & y)
        {
            auto denom = X{1.0};

            for(auto & d : data_)
            {
                denom *= (x - d.first);
            }

            assert(std::abs(denom) > 0);

            auto K = (y - (*this)(x)) / denom;

            data_.emplace_back(x, std::move(K));
        }

        // Свойства
        size_type degree() const
        {
            return data_.empty() ? 0 : data_.size() - 1;
        }

        result_type operator()(param_type x) const
        {
            if(data_.empty())
            {
                return result_type{0.0};
            }

            auto i = ural::sequence(data_);

            auto r = result_type{i->second};
            auto denom = (x - i->first);
            ++ i;

            for(; !!i; ++ i)
            {
                r += i->second * denom;
                denom *= (x - i->first);
            }

            return r;
        }

    private:
        typedef std::pair<X, Y> Pair;
        std::vector<Pair> data_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED
