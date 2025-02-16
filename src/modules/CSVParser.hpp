#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CSVParser {
  public:
	CSVParser() = default;

	/**
	 * @brief Loads a CSV file and returns a map of parsed rows. It uses ',' as a delimiter.
	 *
	 * @tparam KeyType			The type of the key in the returned map.
	 * @tparam ElementType		The type of the element in the returned map.
	 * @tparam RowParserType    A callable that takes a const std::vector<std::string>& and returns std::pair<KeyType,
	 *							ElementType>. Each column is indexed, starting at 0.
	 *
	 * @param filepath   The name (and path) of the CSV file.
	 * @param parseRow   A callable used to transform a row (split into tokens) into a key/value pair.
	 * @return std::unordered_map<KeyType, ElementType> containing all parsed rows.`
	 *
	 * @throws std::runtime_error if the file cannot be opened or if the CSV is empty.
	 */
	template <typename KeyType, typename ElementType, typename RowParserType>
	static std::unordered_map<KeyType, ElementType> loadFromFile(const std::string &filepath, RowParserType &&parseRow)
	{
		std::unordered_map<KeyType, ElementType> data;
		std::ifstream file(filepath);
		if (!file) {
			throw std::runtime_error("Unable to open CSV file: " + filepath);
		}

		std::string line;
		// Optionally, skip the header line.
		if (!std::getline(file, line)) {
			throw std::runtime_error("CSV file is empty: " + filepath);
		}

		int lineNumber = 1;
		while (std::getline(file, line)) {
			++lineNumber;
			try {
				auto tokens = splitLine(line);
				auto [key, value] = parseRow(tokens);
				data.emplace(std::move(key), std::move(value));
			} catch (const std::exception &e) {
				std::cerr << "Error parsing line " << lineNumber << ": " << e.what() << " | " << line << std::endl;
			}
		}

		return data;
	}

  private:
	// Splits a CSV line into tokens using the specified delimiter.
	static std::vector<std::string> splitLine(const std::string &line, char delimiter = ',')
	{
		std::vector<std::string> tokens;
		std::istringstream tokenStream(line);
		std::string token;
		while (std::getline(tokenStream, token, delimiter)) {
			tokens.push_back(trim(token));
		}
		return tokens;
	}

	// Returns a trimmed copy of the string (removes leading and trailing whitespace).
	static std::string trim(const std::string &s)
	{
		auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); });
		auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
		return (start < end ? std::string(start, end) : std::string());
	}
};
