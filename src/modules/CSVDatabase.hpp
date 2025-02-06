#pragma once

#include "CSVParser.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

// A generic database that loads data from a CSV file into an unordered_map.
template <typename KeyType, typename ElementType>
class CSVDatabase {
  public:
	template <typename RowParserType>
	CSVDatabase(const std::string &filePath, RowParserType &&parseFunc)
	{
		db = CSVParser::loadFromFile<KeyType, ElementType>(filePath, std::forward<RowParserType>(parseFunc));
	}

	const ElementType &get(const KeyType &id) const { return db.at(id); }

	std::size_t size() const { return db.size(); }

  private:
	std::unordered_map<KeyType, ElementType> db;
};
