#pragma once

#include <string>

#include "database.h"

namespace join
{


std::string intersection(const Database::Snapshot &snapshot);
std::string symmetricDifference(const Database::Snapshot &snapshot);

}
