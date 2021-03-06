//
// Unit Tests: JSON
//
// Description: JSON stringification exceptions unit
// tests for JSON class using the Catch2 test framework.
//
// ================
// Test definitions
// =================
#include "JSONLib_Tests.hpp"
// ======================
// JSON library namespace
// ======================
using namespace JSONLib;
// ==========
// Test cases
// ==========
// ====================================
// Stringification generated exceptions
// ====================================
TEST_CASE("Stringify generated exceptions", "[JSON][Stringify][Exceptions]") {
  const JSON json;
  SECTION("Stringify has no JSON to process", "[JSON][Stringify][Exceptions]") {
    REQUIRE_THROWS_AS(json.stringify(BufferDestination{}), JSONLib::Error);
    REQUIRE_THROWS_WITH(json.stringify(BufferDestination{}),
                        "JSON Error: No JSON to stringify.");
  }
}