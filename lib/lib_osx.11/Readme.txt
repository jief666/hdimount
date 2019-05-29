2019-05

Libiconv from Apple declare iconv_open() etc. Libiconv from GNU declares libiconv_open() etc.
Therefore, libxml2 can be compiled with one or the other and the result cannot be swapped.

To compile libxml2 with Libiconv GNU :
  - brew edit libxml2
  - add "--with-iconv=/usr/local/Cellar/libiconv/1.16" to configure parameters
  - delete or rename /usr/include/iconv.h.
  - brew install libxml2 --debug
  - enter shell at error
  - get the current working directory
  - open a new shell
  - cd to that directory
  - make
  - get result from .libs
