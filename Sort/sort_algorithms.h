#pragma once
#include <utility>
#include <iterator>
#include <functional>
#include <algorithm>

//buffer for merge sort
#include <set>

//heap_sort for non-RandomAccessIterator ranges
#include <set>

/*************************************************************************************************************
See https://academy.yandex.ru/posts/osnovnye-vidy-sortirovok-i-primery-ikh-realizatsii for simple examples
1. Bubble sort
2. Shaker sort
3. Comb sort
4. Insertion sort
5. Selection sort
6. Quick sort
7. Merge sort
8. Heap sort
**************************************************************************************************************/

namespace algo {
	namespace sort_details {
		template <class ForwardIt, class DifferenceType, class UnaryPredicate>
		void bubble_sort_impl(ForwardIt first, ForwardIt last, DifferenceType distance, UnaryPredicate&& pred) {
			using difference_type = typename std::iterator_traits<ForwardIt>::difference_type;
			for (difference_type idx = 0; idx + 1 < distance; ++idx) {
				ForwardIt last_in_pass{ first };
				std::advance(last_in_pass, distance - idx);						//Move the iterator to the final position for the current pass
				for (ForwardIt it = first; std::next(it) != last_in_pass; it = std::next(it)) {
					if (pred(*std::next(it), *it)) {							//If out of order
						std::iter_swap(it, std::next(it));
					}
				}
			}
		}
	}

	template <class ForwardIt, class UnaryPredicate>
	void bubble_sort(ForwardIt first, ForwardIt last, UnaryPredicate pred) {		
		bubble_sort_impl(first, last, std::distance(first, last), pred);
	}

	template <class ForwardIt>
	void bubble_sort(ForwardIt first, ForwardIt last) {
		bubble_sort(first, last, std::less<>{});
	}

	namespace sort_details {
		template <class BidirectionalIt, class UnaryPredicate>
		void shaker_pass_left(BidirectionalIt left, BidirectionalIt right, UnaryPredicate& pred) {
			for (; left != right; left = std::next(left)) {
				if (pred(*std::next(left), *left)) {
					std::iter_swap(left, std::next(left));
				}
			}
		}

		template <class BidirectionalIt, class UnaryPredicate>
		void shaker_pass_right(BidirectionalIt left, BidirectionalIt right, UnaryPredicate& pred) {
			for (; right != left; right = std::prev(right)) {
				if (pred(*right, *std::prev(right))) {
					std::iter_swap(right, std::prev(right));
				}
			}
		}

		template <class BidirectionalIt, class UnaryPredicate>
		std::enable_if_t<std::negation_v<
				std::is_same<typename BidirectionalIt::iterator_category, std::random_access_iterator_tag>
		>, void> shaker_sort_impl(BidirectionalIt first, BidirectionalIt before_last, UnaryPredicate&& pred) {	//sort range [first; before_last]
			while (first != before_last) {		
				shaker_pass_left(first, before_last, pred);
				before_last = std::prev(before_last);				//The right element is already in his place		
				shaker_pass_right(first, before_last, pred);
				if (first != before_last) {							//operator<=() may not be defined
					first = std::next(first);						//The left element is already in his place			
				}			
			}
			if (pred(*before_last, *first)) {
				std::iter_swap(first, before_last);
			}
		}

		template <class RandomIt, class UnaryPredicate>
		std::enable_if_t<std::is_same_v<typename RandomIt::iterator_category, std::random_access_iterator_tag>,
			void> shaker_sort_impl(RandomIt first, RandomIt before_last, UnaryPredicate&& pred) {	//sort range [first; before_last]
			while (first <= before_last) {
				shaker_pass_left(first, before_last, pred);
				before_last = std::prev(before_last);				//The right element is already in his place				
				shaker_pass_right(first, before_last, pred);
				first = std::next(first);							//The left element is already in his place
			}
		}
	}

	template <class BidirectionalIt, class UnaryPredicate>
	void shaker_sort(BidirectionalIt first, BidirectionalIt last, UnaryPredicate pred) {
		if (first != last) {
			sort_details::shaker_sort_impl(first, std::prev(last), pred);
		}
	}

	template <class BidirectionalIt>
	void shaker_sort(BidirectionalIt first, BidirectionalIt last) {
		shaker_sort(first, last, std::less<>{});
	}

	template <class ForwardIt, class UnaryPredicate>
	void comb_sort(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
		static constexpr double factor{ 1.247 };											//Decreasing factor

		using difference_type = typename std::iterator_traits<ForwardIt>::difference_type;
		const difference_type distance{ std::distance(first, last) };

		for (double step = static_cast<double>(distance); step >= 1; step /= factor) {	
			ForwardIt pass_end{ last };
			std::advance(pass_end, -static_cast<difference_type>(step));					//Move to the beginning
			for (ForwardIt left = first; left != pass_end; left = std::next(left)) {
				ForwardIt right{ left };
				std::advance(right, static_cast<difference_type>(step));
				if (pred(*std::next(left), *right)) {
					std::iter_swap(left, right);
				}
			}
		}
		sort_details::bubble_sort_impl(first, last, distance, pred);
	}

	template <class ForwardIt>
	void comb_sort(ForwardIt first, ForwardIt last) {
		comb_sort(first, last, std::less<>{});
	}

	template <class BidirectionalIt, class UnaryPredicate>
	void insertion_sort(BidirectionalIt first, BidirectionalIt last, UnaryPredicate pred) {
		for (BidirectionalIt right = first; right != last; right = std::next(right)) {
			BidirectionalIt left{ right };					
			while(left != first && pred(*right, *std::prev(left))) {	//Shift to the beginning of the range where the element should be placed
				left = std::prev(left);
			}															//By the end of the cycle, the left will be in the correct position in [first; right)
			std::rotate(left, right, std::next(right));					//Cyclic shift: {left, ..., right, last} => {right, left, ..., prev(right), last}
		}
	}

	template <class BidirectionalIt>
	void insertion_sort(BidirectionalIt first, BidirectionalIt last) {
		insertion_sort(first, last, std::less<>{});
	}

	namespace sort_details {
		template <class InputIt, class UnaryPredicate>					//Finding the largest/smallest/etc. element using the predicate
		InputIt find_most_suitable_element(InputIt first, InputIt last, UnaryPredicate&& pred) {	
			InputIt result{ first };
			for (; first != last; first = std::next(first)) {
				if (pred(*first, *result)) {
					result = first;
				}
			}
			return result;
		}
	}

	template <class BidirectionalIt, class UnaryPredicate>
	void selection_sort(BidirectionalIt first, BidirectionalIt last, UnaryPredicate pred) {
		for (; first != last; first = std::next(first)) {
			BidirectionalIt most_suitable_elem{ sort_details::find_most_suitable_element(first, last, pred) };
			std::iter_swap(first, most_suitable_elem);
		}
	}

	template <class BidirectionalIt>
	void selection_sort(BidirectionalIt first, BidirectionalIt last) {
		selection_sort(first, last, std::less<>{});
	}

	namespace sort_details {
		template <class BidirectionalIt, class UnaryPredicate>
		BidirectionalIt make_partition(BidirectionalIt first, BidirectionalIt before_last, UnaryPredicate&& pred) {
			BidirectionalIt less{ first };
			for (; first != before_last; first = std::next(first)) {
				if (!pred(*before_last, *first)) {						//If predicate is std::less, for example: "first <= before_last"
					std::iter_swap(first, less);
					less = std::next(less);
				}
			}
			std::iter_swap(less, before_last);
			return less;
		}

		template <class BidirectionalIt, class UnaryPredicate>
		void quick_sort_impl(BidirectionalIt first, BidirectionalIt before_last, UnaryPredicate&& pred) {
			if (first != before_last) {									//operator<() may not be defined
				BidirectionalIt pivot{ make_partition(first, before_last, pred) };
				if (pivot != first) {
					quick_sort_impl(first, std::prev(pivot), pred);
				}
				if (pivot != before_last) {
					quick_sort_impl(std::next(pivot), before_last, pred);
				}
			}
		}
	}

	template <class BidirectionalIt, class UnaryPredicate>		//
	void quick_sort(BidirectionalIt first, BidirectionalIt last, UnaryPredicate pred) {
		if (first != last) {
			sort_details::quick_sort_impl(first, std::prev(last), pred);
		}
	}

	template <class BidirectionalIt>	
	void quick_sort(BidirectionalIt first, BidirectionalIt last) {
		quick_sort(first, last, std::less<>{});
	}

	namespace sort_details {
		template <class ForwardIt, class LinearBuffer, class UnaryPredicate>
		void merge_sort_impl(
			ForwardIt first, ForwardIt before_last,
			LinearBuffer& buffer,
			std::pair<size_t, size_t> range_in_buffer,					//For objects sizeof()<= 16 bytes pass by value is more efficient
			UnaryPredicate pred
		) {
			if (first != before_last) {																
				const auto half_distance{(range_in_buffer.second - range_in_buffer.first) / 2 };	
				ForwardIt middle{ first };
				std::advance(middle, half_distance);

				merge_sort_impl(
					first, 
					middle,
					buffer, 
					make_pair(range_in_buffer.first, range_in_buffer.first + half_distance),
					pred
				);
				merge_sort_impl(
					std::next(middle),
					before_last,
					buffer,
					make_pair(range_in_buffer.first + half_distance + 1, range_in_buffer.second),
					pred
				);

				std::merge(
					first,																	//first subrange begin
					std::next(middle),														//first subrange end
					std::next(middle),														//second subrange begin
				    std::next(before_last),													//second subrange end
					buffer.begin() + range_in_buffer.first,
					pred																	//Uses a predicate for correct merge
				);
				std::move(																	//Move the sorted subrange back to the original
					std::make_move_iterator(buffer.begin() + range_in_buffer.first),
					std::make_move_iterator(buffer.begin() + range_in_buffer.second + 1),//range_in_buffer.second - position of the last element of the subrange in buffer
					first
				);
			}
		}

		template <class ForwardIt, class LinearBuffer, class UnaryPredicate>
		void in_place_merge_sort_impl(ForwardIt first, ForwardIt before_last, UnaryPredicate pred) {

		}
	}

	template <class ForwardIt, class UnaryPredicate>
	std::enable_if_t< 
		std::is_default_constructible_v<typename std::iterator_traits<ForwardIt>::value_type>,//To initialize the buffer objects must have a default constructor 
		void> merge_sort(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
		if (first != last) {
			using value_type = typename std::iterator_traits<ForwardIt>::value_type;
			std::vector<value_type> buffer;
			const auto distance{ std::distance(first, last) };
			buffer.resize(distance);
			sort_details::merge_sort_impl(
				first,
				std::prev(last),
				buffer,
				std::make_pair(static_cast<size_t>(0), buffer.size() - 1),
				pred
			);
		}
	}

	template <class ForwardIt, class UnaryPredicate>
	std::enable_if_t<
		std::negation_v<
		std::is_default_constructible<typename std::iterator_traits<ForwardIt>::value_type	
		>>, void> merge_sort(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
		static_assert(false, "Will be implemented in the future")
	}

	template <class ForwardIt>
	void merge_sort(ForwardIt first, ForwardIt last) {
		merge_sort(first, last, std::less<>{});
	}
	
	namespace sort_details{
		template <class RandomIt, class UnaryPredicate>
		std::enable_if_t<std::is_same_v<typename RandomIt::iterator_category, std::random_access_iterator_tag>,
			void> heap_sort_impl(RandomIt first, RandomIt last, UnaryPredicate&& pred) {
			std::make_heap(first, last, pred);
			for (RandomIt end = last; end != first; end = std::prev(end)) {
				std::pop_heap(first, end);
			}
		}

		template <class Set, class OutputIt> 
		OutputIt extract_from_set(Set& set, OutputIt d_first) {
			while (!set.empty()) {
				auto node{ set.extract(set.begin()) };
				*d_first++ = std::move(node.value());
			}
			return d_first;
		}

		template <class ForwardIt, class UnaryPredicate>
		std::enable_if_t<
			std::negation_v<std::is_same<typename ForwardIt::iterator_category, std::random_access_iterator_tag>
			>, void> heap_sort_impl(ForwardIt first, ForwardIt last, UnaryPredicate&& pred) {
			using value_type = typename std::iterator_traits<ForwardIt>::value_type;
			std::multiset<													//Range can contain duplicate values
				value_type, UnaryPredicate
			> heap(pred);													//multiset has costructor only with const UnaryPredicate& :(
			for (ForwardIt target = first; target != last; target = std::next(target)) {
				heap.insert(std::move_if_noexcept(*target));				//If an exception is thrown the original range will remain valid
			}
			extract_from_set(heap, first);
		}
	}

	template <class ForwardIt, class UnaryPredicate>
	void heap_sort(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
		sort_details::heap_sort_impl(first, last, pred);
	}

	template <class ForwardIt>
	void heap_sort(ForwardIt first, ForwardIt last) {
		heap_sort(first, last, std::less<>{});
	}
}