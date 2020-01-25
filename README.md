# Foreign Function Interface    

A simple scripting language for loading C functions by specifying the library
they belong to, the name of the symbol, and its signature.

This requires some low level bit diddling.

It uses [libffi](https://github.com/libffi/libffi).
The primary data structure is `ffi_cif` which stands for ffi C interface.

The C interface describes the return value, the arguments of a function,
and the application binary interface. The ABI specifies how arguments
are pushed on the call stack and cleaned up after the function returns.

To load any C symbol at runtime from a library use

```
-llibrary symbol return type ... [abi]
```

where `return` is the type returned by the function, `type...` are the types to be passed
to the function, and  abi` is an optional argument specifying the application binary interface.
By default it uses the ...

The `types are limited to ...
This is all that is needed by assembly code to ...

```
-lm cos double double
```

Now `cos` can be called in a `ffi` script.

```
cos 0
```

The result is pushed onto a stack.

.n pull out the n-th stack item
,n roll n-th stack item to top
