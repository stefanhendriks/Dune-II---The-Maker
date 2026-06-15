# Coding Conventions

## Brace style

Always expand if/else/else-if bodies to multiple lines. Never compress to a one-liner, even for a single statement.

```cpp
// correct
if (condition) {
    statement;
} else if (other) {
    statement;
}

// wrong
if (condition) { statement; } else { statement; }
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
