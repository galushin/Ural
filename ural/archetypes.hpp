#ifndef Z_URAL_ARCHETYPES_HPP_INCLUDED
#define Z_URAL_ARCHETYPES_HPP_INCLUDED

/** @file ural/archetypes.hpp
 @brief Архетипы
*/

#include <ural/sequence/base.hpp>

namespace ural
{
namespace archetypes
{
    /** @brief Архетип функционального объекта (остаётся неопределённым)
    @tparam Signature сигнатура
    */
    template <class Signature>
    class functor;

    /** @brief Архетип функционального объекта (общее определение)
    @tparam R типы аргументов
    @tparam Args типы аргументов
    */
    template <class R, class... Args>
    class functor<R(Args...)>
    {
    public:
        R operator()(Args...) const
        {
            return instance();
        }

    private:
        static R instance()
        {
            static R r;
            return r;
        }
    };

    /** @brief Архетип функционального объекта без возвращаемого значения
    @tparam Args типы аргументов
    */
    template <class... Args>
    class functor<void(Args...)>
    {
    public:
        void operator()(Args...) const
        {}
    };

    /** @brief Архетип последовательности ввода
    @tparam Value тип значения
    */
    template <class Value>
    class input_sequence
     : public ural::sequence_base<input_sequence<Value>>
    {
    public:
        /// @brief Тип значения
        typedef Value value_type;

        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип для представления расстояния
        typedef size_t distance_type;

        /// @brief Тэг категории обхода
        typedef single_pass_traversal_tag traversal_tag;

        /** @brief Проверка исчерпания последовательности
        @return @b true
        */
        bool operator!() const
        {
            return true;
        }

        /** @brief Первый элемент последовательности
        @return Ссылка на первый элемент последовательности
        */
        reference front() const
        {
            return this->value();
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {}

    private:
        static reference value()
        {
            static value_type x;
            return x;
        }
    };
}
// namespace archetypes
}
// namespace ural

#endif
// Z_URAL_ARCHETYPES_HPP_INCLUDED
