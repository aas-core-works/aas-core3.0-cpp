#include <aas_core/aas_3_0/verification.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>


namespace aas = aas_core::aas_3_0;

// NOTE (mristin):
// We test later in the further tests many more values, but the following unit tests
// make the debugging a bit easier.

TEST_CASE("Test IsXsDate on a common value") {
  CHECK(aas::verification::IsXsDate(L"2022-04-01-02:00"));
}

TEST_CASE("Test IsXsDate on an obviously invalid year") {
  CHECK(!aas::verification::IsXsDate(L"wrong-04-01-02:00"));
}

TEST_CASE("Test IsXsDate on an obviously invalid month") {
  CHECK(!aas::verification::IsXsDate(L"2022-wrong-01-02:00"));
}

TEST_CASE("Test IsXsDate on an obviously invalid day") {
  CHECK(!aas::verification::IsXsDate(L"2022-04-wrong-02:00"));
}

TEST_CASE("Test IsXsDate on a large negative year") {
  CHECK(aas::verification::IsXsDate(L"-12345678901234567890123456789012345678901234567890-04-01"));
}

TEST_CASE("Test IsXsDate on a BC 5 as a leap year") {
  CHECK(aas::verification::IsXsDate(L"-0005-02-29"));
}

TEST_CASE("Test IsXsDateTime") {
  CHECK(aas::verification::IsXsDateTime(L"-0811-10-21T24:00:00.000000Z"));
}

TEST_CASE("Test IsXsDouble") {
  CHECK(aas::verification::IsXsDouble(L"+76E-86"));
}
