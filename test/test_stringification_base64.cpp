/**
 * Test base64 encoding and decoding.
 */

#include "aas_core/aas_3_0/stringification.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;

std::string BytesToString(const std::vector<uint8_t>& bytes) {
  std::string result;
  result.resize(bytes.size());
  for (size_t i = 0; i < bytes.size(); ++i
    ) {
    result[i] = bytes[i];
  }
  return result;
}

std::vector<uint8_t> StringToBytes(const std::string& text) {
  std::vector<uint8_t> result;
  result.resize(text.size());
  for (size_t i = 0; i < text.size(); ++i
    ) {
    result[i] = text[i];
  }
  return result;
}

void AssertEncodeDecode(
  const std::string& text,
  const std::string& expected_encoded
) {
  const std::string encoded = aas::stringification::Base64Encode(
    StringToBytes(text)
  );

  REQUIRE(
    expected_encoded
      == encoded
  );

  aas::common::expected<
    std::vector<std::uint8_t>,
    std::string
  > bytes = aas::stringification::Base64Decode(encoded);

  REQUIRE(bytes.has_value());

  REQUIRE(
    BytesToString(bytes.value())
      == text
  );
}

TEST_CASE("Test '' is encoded ''") {
  AssertEncodeDecode(
    "",
    ""
  );
}

TEST_CASE("Test 'f' is encoded 'Zg=='") {
  AssertEncodeDecode(
    "f",
    "Zg=="
  );
}

TEST_CASE("Test 'fo' is encoded 'Zm8='") {
  AssertEncodeDecode(
    "fo",
    "Zm8="
  );
}

TEST_CASE("Test 'foo' is encoded 'Zm9v'") {
  AssertEncodeDecode(
    "foo",
    "Zm9v"
  );
}

TEST_CASE("Test 'foob' is encoded 'Zm9vYg=='") {
  AssertEncodeDecode(
    "foob",
    "Zm9vYg=="
  );
}

TEST_CASE("Test 'fooba' is encoded 'Zm9vYmE='") {
  AssertEncodeDecode(
    "fooba",
    "Zm9vYmE="
  );
}

TEST_CASE("Test 'foobar' is encoded 'Zm9vYmFy'") {
  AssertEncodeDecode(
    "foobar",
    "Zm9vYmFy"
  );
}

TEST_CASE("Test unexpected padding in the middle") {
  const std::string encoded = "Zm9vYmFy";
  for (size_t i = 0; i < encoded.size() - 1; ++i
    ) {
    const std::string bad_encoded(
      encoded.substr(0, i)
        + "="
        + encoded.substr(i + 1, encoded.size() - i)
    );

    aas::common::expected<
      std::vector<std::uint8_t>,
      std::string
    > bytes = aas::stringification::Base64Decode(bad_encoded);

    REQUIRE(!bytes.has_value());

    std::stringstream ss;
    ss
      << "Expected a valid character from base64-encoded string, "
      << "but got at index " << i << ": 61 (code: 61)";

    REQUIRE(
      bytes.error()
        == ss.str()
    );
  }
}

// NOTE (mristin):
// The following checks come from: https://eprint.iacr.org/2022/361.pdf
TEST_CASE("Test 'Hello' is encoded as 'SGVsbG8='") {
  AssertEncodeDecode(
    "Hello",
    "SGVsbG8="
  );
}

// NOTE (mristin):
// This is not a test case, but we merely document that there is a possible
// attack vector where different strings encode the *same* byte sequence.
TEST_CASE("Test that our implementation suffers from padding inconsistency of 'Hello' as 'SGVsbG9='") {
  const std::string encoded = "SGVsbG9=";

  aas::common::expected<
    std::vector<std::uint8_t>,
    std::string
  > bytes = aas::stringification::Base64Decode(encoded);

  REQUIRE(bytes.has_value());

  REQUIRE(
    BytesToString(bytes.value())
      == "Hello"
  );
}

void AssertDecode(
  const std::string& encoded,
  const std::vector<std::uint8_t>& expected_decoded
) {
  aas::common::expected<
    std::vector<std::uint8_t>,
    std::string
  > bytes = aas::stringification::Base64Decode(encoded);

  INFO(encoded)
  REQUIRE(bytes.has_value());

  INFO(encoded)
  REQUIRE(bytes->size() == expected_decoded.size());

  for (size_t i = 0; i < bytes->size(); ++i
    ) {
    INFO(encoded + " is not correctly decoded at " + std::to_string(i))
    REQUIRE(bytes->at(i) == expected_decoded[i]);
  }
}

// NOTE (mristin, 2022-12-02):
// The following tests have been scraped from:
// https://github.com/rickkas7/Base64RK/blob/183d20e62b96ef9b0230c80c7a172715ca6f661e/test/unit-test/unit-test.cpp
TEST_CASE("Test table from rickkas7/Base64RK") {
  AssertDecode(
    "",
    {}
  );

  AssertDecode(
    "HA==",
    {28}
  );

  AssertDecode(
    "rtA=",
    {174, 208}
  );

  AssertDecode(
    "MuDQ",
    {50, 224, 208}
  );

  AssertDecode(
    "LRLrBg==",
    {45, 18, 235, 6}
  );

  AssertDecode(
    "dceZ3aA=",
    {117, 199, 153, 221, 160}
  );

  AssertDecode(
    "jQy8MK0t",
    {141, 12, 188, 48, 173, 45}
  );

  AssertDecode(
    "RY+UfdUDlA==",
    {69, 143, 148, 125, 213, 3, 148}
  );

  AssertDecode(
    "+6KmPIGDLg0=",
    {251, 162, 166, 60, 129, 131, 46, 13}
  );

  AssertDecode(
    "zyjM/k1HPQiA",
    {207, 40, 204, 254, 77, 71, 61, 8, 128}
  );

  AssertDecode(
    "nJbVrnU7RtzSPw==",
    {156, 150, 213, 174, 117, 59, 70, 220, 210, 63}
  );

  AssertDecode(
    "wDatE35UGvqnBTI=",
    {192, 54, 173, 19, 126, 84, 26, 250, 167, 5, 50}
  );

  AssertDecode(
    "FAEPTeCNmS+6qsjj",
    {20, 1, 15, 77, 224, 141, 153, 47, 186, 170, 200, 227}
  );

  AssertDecode(
    "VaaCqcF1wlHuQLTkfw==",
    {85, 166, 130, 169, 193, 117, 194, 81, 238, 64, 180, 228, 127}
  );

  AssertDecode(
    "QYKREfdjniR1CMABweo=",
    {65, 130, 145, 17, 247, 99, 158, 36, 117, 8, 192, 1, 193, 234}
  );

  AssertDecode(
    "CTf9s8td7Tr8yKiRczUf",
    {9, 55, 253, 179, 203, 93, 237, 58, 252, 200, 168, 145, 115, 53, 31}
  );

  AssertDecode(
    "/kyMTRA0d+W64W21hiLlmesBEH4RDg0MivjmPEmAdMzP",
    {254, 76, 140, 77, 16, 52, 119, 229, 186, 225, 109, 181, 134, 34,
     229, 153, 235, 1, 16, 126, 17, 14, 13, 12, 138,
     248, 230, 60, 73, 128, 116, 204, 207}
  );

  AssertDecode(
    "m5/QqN3YcQg6iE1dVjJQWw5mCjYCnsNLKPVUsXQ269l86M1xbyHn+/aIKafJ6hRT4UOD0mH"
    "CAfye/VSHdwH+C7bQ",
    {155, 159, 208, 168, 221, 216, 113, 8, 58, 136, 77, 93, 86, 50, 80,
     91, 14, 102, 10, 54, 2, 158, 195, 75, 40, 245,
     84, 177, 116, 54, 235, 217, 124, 232, 205, 113, 111, 33, 231, 251, 246, 136, 41,
     167, 201, 234, 20, 83, 225, 67,
     131, 210, 97, 194, 1, 252, 158, 253, 84, 135, 119, 1, 254, 11, 182, 208}
  );

  AssertDecode(
    "ZSsh3WyA3d00CyimeMZPAI1/t+LF6DQdiAEQG9hHlfuDXhZ2Gf/Wd1u2y+RDFli0oSq2fbfv"
    "utgXCxGQfMmxA1NEg/k0vQIcSqzWdX7571oWzKHz6KZnlhTwPF27QznnAWbdwkyiCFlIlpL3tYbyzbMT"
    "m0ReUtY4AW9tTpvYSQdQ8w==",
    {101, 43, 33, 221, 108, 128, 221, 221, 52, 11, 40, 166, 120, 198,
     79, 0, 141, 127, 183, 226, 197, 232, 52, 29, 136,
     1, 16, 27, 216, 71, 149, 251, 131, 94, 22, 118, 25, 255, 214, 119, 91, 182, 203,
     228, 67, 22, 88, 180, 161, 42,
     182, 125, 183, 239, 186, 216, 23, 11, 17, 144, 124, 201, 177, 3, 83, 68, 131,
     249, 52, 189, 2, 28, 74, 172, 214,
     117, 126, 249, 239, 90, 22, 204, 161, 243, 232, 166, 103, 150, 20, 240, 60, 93,
     187, 67, 57, 231, 1, 102, 221, 194,
     76, 162, 8, 89, 72, 150, 146, 247, 181, 134, 242, 205, 179, 19, 155, 68, 94, 82,
     214, 56, 1, 111, 109, 78, 155,
     216, 73, 7, 80, 243}
  );

  AssertDecode(
    "seCV0jbCe42d4GMBvXIl5vqVoZ6K4gUSg6nKgi5tZiNxx22yYon5ReOC6PZpZWHqeJcfKVo"
    "UD/Za1ieLVjROriNfP+Lxf6Tbroz/EXh2YYOFI+d110oObkYqbgcl6ovS4CRH40Sb7L89Uwu9WDzQA"
    "215IEepOn1wdBCZyEryWY2PGdNcs+Pft9sSNtBQ/QtMRZYEMO5JpRmFyFDPVLZA+5xM/3Dj65odv1b"
    "ZdtszJq0Gbw43ww0t7EK0J21uBy89Z2R1N1kjQGpqKu4lRhQIgCKDOHhnfZW2gtkmZhiqby9xT44Ep1"
    "AMrmZKgW3MDPzIX2Ez16mUv3Gb5JHrI/w1sQ==",
    {177, 224, 149, 210, 54, 194, 123, 141, 157, 224, 99, 1, 189,
     114, 37, 230, 250, 149, 161, 158, 138, 226, 5, 18,
     131, 169, 202, 130, 46, 109, 102, 35, 113, 199, 109, 178, 98, 137, 249, 69, 227,
     130, 232, 246, 105, 101, 97, 234,
     120, 151, 31, 41, 90, 20, 15, 246, 90, 214, 39, 139, 86, 52, 78, 174, 35, 95, 63,
     226, 241, 127, 164, 219, 174,
     140, 255, 17, 120, 118, 97, 131, 133, 35, 231, 117, 215, 74, 14, 110, 70, 42,
     110, 7, 37, 234, 139, 210, 224, 36,
     71, 227, 68, 155, 236, 191, 61, 83, 11, 189, 88, 60, 208, 3, 109, 121, 32, 71,
     169, 58, 125, 112, 116, 16, 153,
     200, 74, 242, 89, 141, 143, 25, 211, 92, 179, 227, 223, 183, 219, 18, 54, 208,
     80, 253, 11, 76, 69, 150, 4, 48,
     238, 73, 165, 25, 133, 200, 80, 207, 84, 182, 64, 251, 156, 76, 255, 112, 227,
     235, 154, 29, 191, 86, 217, 118,
     219, 51, 38, 173, 6, 111, 14, 55, 195, 13, 45, 236, 66, 180, 39, 109, 110, 7,
     47, 61, 103, 100, 117, 55, 89, 35,
     64, 106, 106, 42, 238, 37, 70, 20, 8, 128, 34, 131, 56, 120, 103, 125, 149,
     182, 130, 217, 38, 102, 24, 170, 111,
     47, 113, 79, 142, 4, 167, 80, 12, 174, 102, 74, 129, 109, 204, 12, 252, 200,
     95, 97, 51, 215, 169, 148, 191, 113,
     155, 228, 145, 235, 35, 252, 53, 177}
  );
}
