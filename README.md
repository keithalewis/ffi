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
The types are the standard names for C types: `void` (only for return), `int`, `float`, `double`,
`int<n>_t`, `uint<n>_t`, and `void*` for any pointer type. Each element of the stack is
a variant of these types.

To use a temporary `cif` based on stack arguments use `name@<n>` where `<n>` is the number
of stack arguments to use. Use `name!<n>` to also replace the `name` in the dictionary
with the new `cif`. All variadic functions must be called using `name@<n>.<m>`
where `<n>` is the number of fixed args and `<m>` is the number of variadic args.
