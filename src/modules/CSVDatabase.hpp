#pragma once

#include "CSVParser.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

/**
 * @brief Loads CSV data into an unordered_map.
 *
 * CSVDatabase uses CSVParser to load a CSV file. For each row (after skipping the header),
 * it splits the line into tokens (columns) and calls a user-supplied parsing function.
 * The parsing function must accept a std::vector<std::string> (the tokens) and return a
 * std::pair<KeyType, ElementType>.
 *
 * @tparam KeyType      Type used as the key in the map.
 * @tparam ElementType  Type stored as the value.
 */
template <typename KeyType, typename ElementType>
class CSVDatabase {
  public:
	/**
	 * @brief Constructs the database by loading a CSV file.
	 *
	 * @param filePath   Path to the CSV file.
	 * @param parseFunc  A callable with signature:
	 *                   std::pair<KeyType, ElementType>(const std::vector<std::string>&)
	 */
	template <typename RowParserType>
	CSVDatabase(const std::string &filePath, RowParserType &&parseFunc)
	{
		db = CSVParser::loadFromFile<KeyType, ElementType>(filePath, std::forward<RowParserType>(parseFunc));
	}

	const ElementType &get(const KeyType &id) const { return db.at(id); }

	std::size_t size() const { return db.size(); }

	// TODO: could we try something like this, where we let the user override an abstract parse function instead of passing a function of type RowParserType?
	// Technically this should probably be overwritten for the CSVParser, if we keep CSVParser separate.
	// static std::pair<KeyType, ElementType> parse(const std::vector<std::string> &tokens) = 0;

  private:
	std::unordered_map<KeyType, ElementType> db;
};
