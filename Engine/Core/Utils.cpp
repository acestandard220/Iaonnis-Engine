#include "Utils.h"

namespace Iaonnis {
	std::filesystem::path filespace::generateDuplicateFilename(std::filesystem::path path)
	{
		std::filesystem::path parentDirectory = path.parent_path();
		std::string extension = path.extension().string();
		std::string fileName = path.stem().string();
		std::filesystem::path newPath = path;

		size_t count = 1;
		while (std::filesystem::exists(newPath))
		{
			std::string newName = fileName + "(" + std::to_string(count) + ")" + extension;
			newPath = parentDirectory / newName;
			count++;
		}
		return newPath;
	}

	/// <summary>
	/// Removes directory or file
	/// </summary>
	void filespace::remove(std::filesystem::path path)
	{
		if (std::filesystem::exists(path))
		{
			auto directory = std::filesystem::directory_entry(path);
			if (directory.is_directory())
				std::filesystem::remove_all(path);
			else
				std::filesystem::remove(path);
		}
	}

	/// <summary>
	/// Creates a new folder/directory.
	/// No need to inclde folder name.
	/// </summary>
	/// <param name="path">
	/// Path to create new folder without folder name.
	/// </param>
	void filespace::createDirectory(std::filesystem::path path)
	{
		std::filesystem::path newDirectory = path / "New Folder";

		if (std::filesystem::exists(newDirectory))
		{
			newDirectory = generateDuplicateFilename(newDirectory);
		}

		std::filesystem::create_directory(newDirectory);
	}

	std::vector<std::filesystem::directory_entry> filespace::getDirectories(filepath path)
	{
		std::vector<std::filesystem::directory_entry> dirs;
		for (auto& directory : std::filesystem::directory_iterator(path))
		{
			dirs.push_back(directory);
		}
		return dirs;
	}

	std::string filespace::getStem(filepath path)
	{
		return path.stem().string();
	}

	std::string filespace::getExtension(filepath path)
	{
		return path.extension().string();
	}

	std::string filespace::getFileType(filepath path)
	{
		auto ext = getExtension(path);
		if (ext.empty())
			return "";
		ext = ext.substr(1);

		std::transform(ext.begin(), ext.end(), ext.begin(), toupper);
		return ext;
	}

	std::string filespace::getLastModified(filepath path)
	{
		std::filesystem::directory_entry directory(path);
		auto ftime = directory.last_write_time();

		// Convert file_clock -> system_clock
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
		);

		std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
		std::tm* time_info = std::localtime(&cftime);

		std::stringstream ss;
		ss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");

		return ss.str();
	}


	std::string filespace::getFileSize(filepath path)
	{
		std::filesystem::directory_entry dir(path);
		auto fileSize = dir.file_size();

		std::stringstream ss;
		if (fileSize < 1024)
			ss << fileSize << " B";
		else if (fileSize < 1024 * 1024)
			ss << (fileSize / 1024.0) << " KB";
		else
			ss << (fileSize / (1024.0 * 1024.0)) << " MB";

		return ss.str();
	}

	bool filespace::exists(filepath path)
	{
		return std::filesystem::exists(path);
	}

}