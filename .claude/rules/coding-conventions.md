# Coding Conventions

## Brace style

Use braces for if/else/else-if bodies. The only one-liner allowed is a true
single line — condition and statement together, no line break — and that's
reserved for early returns/exits (guard clauses), not real logic.

```cpp
// correct
if (condition) {
    statement;
} else if (other) {
    statement;
}

// acceptable — early return/exit only, condition and statement on one line
if (condition) return;
if (!isValid) continue;

// wrong — braces compressed onto one line
if (condition) { statement; } else { statement; }

// wrong — condition and statement split across two lines without braces
if (condition)
    statement;
```

## std::optional comparisons

`std::optional<T>` supports `operator==` and `operator!=` with `T` directly. Compare without dereferencing.

```cpp
// correct
if (opt != value) { ... }

// wrong
if (*opt != value) { ... }
```

Only dereference (`*opt` or `opt.value()`) when you need to pass the contained value to a function expecting `T`.

## cAbstractStructure ID accessor

Use `getStructureId()` to get the numeric ID of a structure instance. Do not access `.id` directly or call `.getId()`.

```cpp
// correct
structure->getStructureId()

// wrong
structure->id
structure->getId()
```
