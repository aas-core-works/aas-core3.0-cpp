#include "./common.hpp"
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/verification.hpp>
#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// TODO: remove
#include <iostream>
#include <fstream>

namespace aas = aas_core::aas_3_0;

void AssertNoVerificationError(
  const std::filesystem::path &xml_path
) {
  std::shared_ptr<
    aas::types::IClass
  > instance(
    test::common::xmlization::MustDeserializeFile(xml_path)
  );

  std::vector<std::string> error_messages;
  for (
    const aas::verification::Error &error
    :  aas::verification::RecursiveVerification(instance)
    ) {
    error_messages.emplace_back(
      aas::common::Concat(
        aas::common::WstringToUtf8(error.path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(error.cause)
      )
    );
  }

  if (!error_messages.empty()) {
    std::vector<std::string> parts;
    parts.emplace_back("Expected no error messages from ");
    parts.emplace_back(xml_path.string());
    parts.emplace_back(", but got:\n");
    for (std::string& error_message : error_messages) {
      parts.emplace_back(error_message);
    }

    INFO(test::common::JoinStrings(parts, ""))
    CHECK(error_messages.empty());
  }
}

void AssertOneOrMoreVerificationErrors(
  const std::filesystem::path &xml_path,
  const std::filesystem::path &error_path
) {
  std::shared_ptr<
    aas::types::IClass
  > instance(
    test::common::xmlization::MustDeserializeFile(xml_path)
  );

  std::vector<std::string> error_messages;
  for (
    const aas::verification::Error &error
    :  aas::verification::RecursiveVerification(instance)
    ) {
    error_messages.emplace_back(
      aas::common::Concat(
        aas::common::WstringToUtf8(error.path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(error.cause)
      )
    );
  }

  if (error_messages.empty()) {
    INFO(
      aas::common::Concat(
        "Expected one or more error messages from ",
        xml_path.string(),
        ", but got none"
      )
    )
    CHECK(!error_messages.empty());
  }

  std::string got = test::common::JoinStrings(error_messages, "\n") + "\n";

  test::common::AssertContentEqualsExpectedOrRecord(got, error_path);
}

const std::filesystem::path& DetermineXmlDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "Xml";
  }

  return *result;
}

const std::filesystem::path& DetermineErrorDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "VerificationError";
  }

  return *result;
}

const std::vector<std::string> kCausesForVerificationFailure = {
  "DateTimeStampUtcViolationOnFebruary29th",
  "MaxLengthViolation",
  "MinLengthViolation",
  "PatternViolation",
  "InvalidValueExample",
  "InvalidMinMaxExample",
  "SetViolation",
  "ConstraintViolation"
};

// NOTE (mristin):
// We test later in the further tests many more values, but the following unit tests
// make the debugging a bit easier.

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

TEST_CASE("Test IsXsDouble") {
  CHECK(aas::verification::IsXsDouble(L"+76E-86"));
}

TEST_CASE("Test verification of a valid Extension") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "extension",
      ".xml"
    )
  );

  std::ofstream ofs("c:\\users\\rist\\tmp\\log.txt");
  for (const std::filesystem::path &path : paths) {

    ofs << "path: " << path << std::endl;
    ofs.flush();

    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of an invalid Extension") {
  for (const std::string &cause : kCausesForVerificationFailure) {
    const std::deque<std::filesystem::path> paths(
      test::common::FindFilesBySuffixRecursively(
        DetermineXmlDir()
          / "ContainedInEnvironment"
          / "Unexpected"
          / cause
          / "extension",
        ".xml"
      )
    );

    for (const std::filesystem::path &path : paths) {
      const std::filesystem::path parent(
        (
          DetermineErrorDir()
            / std::filesystem::relative(path, DetermineXmlDir())
        ).parent_path()
      );

      const std::filesystem::path error_path(
        parent
          / (path.filename().string() + ".error")
      );

      AssertOneOrMoreVerificationErrors(
        path,
        error_path
      );
    }
  }
}
