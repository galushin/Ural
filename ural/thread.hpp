#ifndef Z_URAL_THREAD_HPP_INCLUDED
#define Z_URAL_THREAD_HPP_INCLUDED

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

/** @file ural/thread.hpp
 @brief Классы и функции для многопточного программирования
*/

#include <cstddef>

#include <atomic>
#include <mutex>

namespace ural
{
namespace experimental
{
    /** @brief Стратегия многопотоночти: многопоточная программа, использовать
    средства стандартной библиотеки C++11
    */
    class cpp11_thread_policy
    {
    public:
        /// @brief Тип потоково-безопасного счётчика
        typedef std::atomic<std::size_t> atomic_counter_type;

        /// @brief Тип мьютекса
        typedef std::mutex mutex_type;
    };

    /** @brief Класс, реализующий синтаксические требования к мьютексу, но не
    выполняющий никакой синхронизации
    */
    class single_thread_mutex
    {
    public:
        /// @brief Конструктор
        constexpr single_thread_mutex() noexcept = default;

        /// @brief Копирование запрещено
        single_thread_mutex(single_thread_mutex const &) = delete;

        /// @brief Копирующее присваивание запрещено
        single_thread_mutex&
        operator=(single_thread_mutex const &) = delete;

        /// @brief Блокировать
        void lock()
        {}

        /** @brief Попробовать заблокировать
        @return @b true
        */
        bool try_lock()
        {
            return true;
        }

        /// @brief Разблокировать
        void unlock()
        {}
    };

    /// @brief Стратегия многопотоночти: однопоточная программа
    class single_thread_policy
    {
    public:
        /// @brief Тип потоково-безопасного счётчика
        typedef std::size_t atomic_counter_type;

        /// @brief Тип мьютекса
        typedef single_thread_mutex mutex_type;
    };
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_THREAD_HPP_INCLUDED
