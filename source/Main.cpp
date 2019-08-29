#include "Renderer.hpp"
#include "Utils.hpp"

#include <iostream>

int main() {
    Renderer app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
