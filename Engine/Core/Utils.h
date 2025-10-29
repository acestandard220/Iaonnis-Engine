#pragma once
#include "pch.h"

namespace Iaonnis {
	template <typename T>
	void removeFromVector(std::vector<T>& vec, const T& value)
	{
		vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
	}


	static std::string FormatBytes(size_t bytes)
	{
		static const double KB = 1024.0;
		static const double MB = KB * 1024.0;
		static const double GB = MB * 1024.0;

		if (bytes == 0)
			return "No Disk Write";

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2);

		if (bytes < KB)
			oss << bytes << " B";
		else if (bytes < MB)
			oss << (bytes / KB) << " KB";
		else if (bytes < GB)
			oss << (bytes / MB) << " MB";
		else
			oss << (bytes / GB) << " GB";

		return oss.str();
	}

	/// <summary>
	/// Get Epoch of past from now.
	/// </summary>
	/// <param name="storedTime"></param>
	/// <returns></returns>
	static std::string DecodeFileTime(uint64_t storedTime)
	{
		if (storedTime == 0)
			return "Unknown";

		std::time_t t = static_cast<std::time_t>(storedTime);
		char buffer[32];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
		return buffer;
	}

	static uint64_t GetEpoch(int days)
	{
		auto now = std::chrono::system_clock::now();

		auto last_week = now - std::chrono::hours(24 * days);

		auto epoch = std::chrono::duration_cast<std::chrono::seconds>(
			last_week.time_since_epoch()).count();

		return epoch;
	}

	namespace filespace
	{
		std::filesystem::path generateDuplicateFilename(std::filesystem::path path);

		using filepath = std::filesystem::path;
		using filelastwrite = std::filesystem::file_time_type;

		/// <summary>
		/// Removes directory or file
		/// </summary>
		void remove(std::filesystem::path path);

		/// <summary>
		/// Creates a new folder/directory.
		/// No need to inclde folder name.
		/// </summary>
		/// <param name="path">
		/// Path to create new folder without folder name.
		/// </param>
		void createDirectory(std::filesystem::path path);

		/// <summary>
		/// Returns a list of all directory entries in a path
		/// </summary>
		std::vector<std::filesystem::directory_entry> getDirectories(filepath path);

		/// <summary>
		/// Returns the stem (filename without extension) of the specified file path.
		/// </summary>
		std::string getStem(filepath path);

		/// <summary>
		/// Retrieves the file extension from a given file path.
		/// </summary>
		/// <param name="path">The file path from which to extract the extension.</param>
		/// <returns>
		/// A string containing the file extension, including the leading dot if present. 
		/// Returns an empty string if the file has no extension.
		/// </returns>
		std::string getExtension(filepath path);

		/// <summary>
		/// Determines the type of a file based on its path.
		/// </summary>
		/// <param name="path">The path to the file whose type is to be determined.</param>
		/// <returns>A string representing the file type (e.g., extension or MIME type).</returns>
		std::string getFileType(filepath path);


		/// <summary>
		/// Retrieves the last modified timestamp of a file at the specified path.
		/// </summary>
		/// <param name="path">The path to the file whose last modified time is to be retrieved.</param>
		/// <returns>The last modified timestamp of the file, typically as a time or date-time object.</returns>
		std::string getLastModified(filepath path);

		/// <summary>
		/// Returns the size of the file at the specified path as a string.
		/// </summary>
		/// <param name="path">The path to the file whose size is to be retrieved.</param>
		/// <returns>A string representing the size of the file.</returns>
		std::string getFileSize(filepath path);

		/// <summary>
		/// Retrieves the size of the file at the specified path in bytes.
		/// </summary>
		/// <param name="path">The filesystem path to the file whose size will be retrieved.</param>
		/// <returns>The size of the file in bytes.</returns>
		size_t getByteSize(filepath path);

		/// <summary>
		/// Checks if the specified file or directory exists at the given path.
		/// </summary>
		/// <param name="path">The path to check for existence.</param>
		/// <returns>True if the file or directory exists, false otherwise.</returns>
		bool exists(filepath path);


		uint64_t GetFileWriteTime(const filepath& filePath);
	}
}