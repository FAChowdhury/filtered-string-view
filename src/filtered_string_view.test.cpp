#include "./filtered_string_view.h"

#include <catch2/catch.hpp>

TEST_CASE("Static Data Members") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		REQUIRE(fsv::filtered_string_view::default_predicate(c));
	}
}

TEST_CASE("Default Constructor") {
	auto sv = fsv::filtered_string_view{};
	REQUIRE(sv.size() == 0);
}

TEST_CASE("Implicit String Constructor") {
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s};
	REQUIRE(sv.size() == 3);
}

TEST_CASE("String Constructor with Predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred};
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Implicit Null Terminated String Constructor") {
	auto sv = fsv::filtered_string_view{"cat"};
	REQUIRE(sv.size() == 3);
}

TEST_CASE("Null Terminated String Constructor with Predicate") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{"cat", pred};
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Copy Constructor and Move Constructor") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto copy = sv1;

	REQUIRE(copy.data() == sv1.data()); // pointers compare equal.

	const auto move = std::move(sv1);
	REQUIRE(sv1.data() == nullptr); // true: sv1's guts were moved into `move`
	REQUIRE(move.data() == copy.data()); // check that move contains sv1's content
}

TEST_CASE("Copy Assignment") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;
	REQUIRE(fsv1 == fsv2);
}

TEST_CASE("at() works") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	auto sv = fsv::filtered_string_view{"Malamute", is_vowel};
	REQUIRE(sv.at(0) == 'a');
	REQUIRE(sv.at(1) == 'a');
	REQUIRE(sv.at(2) == 'u');
	REQUIRE(sv.at(3) == 'e');
}

TEST_CASE("Move Assignment with new fsv") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	auto fsv1 = fsv::filtered_string_view{"'89 baby", pred};
	auto fsv2 = fsv::filtered_string_view{};

	fsv2 = std::move(fsv1);

	REQUIRE(fsv1.size() == 0);
	REQUIRE(fsv1.data() == nullptr);
}

TEST_CASE("Move Assignment with self assignment") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	auto fsv1 = fsv::filtered_string_view{"'89 baby", pred};
	const char* ptr = fsv1.data();
	std::size_t sz = fsv1.size();
	REQUIRE(ptr != nullptr);
	REQUIRE(sz == 2);

	fsv1 = std::move(fsv1);

	REQUIRE(fsv1.data() == ptr);
	REQUIRE(fsv1.size() == 2);
}

TEST_CASE("at() throws exception") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	auto sv = fsv::filtered_string_view{"Malamute", is_vowel};
	REQUIRE_THROWS_AS(sv.at(-1), std::domain_error);
	REQUIRE_THROWS_AS(sv.at(8), std::domain_error);
	REQUIRE_THROWS_AS(sv.at(4), std::domain_error);
}

TEST_CASE("at() has correct error message") {
	auto sv = fsv::filtered_string_view{""};
	try {
		auto x = sv.at(0);
		(void)x;
	} catch (const std::domain_error& e) {
		REQUIRE(e.what() == std::string("filtered_string_view::at(0): invalid index"));
	}
	try {
		auto y = sv.at(3);
		(void)y;
	} catch (const std::domain_error& e) {
		REQUIRE(e.what() == std::string("filtered_string_view::at(3): invalid index"));
	}
}

TEST_CASE("subscript") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
	REQUIRE(fsv1[0] == ' ');
	REQUIRE(fsv1[1] == '9');
	REQUIRE(fsv1[2] == '0');
	REQUIRE(fsv1[3] == ' ');
	REQUIRE(fsv1[4] == ' ');
}

TEST_CASE("string type conversion no predicate") {
	auto sv = fsv::filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);
	REQUIRE(sv.data() != s.data());
	REQUIRE(s == "vizsla");
}

TEST_CASE("string type conversion with predicate") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
	auto s = static_cast<std::string>(fsv1);
	REQUIRE(s == " 90  ");
}

TEST_CASE("string type conversion with empty string") {
	auto fsv1 = fsv::filtered_string_view{};
	auto s = static_cast<std::string>(fsv1);
	REQUIRE(s == "");
}

TEST_CASE("empty()") {
	auto sv = fsv::filtered_string_view{"Australian Shephard"};
	auto empty_sv = fsv::filtered_string_view{};
	REQUIRE(sv.empty() == false);
	REQUIRE(empty_sv.empty() == true);

	auto sv2 = fsv::filtered_string_view{"Border Collie", [](const char& c) { return c == 'z'; }};
	REQUIRE(sv2.empty() == true);
}

TEST_CASE("Access to predicate") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	const auto s = fsv::filtered_string_view{"doggo", pred};

	const auto& predicate = s.predicate();
	REQUIRE(predicate('9') == true);
	REQUIRE(predicate('0') == true);
	REQUIRE(predicate(' ') == true);
	REQUIRE(predicate('b') == false);
}

TEST_CASE("Output Stream") {
	auto fsv = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	std::ostringstream os;
	os << fsv;
	REQUIRE(os.str() == "c++");

	auto fsv2 = fsv::filtered_string_view{"c++ > rust > java"};
	std::ostringstream os2;
	os2 << fsv2;
	REQUIRE(os2.str() == "c++ > rust > java");

	auto fsv3 = fsv::filtered_string_view{};
	std::ostringstream os3;
	os3 << fsv3;
	REQUIRE(os3.str() == "");

	auto fsv4 = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'n'; }};
	std::ostringstream os4;
	os4 << fsv4;
	REQUIRE(os4.str() == "");

	auto fsv5 =
	    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == 'r' || c == 'j'; }};
	std::ostringstream os5;
	os5 << fsv5;
	REQUIRE(os5.str() == "crj");
}

TEST_CASE("Relational Comparison") {
	auto const lo = fsv::filtered_string_view{"aaa"};
	auto const hi = fsv::filtered_string_view{"zzz"};

	REQUIRE((lo < hi) == true);
	REQUIRE((lo <= hi) == true);
	REQUIRE((lo > hi) == false);
	REQUIRE((lo >= hi) == false);
	REQUIRE((lo <=> hi) == std::strong_ordering::less);
}

TEST_CASE("Relational Comparison with predicate") {
	auto const lo = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	auto const hi =
	    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'j' || c == 'a' || c == 'v'; }};

	REQUIRE((lo < hi) == true);
	REQUIRE((lo <= hi) == true);
	REQUIRE((lo > hi) == false);
	REQUIRE((lo >= hi) == false);
	REQUIRE((lo <=> hi) == std::strong_ordering::less);

	auto const equal1 =
	    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	auto const equal2 = fsv::filtered_string_view{"i love c++", [](const char& c) { return c == 'c' || c == '+'; }};

	REQUIRE((equal1 < equal2) == false);
	REQUIRE((equal1 <= equal2) == true);
	REQUIRE((equal1 > equal2) == false);
	REQUIRE((equal1 >= equal2) == true);
	REQUIRE((equal1 == equal2) == true);
	REQUIRE((equal1 <=> equal2) == std::strong_ordering::equivalent);

	auto const equal3 = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'n'; }};
	auto const equal4 = fsv::filtered_string_view{"i love c++", [](const char& c) { return c == 'a'; }};
	REQUIRE((equal3 <=> equal4) == std::strong_ordering::equivalent);
}

TEST_CASE("compose") {
	auto best_languages = fsv::filtered_string_view{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                                   [](const char& c) { return c > ' '; }};

	auto sv = fsv::compose(best_languages, vf);
	std::ostringstream os;
	os << sv;
	REQUIRE(os.str() == "c/c++");

	auto best_languages2 = fsv::filtered_string_view{"c / c++"};
	auto vf2 = std::vector<fsv::filter>{[](const char& c) { return c == 'c' || c == '/' || c == ' '; },
	                                    [](const char& c) { return c >= ' '; }};

	auto sv2 = fsv::compose(best_languages2, vf2);
	std::ostringstream os2;
	os2 << sv2;
	REQUIRE(os2.str() == "c / c");
}

TEST_CASE("substr") {
	auto sv = fsv::filtered_string_view{"Siberian Husky"};
	REQUIRE(fsv::substr(sv, 9) == "Husky");
	REQUIRE(fsv::substr(sv, 5, 5) == "ian H");
}

TEST_CASE("substr with predicate") {
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv = fsv::filtered_string_view{"Sled Dog", is_upper};
	REQUIRE(fsv::substr(sv, 0, 1) == "S");
	REQUIRE(fsv::substr(sv, 0, 2) == "SD");
	REQUIRE(fsv::substr(sv, 1, 1) == "D");
}

TEST_CASE("split") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v = fsv::split(sv, tok);

	std::ostringstream os1;
	os1 << v[0];
	REQUIRE(os1.str() == "DEADBEEF");

	std::ostringstream os2;
	os2 << v[1];
	REQUIRE(os2.str() == "deadbeef");
}

TEST_CASE("split2") {
	auto sv = fsv::filtered_string_view{"xax"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};
	CHECK(v == expected);
}

TEST_CASE("split3") {
	auto sv = fsv::filtered_string_view{"xx"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "", ""};
	CHECK(v == expected);
}

TEST_CASE("split fsv empty") {
	auto sv = fsv::filtered_string_view{};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{""};
	REQUIRE(v == expected);
}

TEST_CASE("split tok empty") {
	auto sv = fsv::filtered_string_view{"xoxo"};
	auto tok = fsv::filtered_string_view{""};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"xoxo"};
	REQUIRE(v == expected);
}

TEST_CASE("split tok==fsv") {
	auto sv = fsv::filtered_string_view{"xoxo"};
	auto tok = fsv::filtered_string_view{"xoxo"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", ""};
	REQUIRE(v == expected);
}

TEST_CASE("iterator basic") {
	// With default predicate:
	auto fsv1 = fsv::filtered_string_view{"corki"};
	auto vec = std::vector<char>{fsv1.begin(), fsv1.end()};
	auto expected = std::vector<char>{'c', 'o', 'r', 'k', 'i'};
	REQUIRE(vec == expected);
	auto vec2 = std::vector<char>(fsv1.cbegin(), fsv1.cend());
	REQUIRE(vec2 == expected);
}

TEST_CASE("iter basic with predicate") {
	auto const fsv1 =
	    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'j' || c == 'a' || c == 'v'; }};
	auto vec = std::vector<char>{fsv1.begin(), fsv1.end()};
	auto expected = std::vector<char>{'j', 'a', 'v', 'a'};
	REQUIRE(vec == expected);
}

TEST_CASE("iterator equality") {
	auto fsv1 = fsv::filtered_string_view{"corki"};
	auto it1 = fsv1.begin();
	auto it2 = fsv1.end();
	++it1;
	++it1;
	--it2;
	--it2;
	--it2;
	REQUIRE(it1 == it2);
	REQUIRE(*it2 == 'r');
	REQUIRE(*it2 == 'r');
}

TEST_CASE("iterator equality with predicate") {
	auto const fsv1 =
	    fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'j' || c == 'a' || c == 'v'; }};
	auto it1 = fsv1.begin();
	auto it2 = fsv1.end();
	++it1;
	++it1;
	--it2;
	--it2;
	REQUIRE(it1 == it2);
	REQUIRE(*it1 == *it2);
}

TEST_CASE("iterator with std::next") {
	auto fsv = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                     return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                     }};
	auto it = fsv.begin();
	std::ostringstream os;
	os << *it << *std::next(it) << *std::next(it, 2) << *std::next(it, 3);
	REQUIRE(os.str() == "smyd");
}

TEST_CASE("const iterator with std::next") {
	auto fsv = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                     return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                     }};
	auto it = fsv.cbegin();
	std::ostringstream os;
	os << *it << *std::next(it) << *std::next(it, 2) << *std::next(it, 3);
	REQUIRE(os.str() == "smyd");
}

TEST_CASE("iterator with std::prev") {
	const auto str = std::string("tosa");
	const auto s = fsv::filtered_string_view{str};
	auto it = s.cend();
	std::ostringstream os;
	os << *std::prev(it) << *std::prev(it, 2);
	REQUIRE(os.str() == "as");
}

TEST_CASE("Reverse Iterator and reverse const iterator") {
	auto s = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v = std::vector<char>{s.rbegin(), s.rend()};
	auto vc = std::vector<char>(s.crbegin(), s.crend());
	auto expected = std::vector<char>{'l', 'm'};
	REQUIRE(v[0] == 'l');
	REQUIRE(v[1] == 'm');
	REQUIRE(v == expected);
	REQUIRE(vc == expected);
}

TEST_CASE("Iterator prefix and postfix") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto it1 = sv.rbegin();
	REQUIRE(*it1 == 'f');
	REQUIRE(*(it1++) == 'f');
	REQUIRE(*it1 == 'e');
	++it1;
	++it1;
	REQUIRE(*it1 == 'b');
	REQUIRE(*(++it1) == 'd');

	REQUIRE(*(--it1) == 'b');
	REQUIRE(*(it1--) == 'b');
	REQUIRE(*(it1) == 'e');
}