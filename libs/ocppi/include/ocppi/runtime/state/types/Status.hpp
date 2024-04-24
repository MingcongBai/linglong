// Thish file is generated by /tools/codegen
// DO NOT EDIT IT.

// clang-format off

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Status.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <nlohmann/json.hpp>
#include "ocppi/runtime/state/types/helper.hpp"

namespace ocppi {
namespace runtime {
namespace state {
namespace types {
using nlohmann::json;

enum class Status : int { Created, Creating, Running, Stopped };
}
}
}
}

// clang-format on