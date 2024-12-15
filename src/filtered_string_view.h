#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <algorithm>
#include <compare>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>

namespace fsv {
	using filter = std::function<bool(const char&)>;
	class filtered_string_view {
		class iter {
			friend filtered_string_view;

		 public:
			using MEMBER_TYPEDEFS_GO_HERE = void;

			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using reference = const char&;
			using pointer = void;
			using difference_type = std::ptrdiff_t;

			iter() noexcept;

			[[nodiscard]] auto operator*() const -> reference;
			auto operator->() const -> pointer;

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) -> bool;
			friend auto operator!=(const iter&, const iter&) -> bool;

		 private:
			iter(const char* iterator_ptr, const filtered_string_view* fsv, const int index) noexcept
			: iterator_ptr_{iterator_ptr}
			, fsv_{fsv}
			, index_{index} {}

			const char* iterator_ptr_;
			const filtered_string_view* fsv_;
			int index_;
		};

	 public:
		static filter default_predicate;
		using iterator = iter;
		using const_iterator = iter;
		using reverse_iterator = std::reverse_iterator<iter>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		// range
		[[nodiscard]] auto begin() const noexcept -> iterator;
		[[nodiscard]] auto cbegin() const noexcept -> const_iterator;
		[[nodiscard]] auto rbegin() const noexcept -> reverse_iterator;
		[[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator;

		[[nodiscard]] auto end() const noexcept -> iterator;
		[[nodiscard]] auto cend() const noexcept -> const_iterator;
		[[nodiscard]] auto rend() const noexcept -> reverse_iterator;
		[[nodiscard]] auto crend() const noexcept -> const_reverse_iterator;

		// constructors
		explicit filtered_string_view() noexcept;
		filtered_string_view(const std::string& str) noexcept;
		explicit filtered_string_view(const std::string& str, filter predicate) noexcept;
		filtered_string_view(const char* str) noexcept;
		explicit filtered_string_view(const char* str, filter predicate) noexcept;

		filtered_string_view(const filtered_string_view& other) noexcept;
		filtered_string_view(filtered_string_view&& other) noexcept;

		// destructor
		~filtered_string_view() noexcept;

		// member operators
		auto operator=(const filtered_string_view& other) noexcept -> filtered_string_view&;
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&;
		[[nodiscard]] auto operator[](int n) const -> const char&;
		[[nodiscard]] explicit operator std::string() const noexcept;

		// member functions
		[[nodiscard]] auto size() const noexcept -> std::size_t;
		[[nodiscard]] auto data() const noexcept -> const char*;
		[[nodiscard]] auto at(int index) const -> const char&;
		[[nodiscard]] auto empty() const noexcept -> bool;
		[[nodiscard]] auto predicate() const noexcept -> const filter&;

		// non-member operators
		friend auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
		friend auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;
		friend auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;

	 private:
		const char* ptr_;
		std::size_t length_;
		filter predicate_;
	};

	// non-member utility functions
	[[nodiscard]] auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view;
	[[nodiscard]] auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view;
	[[nodiscard]] auto split(const filtered_string_view& fsv, const filtered_string_view& tok)
	    -> std::vector<filtered_string_view>;

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
