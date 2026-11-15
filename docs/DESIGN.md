# Design notes

## Compiler pipeline

The project is intended to grow through explicit phases: source manager and diagnostics; lexer; parser and AST; name resolution and type checking; typed intermediate representation; optimisations; target-specific code generation; linker/runtime integration.

The bootstrap executable intentionally combines lexing, parsing and evaluation in one translation unit to keep the first milestone easy to inspect. This is temporary. The next refactor should split tokens/lexer, AST/parser, diagnostics, semantic analysis and interpreter into independently tested modules.

## Design constraints

- Do not invoke GCC or Clang as a hidden compilation backend.
- Keep the accepted grammar documented and tested.
- Prefer deterministic diagnostics with source locations.
- Add language features in vertical slices: syntax, semantic rules, runtime/codegen, tests and docs.
- Avoid claiming ISO C++ conformance until a conformance suite supports the claim.