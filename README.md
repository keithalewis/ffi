# Foreign Function Interface    

A simple scripting language for loading C functions by specifying the library
they belong to, the name of the symbol, and its signature.

To load the cosine function from the C math library use
```
-lm cos double double
```

To call the cosine function use
```
cos 3.14
```

This pushes the floating point number `3.14` on the call stack, calls `cos`
and pushes the result of the call on the stack.

To print the result to stdout (and pop the arguments off the stack) use
```
printf "%d"
```
Since the stack contains the result of the previous call this pushes
the string `"%d"` on the stack and calls the C function `printf("%d", cos(3.14))`.

Arguments are pushed right-to-left on top of the stack and all arguments used
in the function call are popped off the stack before the return value is pushed.


## Keywords

The caret symbol `^` can be used as a high water mark. Every argument to the left
of the caret will be preserved on the stack after the call.

The stack is available during commandline parsing. Use `@n` where `n` is an integer
to access the n-th stack item. A shortcut for `@1`, the top of the stack, is `@`

To fetch a stack item and remove it from the stack use `!n`. 

It uses [libffi](https://github.com/libffi/libffi) to be portable.
The primary data structure is `ffi_cif` which stands for ffi C interface.

The C interface describes the return value, the arguments of a function,
and the application binary interface. The ABI specifies how arguments
are pushed on the call stack and cleaned up after the function returns.

To load any C symbol at runtime from a library use

```
-llibrary symbol return type ... [abi]
```

where `return` is the type returned by the function, `type...` are the argument types,
and  abi` is an optional argument specifying the application binary interface.

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
