namespace vaca {

/**

@page page_tn_008 TN008: Unicode support

@li @ref page_tn_008_string_literals
@li @ref page_tn_008_convert_strings
@li @ref page_tn_008_unicode_chars


@section page_tn_008_string_literals String Literals

You have to use the L suffix to define string literals. For example:
@code
String str = L"Hi";
@endcode
This creates a wide character string literal to initialize @c str String.


@section page_tn_008_convert_strings Convert Strings
You can use the convert_to function to convert an ASCII string
to a Vaca String:
@code
String a = convert_to<String>("Hi"); // Use L"Hi" for literals

char* b = strdup("Hi");
String c = convert_to<String>(b);

std::string d = "Hi";
String e = convert_to<String>(d);
@endcode


@section page_tn_008_unicode_chars Unicode Characters
You can use \\xNNNN to create Unicode characters. For example:
@code
Char a_character = L'\x0061'; // same as L'a'
String japanese = L"\x65E5\x672C\x8A9E"; // 日本語
@endcode


*/

}
