#pragma once

#include <string>

#include "database.h"

namespace join
{

std::string intersection(const Database::TableMap &a, const Database::TableMap &b);
std::string symmetricDifference(const Database::TableMap &a, const Database::TableMap &b);

}
