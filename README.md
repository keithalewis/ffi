# Foreign Function Interface    

A simple scripting language for loading C functions by specifying the library
they belong to, the name of the symbol, and its signature.

Maybe a better name for this is C function interface. It uses [libffi](https://github.com/libffi/libffi)

Call C functions at runtime.  

Open a dynamic link library.

Load symbols from the library.

Describe its C interface.

Call the function.

