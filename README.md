# C++ From Scratch

An independent, educational implementation of a C++ compiler, built from first principles. The long-term goal is a self-hosting compiler for a documented, progressively expanding subset of ISO C++ — not a wrapper around Clang or GCC.

> **Status: bootstrap stage.** This repository currently contains a small C++-syntax interpreter used to validate the front end. It is not a complete or standards-conforming C++ compiler, and it does not yet emit native machine code.

## Build

Requirements: CMake 3.20+ and a C++20 compiler (GCC 11+, Clang 14+, or MSVC 19.30+).

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/cxx examples/hello.cpp
```

Run the smoke tests:

```sh
ctest --test-dir build --output-on-failure
```

## First milestone

The bootstrap interpreter supports integer and floating-point declarations, arithmetic, assignment, parentheses, `std::cout`, `std::endl`, and semicolon-terminated statements. Example:

```cpp
int base = 40;
int answer = base + 2;
std::cout << "The answer is " << answer << std::endl;
```

## Architecture and roadmap

1. **Lexing** — source text to tokens with source locations.
2. **Parsing** — recursive-descent statements and Pratt expressions.
3. **Semantic analysis** — scopes, symbol tables, type checking and diagnostics.
4. **Intermediate representation** — typed, target-independent IR.
5. **Code generation** — start with a documented target (x86-64 System V), then add targets.
6. **Runtime and standard library** — ABI, object model, exceptions, containers and library facilities.
7. **Compatibility** — templates, overload resolution, classes, inheritance, lambdas, constexpr, modules and the wider language.
8. **Self-hosting** — compile the compiler with itself.

Each stage will have tests and explicit compatibility notes. C++ is a very large standardised language; compatibility will be claimed feature-by-feature, never implied by the repository name.

## Project layout

- `src/main.cpp` — bootstrap lexer, parser and evaluator.
- `examples/` — small programs supported by the bootstrap.
- `tests/` — command-line smoke tests.
- `docs/` — design notes and implementation milestones.

## Contributing

Open an issue before making a large architectural change. Keep the supported-language document and tests in sync with every feature.

## License

MIT. See [LICENSE](LICENSE).