<h1>Filtered String View Library</h1>
<h2>Description</h2>
<p>A <code>string_view</code> can be thought of as a "view" into an existing character buffer. It contains a pointer and a length that allows you to identify a section of character data without modification. A <code>filtered_string_view</code> is a view into an existing character buffer, but allows for the passing of a predicate function (or filter) that only allows the viewing of characters in the buffer that the predicate allows.</p>
