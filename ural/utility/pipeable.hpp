#ifndef Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED
#define Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED

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

/** @file ural/utility/pipeable.hpp
 @brief Вспомогательные возможности для создания последовательностей в
 конвейерном стиле
*/

#include <ural/functional.hpp>
#include <ural/concepts.hpp>
#include <ural/utility.hpp>

#include <functional>
#include <utility>
#include <tuple>

namespace ural
{
namespace experimental
{
    /** @brief Элемент "конвейера"
    @param Factory тип функции создания
    */
    template <class Factory>
    class pipeable
     : private function_type_t<Factory>
    {
        using Base = function_type_t<Factory>;

    public:
        /** @brief Конструктор без аргументов
        @post <tt> this->function() == function_type_t<Factory>{} </tt>
        */
        constexpr explicit pipeable() = default;

        /** @brief Конструктор
        @param f функция
        @post <tt> this->function() == f </tt>
        */
        constexpr explicit pipeable(Factory f)
         : Base(std::move(f))
        {}

        //@{
        /** @brief Используемый функциональный объект
        @return Константная ссылка на используемый функциональный объект
        */
        function_type_t<Factory> const & function() const &
        {
            return *this;
        }

        function_type_t<Factory> && function() &&
        {
            return static_cast<function_type_t<Factory> &&>(*this);
        }
        //@}
    };

    namespace details
    {
        template <class T>
        struct is_pipeable_impl
        {
        private:
            template <class U>
            static std::true_type test(pipeable<U>);

            static std::false_type test(...);

        public:
            using type = decltype(is_pipeable_impl::test(std::declval<T>()));
        };
    }
    // namespace details

    /** @brief Класс-характеристика для проверки того, что тип является
    результатом инстанцирования шаблона @c pipeable
    @tparam T тип, для которого выполняется проверка
    */
    template <class T>
    struct is_pipeable
     : public ::ural::experimental::details::is_pipeable_impl<T>::type
    {};

    /** @brief Оператор создания адаптора
    @param seq исходная последовательность
    @param pipe объект, содержащий аргументы для создания адаптора
    @return <tt> ::ural::apply(f, std::move(pipe.args)) </tt>, где
    @c f --- <tt> ::ural::curry(Factory{}, std::forward<Sequence>(seq))</tt>
    */
    template <class Sequence, class Factory,
              class = typename std::enable_if<!is_pipeable<Sequence>::value>::type>
    auto operator|(Sequence && seq, pipeable<Factory> pipe)
    -> decltype(std::declval<Factory>()(std::forward<Sequence>(seq)))
    {
        return pipe.function()(std::forward<Sequence>(seq));
    }

    /** @brief Композиция функций создания адапторов в конвейерном стиле
    @param p1, p2 объекты, предназначенные для создания адапторов в конвейерном
    стиле
    @return Объект @c p такой, что <tt> seq | p </tt> эквивалентно
    <tt> seq | p1 | p2 </tt>.
    */
    template <class F1, class F2>
    pipeable<::ural::experimental::compose_function<F2, F1>>
    operator|(pipeable<F1> p1, pipeable<F2> p2)
    {
        using F = ::ural::experimental::compose_function<F2, F1>;
        return pipeable<F>(F(std::move(p2).function(),
                             std::move(p1).function()));
    }

    /** @brief Вспомогательный класс для создания функциональных объектов,
    результаты которых можно использовать как этапы "конвейерного" создания
    последовательностей.
    @tparam Factory тип функционального объекта
    */
    template <class Factory>
    class pipeable_maker
    {
        template <class... Args>
        struct pipeable_binder
        {
        public:
            explicit pipeable_binder(Factory f, Args && ... args)
             : f_(std::move(f))
             , args_(std::forward_as_tuple(std::forward<Args>(args)...))
            {}

            template <class Sequence>
            auto operator()(Sequence && seq) const
            {
                auto f = ::ural::experimental::curry(this->f_, std::forward<Sequence>(seq));
                return ::ural::apply(std::move(f), args_);
            }

        private:
            Factory f_;
            std::tuple<Args...> args_;
        };

        template <class... Args>
        pipeable_binder<Args...>
        pipeable_bind(Args &&... args) const
        {
            return pipeable_binder<Args...>(this->factory_, std::forward<Args>(args)...);
        }

    public:
        pipeable_maker() = default;

        explicit pipeable_maker(Factory f)
         : factory_(std::move(f))
        {}

        /** @brief Оператор вызова функции
        @param args список аргументов
        */
        template <class... Args>
        auto operator()(Args &&... args) const
        {
            auto f = this->pipeable_bind(std::forward<Args>(args)...);
            return pipeable<decltype(f)>(std::move(f));
        }

    private:
        Factory factory_;
    };
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED
