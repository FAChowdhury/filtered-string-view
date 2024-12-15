<h1>Filtered String View Library</h1>
<h2>Description</h2>
<p>A <code>string_view</code> can be thought of as a "view" into an existing character buffer. It contains a pointer and a length that allows you to identify a section of character data without modification. A <code>filtered_string_view</code> is a view into an existing character buffer, but allows for the passing of a predicate function (or filter) that only allows the viewing of characters in the buffer that the predicate allows.</p>

<p>For example, suppose we have the string <code>"Hello, World!"</code> and pass it into the <code>filtered_string_view</code> with the predicate function that returns true if the character is <code>'H'</code>, <code>'e'</code>, <code>'l'</code>, <code>'W'</code> or <code>'d'</code> but false otherwise. In this instance, the <code>filtered_string_view</code> will allows the viewing of the filtered string <code>"HellWld"</code> from the buffer.</p>

<h2>Documentation</h2>

### 1. Constructors

#### 1.1. Default Constructor
```cpp
filtered_string_view();
```

Constructs a `filtered_string_view` with the pointer set to `nullptr`, the length set to `0`, and the predicate set to the `true` predicate.

##### Examples
```cpp
auto sv = fsv::filtered_string_view{};
std::cout << sv.size() << std::endl;
```

Output: `0`


#### 2.4.2. Implicit String Constructor

```cpp
filtered_string_view(const std::string &str);
```

Constructs a `filtered_string_view` with the pointer set to the string's underlying data, and the predicate set to the `true` predicate.

##### Examples

```cpp
auto s = std::string{"cat"};
auto sv = fsv::filtered_string_view{s};
std::cout << sv.size() << std::endl;
```

Output: `3`

#### 2.4.3. String Constructor with Predicate

```cpp
filtered_string_view(const std::string &str, filter predicate);
```

Same as the Implicit String Constructor, but with the predicate set to the given one.

##### Examples

```cpp
auto s = std::string{"cat"};
auto pred = [](const char& c) { return c == 'a'; };
auto sv = fsv::filtered_string_view{s, pred};
std::cout << sv.size() << std::endl;
```

Output: `1`

#### 2.4.4. Implicit Null-Terminated String Constructor

```cpp
filtered_string_view(const char *str);
```

Constructs a `filtered_string_view` with the pointer set to the given null-terminated string, and the predicate set to the `true` predicate.

##### Examples

```cpp
auto sv = fsv::filtered_string_view{"cat"};
std::cout << sv.size() << std::endl;
```

Output: `3`

#### 2.4.5. Null-Terminated String with Predicate Constructor

```cpp
filtered_string_view(const char *str, filter predicate);
```

Same as the implicit null-terminated string pointer constructor, but with the predicate set to the given one.

##### Examples

```cpp
auto pred = [](const char &c) { return c == 'a'; };
auto sv = fsv::filtered_string_view{"cat", pred};
std::cout << sv.size();
```

Output: `1`

#### 2.4.6. Copy and Move Constructors

```cpp
/* 1 */ filtered_string_view(const filtered_string_view &other);
/* 2 */ filtered_string_view(filtered_string_view &&other);
```

##### Examples

```cpp
auto sv1 = fsv::filtered_string_view{"bulldog"};
const auto copy = sv1;

assert(copy.data() == sv1.data()); // pointers compare equal.

const auto move = std::move(sv1);
assert(sv1.data() == nullptr); // true: sv1's guts were moved into `move`
```

----

### 2.5. Destructor
```cpp
~filtered_string_view();
```

**Important**: You must explicitly declare the destructor as default.

----

### 2.5.1. Member Operators
#### 2.5.2. Copy Assignment

```cpp
operator=(const filtered_string_view &other);
```

Copies at least the length, data and predicate of `other` so that after the copy-assignment this `filtered_string_view` and `other` compare equal via `operator==`.

In the case of self-copy, the object should remain unchanged.

##### Examples
```cpp
auto pred = [](const char &c) { return c == '4' || c == '2'; };
auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
auto fsv2 = fsv::filtered_string_view{};
fsv2 = fsv1;
assert(fsv1 == fsv2);
```

#### 2.5.3. Move Assignment
```cpp
operator=(filtered_string_view &&other);
```

The moved from object should be left in a valid state equivalent to a default-constructed `filtered_string_view`, except in the case of self-assignment, in which the moved from object should remain unchanged.

##### Examples
```cpp
auto pred = [](const char &c) { return c == '8' || c == '9'; };
auto fsv1 = fsv::filtered_string_view{"'89 baby", pred};
auto fsv2 = fsv::filtered_string_view{};

fsv2 = std::move(fsv1);

assert(fsv1.size() == 0 && fsv1.data() == nullptr);
```

#### 2.5.4. Subscript
```cpp
auto operator[](int n) -> const char &;
```
Allows reading a character from the **filtered string** given its index.
 
##### Examples

```cpp
auto pred = [](const char &c) { return c == '9' || c == '0' || c == ' '; };
auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
std::cout << fsv1[2] << std::endl;
```

Output: `0`

#### 2.5.5. String Type Conversion
```cpp
explicit operator std::string();
```

Enables type casting a `filtered_string_view` to a `std::string`. The returned `std::string` is a copy of the filtered string, so any characters which are filtered out should not be present. The returned `std::string` is a copy so that modifications to it do not mutate the underyling data backing the `filtered_string_view`.

##### Examples

```cpp
auto sv = fsv::filtered_string_view("vizsla");
auto s = static_cast<std::string>(sv);
std::cout << std::boolalpha << (sv.data() == s.data()) << std::endl;
```

Output: `false`

----

### 2.6. Member Functions

#### 2.6.1. at
```cpp
auto at(int index) -> const char &;
```

Allows reading a character from the **filtered string** given its index.

An index is valid if and only if
```cpp
assert(0 <= index < size());
```
holds. This means no index is valid when `size() == 0`.

Returns:
- the character at `index` in the **filtered string** if the index is valid.

Throws:
- a `std::domain_error{"filtered_string_view::at(<index>): invalid index"}`, where `<index>` should be replaced with the actual index passed in if the index is invalid.
- 
##### Examples

```cpp
auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
auto is_vowel = [&vowels](const char &c){ return vowels.contains(c); };
auto sv = fsv::filtered_string_view{"Malamute", is_vowel};
```

Output: `a`

```cpp
auto sv = fsv::filtered_string_view{""};
try {
  sv.at(0);
} catch (const std::domain_error &e) {
  std::cout << e.what();
}
```

Output: `filtered_string_view::at(0): invalid index`

#### 2.6.2. size
```cpp
auto size() -> std::size_t;
```

Returns the size of the filtered string.

##### Examples

```cpp
auto sv = fsv::filtered_string_view{"Maltese"};
std::cout << sv.size();
```

Output: `7`

```cpp
auto sv = fsv::filtered_string_view{"Toy Poodle", [](const char &c) {
    return c == 'o';
}};
std::cout << sv.size();
```

Output: `3`

#### 2.6.3. empty
```cpp
auto empty() -> bool;
```

Returns whether the filtered string is empty.
The filtered string is "empty" when `size() == 0`.

##### Examples

```cpp
auto sv = fsv::filtered_string_view{"Australian Shephard"};
auto empty_sv = fsv::filtered_string_view{};
std::cout << std::boolalpha << sv.empty() << ' ' << empty_sv.empty();
```

Output: `false true`

```cpp
auto sv = fsv::filtered_string_view{"Border Collie", [](const char &c) {
    return c == 'z';
}};
std::cout << std::boolalpha << sv.empty();
```

Output: `true`

#### 2.6.4. data
```cpp
auto data() -> const char *;
```

Returns a pointer to the underlying data backing the `filtered_string_view`. Filtering behaviour is ignored in this function.

##### Examples

```cpp
auto s = "Sum 42";
auto sv = fsv::filtered_string_view{s, [](const char &c){ return false; }};
for (auto ptr = sv.data(); *ptr; ++ptr) {
  std::cout << *ptr;
}
```

Output: `Sum 42`

#### 2.6.5. predicate
```cpp
auto predicate() -> const filter&;
```
Allows access to the predicate used to do filtering.

##### Examples 

```cpp
const auto print_and_return_true = [](const char &) {
    std::cout << "hi!";
    return true;
};
const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

const auto& predicate = s.predicate();
predicate(char{});
```
Output: `hi!`
    
----

### 2.7. Non-Member Operators

#### 2.7.1. Equality Comparison
```cpp
auto operator==(const filtered_string_view &lhs, const filtered_string_view &rhs) -> bool;
```
Lexicographically compares two `filtered_string_view`s for equality.
The predicate function *does not* participate in equality directly: only the two filtered strings should be lexicographically compared.

##### Examples

```cpp
auto const lo = fsv::filtered_string_view{"aaa"};
auto const hi = fsv::filtered_string_view{"zzz"};

std::cout << std::boolalpha 
          << (lo == hi) << ' '
          << (lo != hi);
```
Output: `false true`

#### 2.7.2. Relational Comparison

```cpp
auto operator<=>(const filtered_string_view &lhs, const filtered_string_view &rhs) -> std::strong_ordering;
```

Uses the C++20 spaceship operator to lexicographically compare two `filtered_string_view`s.
The predicate function *does not* participate in comparison directly: only the two filtered strings should be lexicographically compared.

##### Examples

```cpp
auto const lo = fsv::filtered_string_view{"aaa"};
auto const hi = fsv::filtered_string_view{"zzz"};

std::cout << std::boolalpha 
          << (lo < hi) << ' '
          << (lo <= hi) << ' '
          << (lo > hi) << ' '
          << (lo >= hi) << ' '
          << (lo <=> hi == std::strong_ordering::less);
```
Output: `true true false false true`

#### 2.7.3. Output Stream

```cpp
auto operator<<(std::ostream &os, const filtered_string_view &fsv) -> std::ostream&;
```

Prints to `os` the characters of the filtered string in `fsv`.

There is no newline at the end.

##### Examples

```cpp
auto fsv = fsv::filtered_string_view{"c++ > rust > java", [](const char &c){ return c == 'c' || c == '+'; }};
std::cout << fsv;
```

Output: `c++`

### 2.8. Non-Member Utility Functions

#### 2.8.1. compose
```cpp
auto compose(const filtered_string_view &fsv, const std::vector<filter> &filts) -> filtered_string_view;
```
Accepts a `filtered_string_view` and a vector of filtering predicates and returns a new `filtered_string_view` which filters the same underlying string as `fsv` but with a predicate which only returns `true` if all of the filters in `filts` (called in order of the vector from left to right) also return `true`, that is, all the filters would have filtered the same letter. If any filter in the chain of function calls returns `false`, the new predicate should short-circuit and **not** call the subsequent functions and return `false`.

This is akin to a logical AND. In the expression
```cpp
bool b0 = true, b1 = false, b2 = true;
b0 && b1 && b2;
```
`b0` and `b1` will be evaluated, but since `b1` is `false`, `b2` won't be evaluated. This expression has "short-circuited".

**Note**: `fsv`'s underlying string will always be null-terminated when calling this function, so it is possible to determine its length with `std::strlen()`.

##### Examples
```cpp
auto best_languages = fsv::filtered_string_view{"c / c++"};
auto vf = std::vector<filter>{
  [](const char &c){ return c == 'c' || c == '+' || c == '/'; },
  [](const char &c){ return c > ' '; },
  [](const char &c){ return true; }
};

auto sv = fsv::compose(best_languages, vf);
std::cout << sv;
```

Output: `c/c++`

#### 2.8.2. split

```cpp
auto split(const filtered_string_view &fsv, const filtered_string_view &tok) -> std::vector<filtered_string_view>;
```
Splits `fsv` on the delimiter in `tok` into a vector of substrings. This can be done by initialising each slice of the split with the original underlying data of `fsv` and a new predicate which calculates the extent of the substring within the original string.

Usually, the delimiter appears in the middle of two strings. It is, however, possible that the delimiter appears only on the left or the right of a split, and not the middle. In these cases, the parts before/after the delimiter is equivalent to the `filtered_string_view("")`. Furthermore, the delimiter should not be a part of any of the split slices.

If `tok` does not appear in `fsv`, returns a vector of a single element which is a copy of the original `fsv`. For our purposes, the empty string `""` *never* appears inside of `fsv`.

Similarly, if `fsv` is empty, returns a vector of a single element which is a copy of the original `fsv`.

##### Examples
```cpp
auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef", [&interest](const char &c){ return interest.contains(c); }};
auto tok = fsv::filtered_string_view{" / "};
auto v = fsv::split(sv, tok);

std::cout << v[0] << " " << v[1];
```

Output: `DEADBEEF deadbeef`

```cpp
auto sv = fsv::filtered_string_view{"xax"};
auto tok  = fsv::filtered_string_view{"x"};
auto v = fsv::split(fsv, tok);
auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};

CHECK(v == expected);
```

```cpp
auto sv = fsv::filtered_string_view{"xx"};
auto tok  = fsv::filtered_string_view{"x"};
auto v = fsv::split(sv, tok);
auto expected = std::vector<fsv::filtered_string_view>{"", "", ""};

CHECK(v == expected);
```

#### 2.8.3. substr
```cpp
auto substr(const filtered_string_view &fsv, int pos = 0, int count = 0) -> filtered_string_view;
```

Returns a new `filtered_string_view` with the same underlying string as `fsv` which presents a "substring" view. The substring begins at `pos` and has length `rcount`, where `rcount = count <= 0 ? size() - pos() : count`. That is, it provides a view into the substring `[pos, pos + rcount)` of `fsv`.

**Note**: it is possible to have a substring of length 0. In that case, the returns `filtered_string_view` equivalent to `""`.

##### Examples
```cpp
auto sv = fsv::filtered_string_view{"Siberian Husky"};
std::cout << fsv::substr(sv, 9);
```
Output: `Husky`

```cpp
auto is_upper = [](const char &c) { return std::isupper(static_cast<unsigned char>(c)};
auto sv = fsv::filtered_string_view{"Sled Dog", is_upper};
std::cout << fsv::substr(sv, 0, 2);
```
Output: `SD`

----

### 2.9. Iterator

A bidirectional `const_iterator` is implemented as the iterator for a filtered_string_view -- this means that even with a non-constant `filtered_string_view` it is impossible to mutate the underlying filtered_string.

#### Examples
```cpp
auto print_via_iterator = [](fsv::filtered_string_view const& sv) {
  std::copy(sv.begin(), sv.end(), std::ostream_iterator<char>(std::cout, " "));
}

// With default predicate:
auto fsv1 = fsv::filtered_string_view{"corgi"};
print_via_iterator(fsv1);
```
Output: `c o r g i`

```cpp
// With predicate which removes lowercase vowels:
auto fsv = fsv::filtered_string_view{"samoyed", [](const char &c) {
  return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}};
auto it = fsv.begin();
std::cout << *it << *std::next(it)  << *std::next(it, 2) << *std::next(it, 3);
```
Output: `smyd`

```cpp
const auto str = std::string("tosa")
const auto s = fsv::filtered_string_view{str};
auto it = s.cend();
std::cout << *std::prev(it) << *std::prev(it, 2);
```
Output: `as`


### 2.10. Range

A `filtered_string_view` is able to be used as a bidirectional range. This means that the standard `begin()`, `end()`, `cbegin()`, `cend()`, `rbegin()`, `rend()`, `crbegin()`, `crend()` suite of functions are implemented as member functions.

Furthermore, the bidirectional range type members:
- `iterator`
- `const_iterator`
- `reverse_iterator`
- `const_reverse_iterator`

are publically available.

```cpp
auto begin();
auto cbegin();
```
Returns an iterator pointing to the start of the filtered string.

```cpp
auto end();
auto cend();
```

Returns an iterator pointing one-past-the-end of the filtered string, acting as a placeholder indicating there are no more characters to read.

```cpp
auto rbegin();
auto crbegin();
```

Returns an iterator pointing to the start of the reversed filtered string.

```cpp
auto rend();
auto crend();
```

Returns an iterator pointing one-past-the-end of the reversed filtered string, acting as a placeholder indicating there are no more characters to read.

#### Examples
```cpp
const auto s = fsv::filtered_string_view{"puppy", [](const char &c){ return !(c == 'u' || c == 'y'); }};
auto v = std::vector<char>{s.begin(), s.end()};
std::cout << v[0] << v[1] << v[2] << std::endl;
```
Output: `ppp`


```cpp
auto s = fsv::filtered_string_view{"milo", [](const char &c){ return !(c == 'i' || c == 'o'); }};
auto v = std::vector<char>{s.rbegin(), s.rend()};
std::cout << v[0] << v[1] << std::endl;
```
Output: `lm`

----
