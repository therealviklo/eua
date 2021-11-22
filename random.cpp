#include "random.h"

std::mt19937 rengine(std::random_device{}());
std::uniform_int_distribution<int> uid;