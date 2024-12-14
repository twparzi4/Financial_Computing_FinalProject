/*
This file defines container classes built on STL containers.
*/

#pragma once
#include <vector>
#include <map>
#include "Stock.h"

typedef std::map<std::string, Stock> StocksGroup; // Stocks of Russell 3000 will be stored in this map container. Each group has one StocksGroup instance.
typedef std::pair<std::string, Stock> Pair;
