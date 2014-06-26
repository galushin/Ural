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
#include <mutex>

namespace ural
{
    /** @brief Адаптор функционального объекта, кэширующий значения
    @tparam Signature сигнатура вызова
    @tparam F тип базового функционального объекта
    @tparam Threading стратегия работы с многопоточьностью
    @tparam Map шаблон ассоциативного контейнера, кэширующего значения

    @todo Тесты с различными типами аргументов
    @todo Возможность очищать кэш и управлять его ёмкостью
    */
    template <class Signature, class F,
              class Threading = use_default,
              template <class...> class Map = std::map>
    class memoize_functor;

    /** @brief Адаптор функционального объекта, кэширующий значения
    @tparam R тип возвращаемого значения
    @tparam Args типы аргументов
    @tparam F тип базового функционального объекта
    @tparam Threading стратегия работы с многопоточьностью
    @tparam Map шаблон ассоциативного контейнера, кэширующего значения
    */
    template <class R, class... Args, class F,
              class Threading, template <class...> class Map>
    class memoize_functor<R(Args...), F, Threading, Map>
    {
    public:
        // Типы
        /// @brief Тип возвращаемого значения
        typedef R result_type;

        /// @brief Тип функционального объекта
        typedef decltype(ural::make_functor(std::declval<F>())) target_type;

        // Конструкторы
        /** @brief Конструктор
        @param f базовый функциональный объект
        @post <tt> this->target() == f </tt>
        */
        explicit memoize_functor(F f)
         : members_{std::move(f), Cache{}}
         , mutex_{}
        {}

        memoize_functor(memoize_functor const &);
        memoize_functor(memoize_functor &&);

        memoize_functor & operator=(memoize_functor const &);
        memoize_functor & operator=(memoize_functor &&);

        // Применение функционального объекта
        /** @brief Применение функционального объекта
        @param args аргументы
        @return Значение адаптируемого функционального объекта при данных
        аргументах.
        */
        template <class... As>
        result_type
        operator()(As &&... args)
        {
            auto x = ural::forward_as_tuple(std::forward<As>(args)...);

            std::lock_guard<mutex_type> lock(this->mutex_ref());

            auto pos = this->cache().lower_bound(x);

            if(pos == this->cache().end() || pos->first != x)
            {
                auto y = this->target()(std::forward<As>(args)...);
                pos = cache().insert(pos, std::make_pair(x, y));

            }

            return pos->second;
        }

        // Свойства
        /** @brief Адаптируемый функциональный объект
        @return Адаптируемый функциональный объект
        */
        target_type const & target() const
        {
            return members_[ural::_1];
        }

    private:
        typedef ural::tuple<Args...> args_tuple;
        typedef Map<args_tuple, result_type> Cache;

        typedef typename default_helper<Threading, single_thread_policy>::type
            threading_policy;

        typedef typename threading_policy::mutex_type mutex_type;

        Cache & cache()
        {
            return members_[ural::_2];
        }

        mutex_type & mutex_ref()
        {
            return mutex_;
        }

    private:
        ural::tuple<target_type, Cache> members_;
        mutex_type mutex_;
    };

    /** @brief Мемоизация функционального объекта
    @tparam Signature сигнатура вызова вида R(Args...)
    @param f адаптируемый функциональный объект
    @return <tt> memoize_functor<Signature, F>{std::move(f)} </tt>
    */
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
