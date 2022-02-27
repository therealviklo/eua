#pragma once
#include <string>
#include <vector>
#include "utils.h"
#include "file.h"
#include "shithash.h"
#include "chacha20.h"

bool checkIfPasswordIsValidForFile(const std::string& filename, const std::string& password);