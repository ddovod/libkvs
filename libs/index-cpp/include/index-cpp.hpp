
#pragma once

#include <functional>
#include <unordered_set>
#include <unordered_map>

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning(disable:4996)
#pragma warning(disable:4355)
#pragma warning(disable:4244)
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wconversion"
#   pragma clang diagnostic ignored "-Wshorten-64-to-32"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wconversion"
#   pragma GCC diagnostic ignored "-Wsign-compare"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "../src/linear_hashing_table.h"

#ifdef _MSC_VER
#pragma warning ( pop )
#endif

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
