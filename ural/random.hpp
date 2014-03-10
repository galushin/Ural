#ifndef Z_URAL_RANDOM_HPP_INCLUDED
#define Z_URAL_RANDOM_HPP_INCLUDED

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

/** @file ural/random.hpp
 @brief Средства генерации случайных чисел
 @todo Пересмотр и оптимизация средств ввода/вывода
*/

#include <ural/numeric.hpp>
#include <ural/defs.hpp>

#include <cstdlib>
#include <vector>
#include <ostream>
#include <istream>

namespace ural
{
    class c_rand_engine
    {
    public:
        typedef decltype(std::rand()) result_type;

        result_type operator()() const
        {
            return std::rand();
        }

        constexpr result_type min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return 0;
        }

        constexpr result_type max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return RAND_MAX;
        }
    };

    template <class IntType = int>
    class discrete_distribution
    {
        typedef double weight_type;
    public:
        // Типы
        typedef IntType result_type;

        class param_type
        {
            friend bool operator==(param_type const & x,
                                   param_type const & y)
            {
                return x.data_ == y.data_;
            }

        public:
            typedef ural::discrete_distribution<IntType> distribution_type;

            param_type()
             : data_{1, std::pair<weight_type, size_t>(1.0, 0)}
             , ps_(1, 1.0)
            {}

            template <class Iterator>
            param_type(Iterator first, Iterator last)
            {
                if(first == last)
                {
                    data_.resize(1, std::pair<weight_type, size_t>(1.0, 0));
                    ps_.resize(1, 1.0);
                }
                else
                {
                    std::vector<weight_type> ws(first, last);
                    this->init(std::move(ws));
                }
            }

            param_type(std::initializer_list<weight_type> ws)
             : param_type(ws.begin(), ws.end())
            {}

            template<class UnaryOperation>
            param_type(size_t nw, weight_type xmin, weight_type xmax,
                       UnaryOperation fw)
            {
                std::vector<double> ws;
                if(nw == 0)
                {
                    ws.reserve(1);
                    ws.push_back(1.0);
                }
                else
                {
                    ws.reserve(nw);

                    auto const delta = (xmax - xmin) / nw;

                    for(size_t k = 0; k < nw; ++ k)
                    {
                        ws.push_back(fw(xmin + k * delta + delta / 2));
                    }
                }

                this->init(std::move(ws));
            }

            // Порождение значений
            template <class URNG>
            result_type operator()(URNG & g) const
            {
                std::uniform_int_distribution<result_type>
                    d_pos(this->min(), this->max());

                auto pos = d_pos(g);

                std::bernoulli_distribution d(data_[pos].first);

                if(d(g))
                {
                    return pos;
                }
                else
                {
                    return data_[pos].second;
                }
            }

            // Свойства
            std::vector<weight_type> probabilities() const
            {
                return ps_;
            }

            result_type min URAL_PREVENT_MACRO_SUBSTITUTION () const
            {
                return result_type{0};
            }

            result_type max URAL_PREVENT_MACRO_SUBSTITUTION () const
            {
                return result_type(data_.size() - 1);
            }

        private:
            void init(std::vector<weight_type> ws)
            {
                // @todo Оптмизация (устранить стэки)
                // @todo Выделить алгоритмы
                auto const w_sum = ural::accumulate(ws, weight_type{0.0});

                data_.reserve(ws.size());

                for(size_t k = 0; k < ws.size(); ++ k)
                {
                    ws[k] /= w_sum;
                    data_.emplace_back(1.0, k);
                }

                ps_ = ws;

                auto const w_uni = weight_type{1.0} / ws.size();

                typedef decltype(data_.size()) size_type;

                typedef std::vector<size_type> Stack;

                Stack small;
                Stack large;

                for(size_type i = 0; i != ws.size(); ++ i)
                {
                    if(ws[i] > w_uni)
                    {
                        large.push_back(i);
                    }
                    else
                    {
                        small.push_back(i);
                    }
                }

                for(; !large.empty() && !small.empty(); )
                {
                    auto large_index = large.back();
                    large.pop_back();

                    auto small_index = small.back();
                    small.pop_back();

                    data_[small_index].first = ws[small_index] / w_uni;
                    data_[small_index].second = large_index;

                    ws[large_index] -= (w_uni - ws[small_index]);
                    ws[small_index] = w_uni;

                    if(ws[large_index] > w_uni)
                    {
                        large.push_back(large_index);
                    }
                    else
                    {
                        small.push_back(large_index);
                    }
                }
            }

        private:
            typedef std::pair<weight_type, size_t> Unit;
            std::vector<Unit> data_;
            std::vector<weight_type> ps_;
        };

        // Конструкторы
        discrete_distribution()
         : param_{}
        {}

        template <class Iterator>
        discrete_distribution(Iterator first, Iterator last)
         : param_{std::move(first), std::move(last)}
        {}

        discrete_distribution(std::initializer_list<weight_type> ws)
         : param_{ws}
        {}

        template<class UnaryOperation>
        discrete_distribution(size_t nw,
                              weight_type xmin, weight_type xmax,
                              UnaryOperation fw)
         : param_(nw, xmin, xmax, std::move(fw))
        {}

        discrete_distribution(param_type const & p)
         : param_{p}
        {}

        // Генерация значений
        template <class URNG>
        result_type operator()(URNG & g) const
        {
            return (*this)(g, this->param_);
        }

        template <class URNG>
        result_type operator()(URNG & g, param_type const & parm) const
        {
            return parm(g);
        }

        // Изменение свойств
        void param(param_type const & parm)
        {
            this->param_ = parm;
        }

        // Свойства
        std::vector<weight_type> probabilities() const
        {
            return param_.probabilities();
        }

        param_type const & param() const
        {
            return this->param_;
        }

        result_type min URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return param_.min();
        }

        result_type max URAL_PREVENT_MACRO_SUBSTITUTION () const
        {
            return param_.max();
        }

    private:
        param_type param_;
    };

    template <class IntType>
    bool operator==(discrete_distribution<IntType> const & x,
                    discrete_distribution<IntType> const & y)
    {
        return x.param() == y.param();
    }

    template <class Ch, class Tr, class IntType>
    std::basic_ostream<Ch, Tr> &
    operator<<(std::basic_ostream<Ch, Tr> & os,
               discrete_distribution<IntType> const & d)
    {
        auto ps = d.probabilities();

        os << ps.size();

        for(auto const & p : ps)
        {
            os << ' ' << p;
        }

        return os;
    }

    template <class Ch, class Tr, class IntType>
    std::basic_istream<Ch, Tr> &
    operator>>(std::basic_istream<Ch, Tr> & is,
               discrete_distribution<IntType> & d)
    {
        typename discrete_distribution<IntType>::result_type n;
        is >> n;
        std::vector<double> ps;
        ps.reserve(n);

        for(; n > 0; -- n)
        {
            double reader;
            is >> reader;
            ps.push_back(reader);
        }

        if(!is.fail())
        {
            d = discrete_distribution<IntType>(ps.begin(), ps.end());
        }
        return is;
    }
}
// namespace ural

#endif
// Z_URAL_RANDOM_HPP_INCLUDED
