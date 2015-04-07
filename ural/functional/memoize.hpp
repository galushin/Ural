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
    @todo Возможность управлять ёмкостью и макс. размером кэша
    @todo Использовать по умолчанию в качестве контейнера boost::flat_map или аналог
    @todo Оператор "меньше"
    @todo Нужно ли реализовать аналогичный класс, основанный на линейном поиске?
    @todo Оптимизация пустого класса для target_
    @todo Покрыть тестами конструктор перемещний, операций присваивания и swap
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
    friend bool operator==(memoize_functor const & x, memoize_functor const & y)
    {
        return x.target() == y.target();
    }

    public:
        // Типы
        /// @brief Тип возвращаемого значения
        typedef R result_type;

        /// @brief Тип кортежа аргументов
        typedef ural::tuple<Args...> args_tuple;

        /// @brief Тип кэша
        typedef Map<args_tuple, result_type> Cache;

        /// @brief Тип размера кэша
        typedef typename Cache::size_type size_type;

        /// @brief Тип функционального объекта
        typedef decltype(ural::make_functor(std::declval<F>())) target_type;

        /// @brief Стратегия работы с многопоточьностью
        typedef typename default_helper<Threading, single_thread_policy>::type
            threading_policy;

        // Конструкторы
        /** @brief Конструктор
        @param f базовый функциональный объект
        @post <tt> this->target() == f </tt>
        */
        explicit memoize_functor(F f)
         : target_{std::move(f)}
         , cache_{}
         , mutex_{}
        {}

        /** @brief Конструктор копий
        @param x копируемый объект
        @post <tt> *this == x </tt>
        */
        memoize_functor(memoize_functor const & x)
         : target_{x.target()}
        {}

        // @todo Реализовать
        /** @brief Конструктор перемещения
        */
        memoize_functor(memoize_functor &&);

        /** @brief Оператор копирующего присваивания
        @return <tt> *this </tt>
        */
        memoize_functor & operator=(memoize_functor const &);

        /** @brief Оператор присваивания с перемещением
        @return <tt> *this </tt>
        */
        memoize_functor & operator=(memoize_functor &&);

        // Применение функционального объекта
        /** @brief Применение функционального объекта
        @param args аргументы
        @return Значение адаптируемого функционального объекта при данных
        аргументах.
        */
        template <class... As>
        result_type
        operator()(As &&... args) const
        {
            auto x = ural::forward_as_tuple(std::forward<As>(args)...);

            std::lock_guard<mutex_type> lock(this->mutex_);

            auto pos = this->cache_.find(x);

            if(pos == this->cache_.end())
            {
                auto y = this->target()(std::forward<As>(args)...);
                pos = cache_.emplace_hint(pos, std::move(x), std::move(y));
            }

            return pos->second;
        }

        // Свойства
        /** @brief Адаптируемый функциональный объект
        @return Адаптируемый функциональный объект
        */
        target_type const & target() const
        {
            return this->target_;
        }

        // Размер и ёмкость
        /** @brief Очистка кэша предыдущих вызовов
        @post Все вызовы оператора () будут приводит к вызову сохранённого
        функционального объекта
        */
        void clear_cache()
        {
            std::lock_guard<mutex_type> lock(this->mutex_);
            this->cache_.clear();
        }

        /** @brief Размер кэша
        @return Количество элементов, помещённых в кэш.
        */
        size_type cache_size() const
        {
            return cache_.size();
        }

    private:
        target_type target_;

        typedef typename threading_policy::mutex_type mutex_type;

        // @todo Можно ли объединить это (см. шаблон Монитор)?
        mutable Cache cache_;
        mutable mutex_type mutex_;
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
