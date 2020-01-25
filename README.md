# Foreign Function Interface    

A simple scripting language for loading C functions by specifying the library
they belong to, the name of the symbol, and its signature.

It uses [libffi](https://github.com/libffi/libffi).
The primary data structure is `ffi_cif` which stands for C interface.

The C interface desribes the return value and arguments of a function.
The `libffi` library also needs to know about how to push arguments on
the call stack and clean up after calling a function: the Application
Binary Interface.

To load any C function at runtime from a libray use

```
-llib sym ret arg ...
```



Call C functions at runtime.  

Open a dynamic link library.

Load symbols from the library.

Describe its C interface.

Call the function.

