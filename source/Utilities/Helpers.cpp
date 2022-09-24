#include <Utilities/Helpers.hpp>

std::string ReadShaderFile(std::string const& shaderName)
{
    std::string const fileName = StringFormat("%s/%s.spv", SPIRV_SHADER_PATH, shaderName.c_str());
    return ReadFile(fileName);
}

std::string ReadFile(std::string const& fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        ThrowError("Failed to open file: %s.", fileName.c_str());
    }

    size_t const fileSize = (size_t)file.tellg();
    std::string buffer(fileSize, 0);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
