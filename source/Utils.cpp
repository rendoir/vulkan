#include "Utils.hpp"

#include <fstream>

Settings settings;

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

glm::vec3 toOrbital(glm::vec3 position, glm::vec3 rotation) {
    return glm::vec3(
        -position.z * sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
        position.z * sin(glm::radians(rotation.x)),
        position.z * cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))
    );
}
