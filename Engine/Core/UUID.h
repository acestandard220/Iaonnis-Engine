#pragma once
#include "pch.h"

namespace Iaonnis {
	struct UUID
	{
		uint64_t high;
		uint64_t low;

		bool operator==(const UUID& other) const
		{
			return high == other.high && low == other.low;
		}

		bool operator!=(const UUID& other) const
		{
			return !(*this == other);
		}

		bool operator<(const UUID& other) const
		{
			return (high < other.high) || (high == other.high && low < other.low);
		}
	};

	class UUIDFactory
	{
	public:
		static UUID generateUUID();
		static std::string uuidToString(const UUID& uuid);
		static UUID uuidFromString(const std::string& s);
		static UUID getInvalidUUID() { return invalid; }
	private:
		static UUID invalid;
	};
}
namespace std
{
	template<>
	struct hash<Iaonnis::UUID>
	{
		std::size_t operator()(const Iaonnis::UUID& uuid) const noexcept
		{
            // Combine the two 64-bit integers into one hash
			return std::hash<uint64_t>{}(uuid.high) ^ (std::hash<uint64_t>{}(uuid.low) << 1);
		}
	};
}
