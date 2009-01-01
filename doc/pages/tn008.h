namespace Vaca {

/**

@page page_tn_008 TN008: Unicode support

@li @ref page_tn_008_string_literals
@li @ref page_tn_008_convert_strings
@li @ref page_tn_008_unicode_chars
@li @ref page_tn_008_win98


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


@section page_tn_008_win98 Unicode Support in Windows 98/Me

Vaca can be compiled with Unicode support without problems for
W2K/WinXP. If you want to add Unicode support for Win95/98/Me, you
will need the
<a href="http://www.microsoft.com/downloads/details.aspx?FamilyId=73BA7BD7-ed06-4F0D-80A4-2A7EEAEE17E2&displaylang=en">Microsoft Layer for Unicode (MSLU)</a>.
To avoid the MSLU, don't compile Vaca with Unicode support when you
want to use your application on Win95/98/Me.

*/

}
