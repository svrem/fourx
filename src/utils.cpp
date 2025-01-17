#include "utils.hpp"

#include <iostream>

int utils::generateId()
{
    static int generatedId = 0;
    return generatedId++;
}

std::random_device utils::rd;
std::mt19937 utils::gen(utils::rd());