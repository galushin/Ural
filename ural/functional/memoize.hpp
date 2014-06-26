#ifndef Z_URAL_MEMOIZE_HPP_INCLUDED
#define Z_URAL_MEMOIZE_HPP_INCLUDED

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

/** @file ural/functional/memoize.hpp
 @brief Адаптор функционального объекта, кэширующий значения
*/

#include <map>

namespace ural
{
    /** @todo Настройка контейнера-кэша
    @todo Настройка синхронизации
    @todo Тесты с различными типами аргументов
    */
    template <class Signature, class F>
    class memoize_functor;

    template <class R, class... Args, class F>
    class memoize_functor<R(Args...), F>
    {
    public:
        // Типы
        typedef R result_type;
        typedef decltype(ural::make_functor(std::declval<F>())) target_type;

        // Конструкторы
        explicit memoize_functor(F f)
         : members_{std::move(f), Cache{}}
        {}

        // Применение функционального объекта
        /** @brief Оптимальные типы параметров
        */
        result_type
        operator()(typename boost::call_traits<Args>::param_type... args)
        {
            // @todo Оптимизация
            auto pos = this->cache().find(args_tuple(args...));

            if(pos != this->cache().end())
            {
                return pos->second;
            }
            else
            {
                auto result = this->target()(args...);

                cache()[args_tuple(args...)] = result;

                return result;
            }
        }

        // Свойства
        target_type const & target() const
        {
            return members_[ural::_1];
        }

    private:
        typedef ural::tuple<Args...> args_tuple;
        typedef std::map<args_tuple, result_type> Cache;

        Cache & cache()
        {
            return members_[ural::_2];
        }

    private:
        ural::tuple<target_type, Cache> members_;
    };

    template <class Signature, class F>
    memoize_functor<Signature, F>
    memoize(F f)
    {
        return memoize_functor<Signature, F>{std::move(f)};
    }
}
// namespace ural

#endif
// Z_URAL_MEMOIZE_HPP_INCLUDED
