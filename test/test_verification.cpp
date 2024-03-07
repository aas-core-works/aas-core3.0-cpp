#include "./common.hpp"

#include <aas_core/aas_3_0/verification.hpp>
#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;


TEST_CASE("Test IsXsDate on a common value") {
  CHECK(aas::verification::IsXsDate(L"2022-04-01-02:00"));
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

