#ifndef Z_URAL_ALGORITHM_HPP_INCLUDED
#define Z_URAL_ALGORITHM_HPP_INCLUDED

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

/** @file ural/algorithm.hpp
 @brief Обобщённые алгоритмы

 @todo Бинарный поиск: ограничения на функцию сравнения и значение
 @todo Проверка концепций + изменение имён в алгоритмах вида "*_n"
 @todo Проверить возможность замены ForwardSequence на OutputSequence
 @todo Определить типы возврщаемых значений как в Range extensions
 @todo устранить дублирование в алгортмах за счёт CRTP/Фасадов
*/

/** @defgroup Algorithms Алгоритмы
 @brief Обобщённые операции над последовательностями.
*/

#include <ural/algorithm/container.hpp>

#include <ural/algorithm/sorting.hpp>
#include <ural/algorithm/mutating.hpp>
#include <ural/algorithm/non_modifying.hpp>

#include <ural/algorithm/core.hpp>

#include <ural/sequence/make.hpp>
#include <ural/functional/make_callable.hpp>
#include <ural/random/c_rand_engine.hpp>

#include <cassert>

namespace ural
{
namespace experimental
{
    // Модифицирующие последовательность алгоритмы
    class random_shuffle_fn
    {
    public:
        /** @brief Случайная тасовка элементов последовательности
        @param s последовательность произвольного доступа
        @return Последовательность, полученная из
        <tt> ::ural::sequence_fwd<RASequence>(s) </tt> продвижением до
        исчерпания.
        */
        template <class RASequence>
        auto operator()(RASequence && s) const
        -> decltype(::ural::sequence_fwd<RASequence>(s))
        {
            ::ural::experimental::c_rand_engine rnd;
            return ::ural::shuffle_fn{}(std::forward<RASequence>(s), rnd);
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта, который применяет заданный
    функциональный объект к последовательности кортежей, рассматривая каждый
    элемент кортежа как отдельный аргумент функции.
    */
    class fused_for_each_fn
    {
    public:
        /** @brief Применяет функциональный объект к последовательности
        кортежей, рассматривая каждый элемент кортежа как отдельный аргумент
        функции.
        @param in входная последовательность
        @param f функциональный объект
        @return Кортеж, первый компонент которого получается продвижением
        <tt> ::ural::sequence_fwd<Input>(in) </tt> до исчерпания, а второй
        --- <tt> ::ural::make_callable(std::move(f)) </tt> после его применения
        ко всем элементам последовательности.
        */
        template <class Input, class Function>
        tuple<SequenceType<Input>, FunctionType<Function>>
        operator()(Input && in, Function f) const
        {
            auto f_app = curry(apply, make_callable(std::move(f)));

            auto result = for_each_fn{}(std::forward<Input>(in), f_app);

            return make_tuple(std::move(result)[ural::_1],
                              std::move(result)[ural::_2].argument());
        }
    };

    /** @brief Тип функционального объекта, проверяющего сбалансированность
    открывающихся и закрывающихся скобок в последовательности.
    */
    class balanced_parens_fn
    {
    public:
        /** @brief Проверка того, что скобки в последовательности сбалансированы
        @param input входная последовательность
        @param left_par открывающаяся скобка
        @param right_par закрывающаяся скобка
        @return @b true, если скобки в последовательности сбалансированы,
        иначе --- @b false
        */
        template <class InputSequenced, class T>
        bool
        operator()(InputSequenced && input,
                   T const & left_par, T const & right_par) const
        {
            using D = DifferenceType<SequenceType<InputSequenced>>;
            return (*this)(std::forward<InputSequenced>(input),
                           left_par, right_par,
                           std::numeric_limits<D>::max());
        }

        /** @brief Проверка того, что скобки в последовательности
        сбалансированы, причём вложенность скобок не превышает заданную.
        @param input входная последовательность
        @param left_par открывающаяся скобка
        @param right_par закрывающаяся скобка
        @param max_nesting_level наибольшая допустимая глубина вложения
        скобок
        @return @b true, если скобки в последовательности сбалансированы, а
        их максимальная вложенность не превышает @c max_nesting_level, иначе ---
        @b false
        */
        template <class InputSequenced, class T>
        bool
        operator()(InputSequenced && input,
                   T const & left_par, T const & right_par,
                   DifferenceType<SequenceType<InputSequenced>> max_nesting_level) const
        {
            return this->impl(ural::sequence_fwd<InputSequenced>(input),
                              left_par, right_par, max_nesting_level);
        }
    private:
        template <class InputSequence, class T>
        static bool impl(InputSequence in,
                         T const & left_par, T const & right_par,
                         DifferenceType<InputSequence> const max_nesting_level)
        {
            using namespace ural::concepts;

            BOOST_CONCEPT_ASSERT((SinglePassSequence<InputSequence>));
            BOOST_CONCEPT_ASSERT((ReadableSequence<InputSequence>));
            BOOST_CONCEPT_ASSERT((IndirectPredicate<ural::equal_to<>,
                                                    InputSequence,
                                                    T const *>));

            auto const zero = DifferenceType<InputSequence>{0};

            auto opened = zero;

            for(; !!in; ++ in)
            {
                if(*in == left_par)
                {
                    if(opened > max_nesting_level)
                    {
                        return false;
                    }

                    ++ opened;
                }
                else if(*in == right_par)
                {
                    if(opened == zero)
                    {
                        return false;
                    }

                    -- opened;
                }
            }

            return opened == zero;
        }
    };

    /** @brief Тип функционального объекта, определяющего наименьший элемент
    последовательности и количество его вхождений.
    */
    class min_count_fn
    {
    public:
        /** @brief Определение наименьшего значения и количества его вхождений
        в последовательности.
        @param in входная последовательность
        @param cmp фукнция сравнения
        @return Возвращает кортеж, первый элемент которого равен наименьшему
        значению в последовательности, а второй --- количество вхождений данного
        значения в последовательности.
        */
        template <class InputSequenced, class Compare = ural::less<>>
        tuple<ValueType<SequenceType<InputSequenced>>,
              DifferenceType<SequenceType<InputSequenced>>>
        operator()(InputSequenced && in, Compare cmp = Compare{}) const
        {
            return this->impl(ural::sequence_fwd<InputSequenced>(in),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class InputSequence, class Compare>
        tuple<ValueType<InputSequence>, DifferenceType<InputSequence>>
        impl(InputSequence in, Compare cmp = Compare{}) const
        {
            const auto unit = DifferenceType<InputSequence>(1);
            assert(!!in);

            tuple<ValueType<InputSequence>, DifferenceType<InputSequence>>
                result{*in, unit};
            ++ in;

            for(; !!in; ++ in)
            {
                if(cmp(*in, result[ural::_1]))
                {
                    result[ural::_1] = *in;
                    result[ural::_2] = unit;
                }
                else if(!cmp(result[ural::_1], *in))
                {
                    ++ result[ural::_2];
                }
            }

            return result;
        }
    };

    /** @brief Тип функционального объекта, пропускающий начало одной
    последовательности, если она совпадает с другой последовательностью.
    */
    class skip_over_fn
    {
    public:
        /** @brief Если <tt> stars_with(seq, prefix, bin_pred) </tt>, то
        продвигает @c seq на <tt> size(prefix) </tt> элементов.
        @param seq последовательность, которую предстоит продвинуть.
        @param prefix последовательность, которую нужно пропустить.
        @return То же, что <tt> stars_with(seq, prefix, bin_pred) </tt>.
        */
        template <class ForwardSequence, class InputSequenced,
                  class BinaryPredicate = equal_to<>>
        bool operator()(ForwardSequence & seq, InputSequenced && prefix,
                        BinaryPredicate bin_pred = BinaryPredicate{}) const
        {
            auto res = ural::mismatch_fn{}(seq, std::forward<InputSequenced>(prefix),
                                           std::move(bin_pred));

            if(!res[ural::_2])
            {
                seq = res[ural::_1];
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    /** @brief Тип функционального объекта, определяющий наибольший общий
    префикс двух последовательностей.
    */
    class common_prefix_fn
    {
    public:
        /** @brief Поиск общего префикса @c s1 и @c s2
        @param s1 первая последовательность (должна быть прямой)
        @param s2 вторая последовательность
        @param bin_pred бинарный предикат, задающий функцию сравнения
        @return Часть @c s1, которая является префиксом для @c s2.
        */
        template <class ForwardSequenced, class InputSequenced,
                  class BinaryPredicate = ural::equal_to<>>
        TraversedFrontType<SequenceType<ForwardSequenced>>
        operator()(ForwardSequenced && s1, InputSequenced && s2,
                   BinaryPredicate bin_pred = BinaryPredicate{}) const
        {
            auto r = ural::mismatch_fn{}(std::forward<ForwardSequenced>(s1),
                                         std::forward<InputSequenced>(s2),
                                         std::move(bin_pred));
            return std::move(r)[ural::_1].traversed_front();
        }
    };
}
// namespace experimental

inline namespace v0
{
namespace
{
    // 25.2 Немодифицирующие
    // 25.2.1-3 Кванторы
    constexpr auto const & all_of = odr_const<all_of_fn>;
    constexpr auto const & none_of = odr_const<none_of_fn>;
    constexpr auto const & any_of = odr_const<any_of_fn>;

    // 25.2.4 for_each
    constexpr auto const & for_each = odr_const<for_each_fn>;

    // 25.2.5 Поиск
    constexpr auto const & find = odr_const<find_fn>;
    constexpr auto const & find_if = odr_const<find_if_fn>;
    constexpr auto const & find_if_not = odr_const<find_if_not_fn>;

    // 25.2.6 Поиск конца подпоследовательности (find_end)
    constexpr auto const & find_end = odr_const<find_end_fn>;

    // 25.2.7 Поиск первого вхождения
    constexpr auto const & find_first_of = odr_const<find_first_of_fn>;
    constexpr auto const & find_first_not_of = odr_const<find_first_not_of_fn>;

    // 25.2.8 Поиск соседних одинаковых элементов
    constexpr auto const & adjacent_find = odr_const<adjacent_find_fn>;

    // 25.2.9 Подсчёт
    constexpr auto const & count = odr_const<count_fn>;
    constexpr auto const & count_if = odr_const<count_if_fn>;

    // 25.2.10 Поиск несовпадения
    constexpr auto const & mismatch = odr_const<mismatch_fn>;

    // 25.2.11 Равенство
    constexpr auto const & equal = odr_const<equal_fn>;

    // 25.2.12 Являются ли две последовательности перестановками?
    constexpr auto const & is_permutation = odr_const<is_permutation_fn>;

    // 25.2.13 Поиск подпоследовательностей
    constexpr auto const & search = odr_const<search_fn>;
    constexpr auto const & search_n = odr_const<search_n_fn>;

    // 25.3 Модифицирующие алгоритмы
    // 25.3.1 Копирование
    constexpr auto const & copy = odr_const<copy_fn>;
    constexpr auto const & copy_n = odr_const<copy_n_fn>;
    constexpr auto const & copy_if = odr_const<copy_if_fn>;
    constexpr auto const & copy_backward = odr_const<copy_backward_fn>;

    // 25.3.2 Перемещение
    constexpr auto const & move = odr_const<move_fn>;
    constexpr auto const & move_backward = odr_const_holder<move_backward_fn>::value;
    constexpr auto const & move_if_noexcept = odr_const_holder<move_if_noexcept_fn>::value;

    // 25.3.3 Обмен
    constexpr auto const & swap_ranges = odr_const<swap_ranges_fn>;

    // 25.3.4 Преобразование
    constexpr auto const & transform = odr_const<transform_fn>;

    // 25.3.5 Замена
    constexpr auto const & replace = odr_const<replace_fn>;
    constexpr auto const & replace_if = odr_const<replace_if_fn>;
    constexpr auto const & replace_copy = odr_const<replace_copy_fn>;
    constexpr auto const & replace_copy_if = odr_const<replace_copy_if_fn>;

    // 25.3.6 Заполнение
    constexpr auto const & fill = odr_const<fill_fn>;
    constexpr auto const & fill_n = odr_const<fill_n_fn>;

    // 25.3.7 Порождение
    constexpr auto const & generate = odr_const<generate_fn>;
    constexpr auto const & generate_n = odr_const<generate_n_fn>;

    // 25.3.8 Удаление
    constexpr auto const & remove = odr_const<remove_fn>;
    constexpr auto const & remove_if = odr_const<remove_if_fn>;
    constexpr auto const & remove_copy = odr_const<remove_copy_fn>;
    constexpr auto const & remove_copy_if = odr_const<remove_copy_if_fn>;

    // 25.3.9 Устранение последовательных дубликатов
    constexpr auto const & unique = odr_const<unique_fn>;
    constexpr auto const & unique_copy = odr_const_holder<unique_copy_fn>::value;

    // 25.3.10 Обращение
    constexpr auto const & reverse = odr_const_holder<reverse_fn>::value;
    constexpr auto const & reverse_copy = odr_const<reverse_copy_fn>;

    // 25.3.11 Вращение
    constexpr auto const & rotate = odr_const_holder<rotate_fn>::value;
    constexpr auto const & rotate_copy = odr_const<rotate_copy_fn>;

    // 25.3.12 Тасовка
    constexpr auto const & shuffle = odr_const<shuffle_fn>;

    // 25.3.13 Разделение
    constexpr auto const & is_partitioned = odr_const<is_partitioned_fn>;
    constexpr auto const & partition = odr_const<partition_fn>;
    constexpr auto const & stable_partition = odr_const<stable_partition_fn>;
    constexpr auto const & partition_copy = odr_const<partition_copy_fn>;
    constexpr auto const & partition_point = odr_const<partition_point_fn>;

    // 25.4 Сортировка и связанные с ним операции
    // 25.4.1 Сортировка
    // @todo объект для insertion_sort?

    // 25.4.1.1 Быстрая сортировка
    constexpr auto const & sort = odr_const<sort_fn>;

    // 25.4.1.2 Устойчивая сортировка
    constexpr auto const & stable_sort = odr_const<stable_sort_fn>;

    // 25.4.1.3 Частичная сортировка
    constexpr auto const & partial_sort = odr_const<partial_sort_fn>;

    // 25.4.1.4 Частичная сортировка с копированием
    constexpr auto const & partial_sort_copy = odr_const<partial_sort_copy_fn>;

    // 25.4.1.5 Проверка сортированности
    constexpr auto const & is_sorted = odr_const<is_sorted_fn>;
    constexpr auto const & is_sorted_until = odr_const<is_sorted_until_fn>;

    // 25.4.2 N-ый элемент
    constexpr auto const & nth_element = odr_const<nth_element_fn>;

    // 25.4.3 Бинарный поиск
    constexpr auto const & lower_bound = odr_const<lower_bound_fn>;
    constexpr auto const & upper_bound = odr_const<upper_bound_fn>;
    constexpr auto const & equal_range = odr_const<equal_range_fn>;
    constexpr auto const & binary_search = odr_const<binary_search_fn>;

    // 25.4.4 Слияние
    constexpr auto const & merge = odr_const_holder<merge_fn>::value;
    constexpr auto const & inplace_merge = odr_const_holder<inplace_merge_fn>::value;

    // 25.4.5 Операции с сортированными множествами
    constexpr auto const & includes = odr_const<includes_fn>;
    constexpr auto const & set_union = odr_const_holder<set_union_fn>::value;
    constexpr auto const & set_intersection = odr_const_holder<set_intersection_fn>::value;
    constexpr auto const & set_difference = odr_const_holder<set_difference_fn>::value;
    constexpr auto const & set_symmetric_difference = odr_const_holder<set_symmetric_difference_fn>::value;

    // 25.4.6 Операции с бинарными кучами
    constexpr auto const & push_heap = odr_const<push_heap_fn>;
    constexpr auto const & pop_heap = odr_const<pop_heap_fn>;
    constexpr auto const & make_heap = odr_const<make_heap_fn>;
    constexpr auto const & sort_heap = odr_const<sort_heap_fn>;
    constexpr auto const & is_heap = odr_const<is_heap_fn>;
    constexpr auto const & is_heap_until = odr_const<is_heap_until_fn>;
    constexpr auto const & heap_select = odr_const<heap_select_fn>;

    // 25.4.7 Наибольшее и наименьшее значение
    constexpr auto const & min = odr_const<min_fn>;
    constexpr auto const & max = odr_const<max_fn>;
    constexpr auto const & minmax = odr_const<minmax_fn>;
    constexpr auto const & min_element = odr_const<min_element_fn>;
    constexpr auto const & max_element = odr_const<max_element_fn>;
    constexpr auto const & minmax_element = odr_const<minmax_element_fn>;

    // 25.4.8 Лексикографическое сравнение
    constexpr auto const & lexicographical_compare = odr_const<lexicographical_compare_fn>;

    // 25.4.9 Порождение перестановка
    constexpr auto const & next_permutation = odr_const<next_permutation_fn>;
    constexpr auto const & prev_permutation = odr_const<prev_permutation_fn>;
}
// namespace
}
// namespace v0

namespace experimental
{
namespace
{
    constexpr auto const & random_shuffle = odr_const<random_shuffle_fn>;

    constexpr auto const & fused_for_each = odr_const<fused_for_each_fn>;

    /** @brief Функциональный объект, проверяющий сбалансированность
    открывающихся и закрывающихся скобок в последовательности.
    */
    constexpr auto const & balanced_parens = odr_const<balanced_parens_fn>;

    /** @brief Функциональный объект, определяющий наименьший элемент
    последовательности и количество его вхождений.
    */
    constexpr auto const & min_count = odr_const<min_count_fn>;

    /** @brief Функциональный объект, пропускающий начало одной
    последовательности, если она совпадает с другой последовательностью.
    */
    constexpr auto const & skip_over = odr_const<skip_over_fn>;

    /** @brief Функциональный объект, определяющий наибольший общий префикс
    двух последовательностей.
    */
    constexpr auto const & common_prefix = odr_const<common_prefix_fn>;
}
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
