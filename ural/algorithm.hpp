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
 @todo Сгруппировать объявления переменных
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
            ural::c_rand_engine rnd;
            return ::ural::shuffle_fn{}(std::forward<RASequence>(s), rnd);
        }
    };

    inline namespace
    {
        // 25.2 Немодифицирующие
        // 25.2.1-3 Кванторы
        constexpr auto const all_of = all_of_fn{};
        constexpr auto const none_of = none_of_fn{};
        constexpr auto const any_of = any_of_fn{};

        // 25.2.4 for_each
        constexpr auto const for_each = for_each_fn{};

        // 25.2.5 Поиск
        constexpr auto const find = find_fn{};
        constexpr auto const find_if = find_if_fn{};
        constexpr auto const find_if_not = find_if_not_fn{};

        // 25.2.6 Поиск конца подпоследовательности (find_end)
        constexpr auto const find_end = find_end_fn{};

        // 25.2.7 Поиск первого вхождения
        constexpr auto const find_first_of = find_first_of_fn{};
        constexpr auto const find_first_not_of = find_first_not_of_fn{};

        // 25.2.8 Поиск соседних одинаковых элементов
        constexpr auto const adjacent_find = adjacent_find_fn{};

        // 25.2.9 Подсчёт
        constexpr auto const count = count_fn{};
        constexpr auto const count_if = count_if_fn{};

        // 25.2.10 Поиск несовпадения
        constexpr auto const mismatch = mismatch_fn{};

        // 25.2.11 Равенство
        constexpr auto const equal = equal_fn{};

        // 25.2.12 Являются ли две последовательности перестановками?
        constexpr auto const is_permutation = is_permutation_fn{};

        // 25.2.13 Поиск подпоследовательностей
        constexpr auto const search = search_fn{};
        constexpr auto const search_n = search_n_fn{};

        // 25.3 Модифицирующие алгоритмы
        // 25.3.1 Копирование
        constexpr auto const copy = copy_fn{};
        constexpr auto const copy_n = copy_n_fn{};
        constexpr auto const copy_if = copy_if_fn{};
        constexpr auto const copy_backward = copy_backward_fn{};

        // 25.3.2 Перемещение
        constexpr auto const move = move_fn{};
        constexpr auto const move_backward = move_backward_fn{};
        constexpr auto move_if_noexcept = move_if_noexcept_fn{};

        // 25.3.3 Обмен
        constexpr auto const swap_ranges = swap_ranges_fn{};

        // 25.3.4 Преобразование
        constexpr auto const transform = transform_fn{};

        // 25.3.5 Замена
        constexpr auto const replace = replace_fn{};
        constexpr auto const replace_if = replace_if_fn{};
        constexpr auto const replace_copy = replace_copy_fn{};
        constexpr auto const replace_copy_if = replace_copy_if_fn{};

        // 25.3.6 Заполнение
        constexpr auto const fill = fill_fn{};
        constexpr auto const fill_n = fill_n_fn{};

        // 25.3.7 Порождение
        constexpr auto const generate = generate_fn{};
        constexpr auto const generate_n = generate_n_fn{};

        // 25.3.8 Удаление
        constexpr auto const remove = remove_fn{};
        constexpr auto const remove_if = remove_if_fn{};
        constexpr auto const remove_copy = remove_copy_fn{};
        constexpr auto const remove_copy_if = remove_copy_if_fn{};

        // 25.3.9 Устранение последовательных дубликатов
        constexpr auto const unique = unique_fn{};
        constexpr auto const unique_copy = unique_copy_fn{};

        // 25.3.10 Обращение
        constexpr auto const reverse = reverse_fn{};
        constexpr auto const reverse_copy = reverse_copy_fn{};

        // 25.3.11 Вращение
        constexpr auto const rotate = rotate_fn{};
        constexpr auto const rotate_copy = rotate_copy_fn{};

        // 25.3.12 Тасовка
        constexpr auto const shuffle = shuffle_fn{};
        constexpr auto const random_shuffle = random_shuffle_fn{};

        // 25.3.13 Разделение
        constexpr auto const is_partitioned = is_partitioned_fn{};
        constexpr auto const partition = partition_fn{};
        constexpr auto const stable_partition = stable_partition_fn{};
        constexpr auto const partition_copy = partition_copy_fn{};
        // @todo partition_move
        constexpr auto const partition_point = partition_point_fn{};

        // 25.4 Сортировка и связанные с ним операции
        // 25.4.1 Сортировка
        // @todo объект для insertion_sort?

        // 25.4.1.1 Быстрая сортировка
        constexpr auto const sort = sort_fn{};

        // 25.4.1.2 Устойчивая сортировка
        constexpr auto const stable_sort = stable_sort_fn{};

        // 25.4.1.3 Частичная сортировка
        constexpr auto const partial_sort = partial_sort_fn{};

        // 25.4.1.4 Частичная сортировка с копированием
        constexpr auto const partial_sort_copy = partial_sort_copy_fn{};

        // 25.4.1.5 Проверка сортированности
        constexpr auto const is_sorted = is_sorted_fn{};
        constexpr auto const is_sorted_until = is_sorted_until_fn{};

        // 25.4.2 N-ый элемент
        constexpr auto const nth_element = nth_element_fn{};

        // 25.4.3 Бинарный поиск
        constexpr auto const lower_bound = lower_bound_fn{};
        constexpr auto const upper_bound = upper_bound_fn{};
        constexpr auto const equal_range = equal_range_fn{};
        constexpr auto const binary_search = binary_search_fn{};

        // 25.4.4 Слияние
        constexpr auto const merge = merge_fn{};
        // @todo merge_move
        constexpr auto const inplace_merge = inplace_merge_fn{};

        // 25.4.5 Операции с сортированными множествами
        constexpr auto const includes = includes_fn{};
        constexpr auto const set_union = set_union_fn{};
        constexpr auto const set_intersection = set_intersection_fn{};
        constexpr auto const set_difference = set_difference_fn{};
        constexpr auto const set_symmetric_difference = set_symmetric_difference_fn{};

        // 25.4.6 Операции с бинарными кучами
        constexpr auto const push_heap = push_heap_fn{};
        constexpr auto const pop_heap = pop_heap_fn{};
        constexpr auto const make_heap = make_heap_fn{};
        constexpr auto const sort_heap = sort_heap_fn{};
        constexpr auto const is_heap = is_heap_fn{};
        constexpr auto const is_heap_until = is_heap_until_fn{};
        constexpr auto const heap_select = heap_select_fn{};

        // 25.4.7 Наибольшее и наименьшее значение
        constexpr auto const min = min_fn{};
        constexpr auto const max = max_fn{};
        constexpr auto const minmax = minmax_fn{};
        constexpr auto const min_element = min_element_fn{};
        constexpr auto const max_element = max_element_fn{};
        constexpr auto const minmax_element = minmax_element_fn{};

        // 25.4.8 Лексикографическое сравнение
        constexpr auto const lexicographical_compare = lexicographical_compare_fn{};

        // 25.4.9 Порождение перестановка
        constexpr auto const next_permutation = next_permutation_fn{};
        constexpr auto const prev_permutation = prev_permutation_fn{};
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
