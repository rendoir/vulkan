- Render graph
    - Unify textures and attachments in render graph according to usage
        - Map of images and their usage -> create attachment and texture from it?
    - Reorder passes to fulfil dependencies
    - (Resolve+)Blit pass instead of hardcoded

- Stronger system execution order
    - Scheduler
        - Read/Write directives in compile time
        - Multithreading

- CMake: memory leak analyser (valgrind) / static code analyser (cppcheck) / linter (clang-tidy) / include-what-you-use

- Build a Resource Manager cache to avoid loading multiple times the same 
    - Textures, Materials, Meshes, Models (?)

- Use a transfer queue for transfer operations
