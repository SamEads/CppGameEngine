#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

namespace StringTools
{
	static const std::string allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!_";
	static const std::string allowedSignCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?@#$%^&*()-=_+[]{}\\|;:'\"'/.,<> ";

	inline std::string trim(const std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		if (std::string::npos == first)
		{
			return str;
		}

		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	inline bool contains(const std::string& str, const std::string& substr)
	{
		return str.find(substr) != std::string::npos;
	}

	inline std::string lower(std::string str)
	{
		if (str.empty())
		{
			return str;
		}
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	inline std::string upper(std::string str)
	{
		if (str.empty())
		{
			return str;
		}
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	inline int parseInt(const std::string& str)
	{
		if (str.empty())
		{
			return INT_MIN;
		}

		std::string intStr;
		for (char c : str)
		{
			if (std::isdigit(c))
			{
				intStr.push_back(c);
			}
		}

		return (!intStr.empty()) ? stoi(intStr) : INT_MIN;
	}

	// Replaces any instance of one character in a string with a different char.
	// I.E.: "Tomato" passed with "o" as oldChar and "e" as newChar would create "Temate"
	inline std::string replace(const std::string& str, const char oldChar, const char newChar)
	{
		std::string newStr;
		newStr.reserve(str.size());
		for (auto& chr : str)
		{
			newStr.push_back((chr == oldChar) ? newChar : chr);
		}
		return newStr;
	}
}