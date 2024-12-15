<h1>Filtered String View Library</h1>
<h2>Description</h2>
<p>A <code>string_view</code> can be thought of as a "view" into an existing character buffer. It contains a pointer and a length that allows you to identify a section of character data without modification. A <code>filtered_string_view</code> is a view into an existing character buffer, but allows for the passing of a predicate function (or filter) that only allows the viewing of characters in the buffer that the predicate allows.</p>

<p>For example, suppose we have the string <code>"Hello, World!"</code> and pass it into the <code>filtered_string_view</code> with the predicate function that returns true if the character is 'H', 'e', 'l', 'W' or 'd' but false otherwise. In this instance, the <code>filtered_string_view</code> will allows the viewing of the filtered string <code>"HellWld"</code> from the buffer.</p>
