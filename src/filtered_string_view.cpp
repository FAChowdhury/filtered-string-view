#include "./filtered_string_view.h"

// Implement here
namespace fsv {
	filter filtered_string_view::default_predicate = [](const char&) { return true; };

	// default constructor
	filtered_string_view::filtered_string_view() noexcept
	: ptr_{nullptr}
	, length_{0}
	, predicate_{filtered_string_view::default_predicate} {}

	// implicit string constructor
	filtered_string_view::filtered_string_view(const std::string& str) noexcept
	: ptr_{str.data()}
	, length_{str.length()}
	, predicate_{filtered_string_view::default_predicate} {}

	// string constructor with predicate
	filtered_string_view::filtered_string_view(const std::string& str, filter predicate) noexcept
	: ptr_{str.data()}
	, length_{str.length()}
	, predicate_{predicate} {}

	// implicit null terminated sting constructor
	filtered_string_view::filtered_string_view(const char* str) noexcept
	: ptr_{str}
	, length_{std::strlen(str)}
	, predicate_{filtered_string_view::default_predicate} {}

	// null terminated string constructor with predicate
	filtered_string_view::filtered_string_view(const char* str, filter predicate) noexcept
	: ptr_{str}
	, length_{std::strlen(str)}
	, predicate_{predicate} {}

	// copy constructor
	filtered_string_view::filtered_string_view(const filtered_string_view& other) noexcept
	: ptr_{other.ptr_}
	, length_{other.length_}
	, predicate_{other.predicate_} {}

	// move constructor
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: ptr_{other.ptr_}
	, length_{other.length_}
	, predicate_{other.predicate_} {
		other.ptr_ = nullptr;
		other.length_ = 0;
		other.predicate_ = filtered_string_view::default_predicate;
	}

	// destructor
	filtered_string_view::~filtered_string_view() noexcept = default;

	// member operators
	// copy assignment
	auto filtered_string_view::operator=(const filtered_string_view& other) noexcept -> filtered_string_view& = default;

	// move assignment
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			ptr_ = std::exchange(other.ptr_, nullptr);
			length_ = std::exchange(other.length_, 0);
			predicate_ = std::exchange(other.predicate_, filtered_string_view::default_predicate);
		}
		return *this;
	}

	// subscript
	[[nodiscard]] auto filtered_string_view::operator[](int n) const -> const char& {
		return filtered_string_view::at(n);
	}

	// string type conversion
	[[nodiscard]] filtered_string_view::operator std::string() const noexcept {
		if (filtered_string_view::size() == 0) {
			return std::string();
		}
		auto soln = std::string();
		for (std::size_t i = 0; i < length_; i++) {
			if (predicate_(ptr_[i])) {
				soln += ptr_[i];
			}
		}
		return soln;
	}

	// member functions
	[[nodiscard]] auto filtered_string_view::size() const noexcept -> std::size_t {
		if (ptr_ == nullptr) {
			return static_cast<std::size_t>(0);
		}
		std::size_t soln = 0;
		for (std::size_t i = 0; i < length_; ++i) {
			if (predicate_(ptr_[i])) {
				++soln;
			}
		}
		return soln;
	}

	[[nodiscard]] auto filtered_string_view::data() const noexcept -> const char* {
		return ptr_;
	}

	[[nodiscard]] auto filtered_string_view::at(int index) const -> const char& {
		if (index < 0 or static_cast<std::size_t>(index) >= length_) {
			throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
		}
		int idx = 0;
		for (std::size_t i = 0; i < length_; ++i) {
			if (predicate_(ptr_[i])) {
				if (idx == index) {
					const char& ref = ptr_[i];
					return ref;
				}
				++idx;
			}
		}
		throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
	}

	[[nodiscard]] auto filtered_string_view::empty() const noexcept -> bool {
		return (filtered_string_view::size() == 0) ? true : false;
	}

	[[nodiscard]] auto filtered_string_view::predicate() const noexcept -> const filter& {
		return predicate_;
	}

	// non-member operators
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		std::size_t sz = lhs.size();
		if (sz != rhs.size()) {
			return false;
		}
		for (std::size_t i = 0; i < sz; ++i) {
			if (lhs.at(static_cast<int>(i)) != rhs.at(static_cast<int>(i))) {
				return false;
			}
		}
		return true;
	}

	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream& {
		for (std::size_t i = 0; i < fsv.size(); ++i) {
			os << fsv.at(static_cast<int>(i));
		}
		return os;
	}

	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		auto lhs_size = lhs.size();
		auto rhs_size = rhs.size();
		auto min_size = std::min(lhs_size, rhs_size);
		for (std::size_t i = 0; i < min_size; ++i) {
			if (lhs.at(static_cast<int>(i)) < rhs.at(static_cast<int>(i))) {
				return std::strong_ordering::less;
			}
			else if (lhs.at(static_cast<int>(i)) > rhs.at(static_cast<int>(i))) {
				return std::strong_ordering::greater;
			}
		}

		if (lhs_size < rhs_size) {
			return std::strong_ordering::less;
		}
		else if (lhs_size > rhs_size) {
			return std::strong_ordering::greater;
		}
		else {
			return std::strong_ordering::equivalent;
		}
	}

	[[nodiscard]] auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts)
	    -> filtered_string_view {
		return fsv::filtered_string_view{fsv.data(), [filts](const char& c) {
			                                 for (const auto& filt : filts) {
				                                 if (!filt(c)) {
					                                 return false;
				                                 }
			                                 }
			                                 return true;
		                                 }};
	}

	[[nodiscard]] auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		auto rcount = (count <= 0) ? static_cast<int>(fsv.size()) - pos : count;
		const char* new_ptr = &fsv.at(pos);
		const char* end = &fsv.at(pos + rcount - 1);
		return filtered_string_view{new_ptr, [fsv, end, rcount](const char& c) {
			                            return ((&c <= end) && fsv.predicate()(c) && (rcount != 0));
		                            }};
	}

	[[nodiscard]] auto split(const filtered_string_view& fsv, const filtered_string_view& tok)
	    -> std::vector<filtered_string_view> {
		auto soln = std::vector<filtered_string_view>{};
		if (fsv.empty() or tok.empty() or tok.size() > fsv.size()) {
			auto copy = fsv;
			soln.push_back(copy);
			return soln;
		}
		int begin = 0;
		for (std::size_t end = 0; end < fsv.size(); ++end) {
			if (fsv.at(static_cast<int>(end)) == tok.at(0)) {
				bool is_a_match = true;
				for (std::size_t i = 1; i < tok.size(); ++i) {
					if (fsv.at(static_cast<int>(end + i)) != tok.at(static_cast<int>(i))) {
						is_a_match = false;
						break;
					}
				}
				if (is_a_match) {
					if (static_cast<int>(end) != begin) {
						soln.push_back(fsv::substr(fsv, begin, static_cast<int>(end) - begin));
					}
					else {
						soln.push_back(fsv::filtered_string_view{});
					}
					begin = static_cast<int>(tok.size() + end);
					end = static_cast<std::size_t>(begin - 1);
				}
			}
		}
		if (begin < static_cast<int>(fsv.size())) {
			soln.push_back(fsv::substr(fsv, begin, static_cast<int>(fsv.size()) - begin));
		}
		else {
			soln.push_back(fsv::filtered_string_view{});
		}

		return soln;
	}

	// iterator

	fsv::filtered_string_view::iter::iter() noexcept = default;

	[[nodiscard]] auto fsv::filtered_string_view::iter::operator*() const -> reference {
		return *iterator_ptr_;
	}

	auto fsv::filtered_string_view::iter::operator->() const -> void {}

	auto fsv::filtered_string_view::iter::operator++() -> iter& {
		int sz = static_cast<int>(fsv_->size());
		index_++;
		if (index_ >= sz) {
			index_ = sz;
			iterator_ptr_ = &(fsv_->at(sz - 1));
			++iterator_ptr_;
			return *this;
		}

		do {
			++iterator_ptr_;
		} while (!fsv_->predicate_(*iterator_ptr_));
		return *this;
	}

	auto fsv::filtered_string_view::iter::operator++(int) -> iter {
		auto save = *this;
		++(*this);
		return save;
	}

	auto fsv::filtered_string_view::iter::operator--() -> iter& {
		--index_;
		if (index_ < 0) {
			index_ = -1;
			iterator_ptr_ = fsv_->ptr_;
			--iterator_ptr_;
			return *this;
		}

		do {
			--iterator_ptr_;
		} while (!fsv_->predicate_(*iterator_ptr_));
		return *this;
	}

	auto fsv::filtered_string_view::iter::operator--(int) -> iter {
		auto save = *this;
		--(*this);
		return save;
	}

	auto operator==(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) -> bool {
		return ((lhs.iterator_ptr_ == rhs.iterator_ptr_) and (lhs.fsv_ == rhs.fsv_) and (lhs.index_ == rhs.index_));
	}

	auto operator!=(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) -> bool {
		return !(operator==(lhs, rhs));
	}

	[[nodiscard]] auto fsv::filtered_string_view::begin() const noexcept -> iterator {
		return size() == 0 ? iterator(ptr_, this, 0) : iterator(&((*this).at(0)), this, 0);
	}

	[[nodiscard]] auto fsv::filtered_string_view::end() const noexcept -> iterator {
		return iterator(&((*this).at(static_cast<int>((*this).size() - 1))) + 1, this, static_cast<int>((*this).size()));
	}

	[[nodiscard]] auto fsv::filtered_string_view::cbegin() const noexcept -> const_iterator {
		return size() == 0 ? const_iterator(ptr_, this, 0) : const_iterator(&((*this).at(0)), this, 0);
	}

	[[nodiscard]] auto fsv::filtered_string_view::cend() const noexcept -> const_iterator {
		return const_iterator(&((*this).at(static_cast<int>((*this).size() - 1))) + 1,
		                      this,
		                      static_cast<int>((*this).size()));
	}

	[[nodiscard]] auto fsv::filtered_string_view::rbegin() const noexcept -> reverse_iterator {
		return reverse_iterator{end()};
	}

	[[nodiscard]] auto fsv::filtered_string_view::rend() const noexcept -> reverse_iterator {
		return reverse_iterator{begin()};
	}

	[[nodiscard]] auto fsv::filtered_string_view::crbegin() const noexcept -> const_reverse_iterator {
		return reverse_iterator{cend()};
	}

	[[nodiscard]] auto fsv::filtered_string_view::crend() const noexcept -> const_reverse_iterator {
		return reverse_iterator{cbegin()};
	}

} // namespace fsv
