---
trigger: always_on
---

# Commenting Guidelines

## Purpose
Comments explain **intent and reasoning**, not code behavior.

## Add comments for
- **Why** the code exists or why an approach was chosen
- **Tricky or non-obvious logic**
- **Assumptions, constraints, side effects**
- **TODOs** for concrete, actionable improvements

## Do NOT comment
- Obvious or trivial code
- Syntax or language features
- What can be understood from good naming

## Comment types
- **Inline / block comments**: explain *why*, constraints, edge cases
- **Documentation comments**: for all public or non-trivial functions, classes, or modules

## Documentation format
Use the standard format of the target language:
- Java / Android → Javadoc
- JavaScript / TypeScript → JSDoc
- C / C++ → Doxygen
- Other languages → closest native standard

## Style rules
- Be concise and precise
- Focus on **why**, not **how**
- Keep comments in sync with code
- If a comment explains *what*, improve the code instead

