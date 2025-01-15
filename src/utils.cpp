#include "utils.hpp"

#include <iostream>

int utils::generateId()
{
    static int generatedId = 0;
    return generatedId++;
}