# Bootstrap language support

This document describes the deliberately small language accepted by the current interpreter.

## Supported

- `int`, `float`, and `double` variable declarations with an initializer.
- Numeric literals and string literals.
- `+`, `-`, `*`, `/`, unary minus, and parentheses.
- Variable assignment after declaration.
- `std::cout << expression << ...;` and `std::endl`.
- `//` line comments.

## Not yet supported

This is not general C++. There are no functions, control-flow statements, scopes, pointers, references, arrays, structs/classes, templates, headers, preprocessor, overloads, exceptions, standard library implementation, ABI, or native code generation. Type names are currently parsed for syntax only; values are represented as a numeric-or-string runtime value.

The list above is the source of truth for the bootstrap milestone. Update it alongside implementation and tests.