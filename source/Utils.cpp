#include "Utils.hpp"


Settings g_settings;

bool QueueFamilyIndices::IsComplete() const
{
    return m_graphicsFamily.has_value() && m_presentFamily.has_value();
}

namespace Utils
{
	std::string ReadFile(std::string const& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t const fileSize = (size_t)file.tellg();
		std::string buffer(fileSize, 0);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}

