---
trigger: model_decision
description: when working on c / c++ projects
---

# C / C++ Specific Rules

These rules apply when working on C or C++ projects.

--------------------------------------------------
1. LANGUAGE & DIALECT (HARD)
--------------------------------------------------
- Use **C** or **C++** exactly as specified by the project.
- Do not mix C and C++ unless explicitly required.
- Target the specified language standard (e.g. C11, C17, C++17, C++20).
- Do not rely on compiler-specific extensions unless explicitly stated.

--------------------------------------------------
2. PLATFORM & TOOLCHAIN CONSTRAINTS
--------------------------------------------------
- Target the specified platform, OS, architecture, and compiler.
- Do not assume availability of an operating system, standard library features,
  heap allocation, or filesystem unless stated.
- Respect platform-specific ABI, calling conventions, and alignment rules.

--------------------------------------------------
3. MEMORY & RESOURCE MANAGEMENT (CRITICAL)
--------------------------------------------------
- Be explicit about **ownership**, **lifetime**, and **allocation strategy**.
- Avoid memory leaks, double frees, use-after-free, and buffer overflows.
- Prefer deterministic resource management.
- In C++, follow RAII and avoid raw ownership unless required.
- Do not allocate dynamically in restricted or real-time contexts unless allowed.

--------------------------------------------------
4. CONCURRENCY & EXECUTION MODEL
--------------------------------------------------
- Respect the project’s concurrency model (single-threaded, RTOS, POSIX threads, ISR).
- Never assume thread safety unless explicitly guaranteed.
- Avoid blocking calls in time-critical paths (e.g. ISR, real-time loops).
- Clearly separate interrupt, real-time, and non-real-time code paths.

--------------------------------------------------
5. SAFETY, CORRECTNESS & DEFINED BEHAVIOR
--------------------------------------------------
- Avoid undefined, unspecified, or implementation-defined behavior.
- Validate all inputs that may originate outside the current module.
- Be explicit about integer sizes, signedness, and overflow behavior.
- Do not rely on implicit type conversions or side effects.

--------------------------------------------------
6. DEPENDENCIES & STANDARD LIBRARIES
--------------------------------------------------
- Use only explicitly permitted standard library components.
- Ask before introducing third-party libraries or build-time dependencies.
- Avoid hidden dependencies via headers or transitive includes.

--------------------------------------------------
7. BUILD & STRUCTURE RULES
--------------------------------------------------
- Follow the existing project structure and naming conventions.
- Do not change compiler flags, linker settings, or build scripts unless requested.
- Keep headers minimal and implementation details private.
- Avoid unnecessary macros; prefer constants, enums, or inline functions where appropriate.

--------------------------------------------------
8. OUTPUT & CHANGE CONTROL
--------------------------------------------------
- Prefer simple, predictable solutions over clever optimizations.
- Do not refactor existing code unless explicitly requested.
- Assume production, embedded, or safety-relevant deployment.
- Never fabricate register addresses, memory maps, constants, or hardware behavior.
