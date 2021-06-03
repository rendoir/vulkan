#include "Utils.hpp"


Settings g_settings;

bool QueueFamilyIndices::IsComplete() const
{
    return m_graphicsFamily.has_value() && m_presentFamily.has_value();
}

std::vector<char> ReadFile(std::string const& filename) {
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
