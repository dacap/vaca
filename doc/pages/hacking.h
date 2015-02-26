namespace vaca {

/**

@page page_hacking Hacking Vaca

@li @ref page_hacking_files
@li @ref page_hacking_coding_standards


@section page_hacking_files Files

@section page_hacking_coding_standards Coding Standards

Editor configuration:
@code
  Indent: 2 spaces (use 1 tab to replace 8 spaces)
  Tab size: 8 spaces
@endcode

Function definition:
@code
int function_name(int arg1, int arg2, const char* arg3, double arg4, ...)
{
  statements...
  return 0;
}
@endcode

Function definition (alternative)
@code
int function_name(int arg1, int arg2,
                  const char* arg3,
                  double arg4, ...)
{
  statements...
  return 0;
}
@endcode

Flow control structures:
@code
  if (condition) {
     statements...
  }

  for (int c=0; c<10; ++c) {
     statements...
  }

  for (std::vector<int>::iterator
         it=container.begin(); it!=container.end(); ++it) {
     statements...
  }

  while (condition) {
     statements...
  }

  switch (condition) {
    case 0:
      statements...
      break;
    case 1: {
      definitions...
      statements...
      break;
    }
    default:
      statements...
      break;
  }
@endcode

*/

}

