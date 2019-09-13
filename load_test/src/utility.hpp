
#pragma once

#include <argh.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

template <typename T>
bool parseParam(argh::parser& args, const std::string& paramName, T& value);

std::string generatePath();

template <typename T>
typename T::value_type getRandomElement(const T& container);

#include "utility.inl"
