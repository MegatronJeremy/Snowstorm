#pragma once

#include <filesystem>

namespace Snowstorm
{
	static std::filesystem::file_time_type GetLastWriteTime(const std::string& filepath)
	{
		return std::filesystem::last_write_time(filepath);
	}
}
