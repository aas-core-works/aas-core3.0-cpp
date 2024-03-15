#include "./common.hpp"
#include "./common_examples.generated.hpp"
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <deque>

namespace aas = aas_core::aas_3_0;

const std::filesystem::path& DetermineXmlDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "Xml";
  }

  return *result;
}

const std::filesystem::path& DetermineDescentDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "Descent";
  }

  return *result;
}

const std::filesystem::path& DetermineDescentOnceDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "DescentOnce";
  }

  return *result;
}

void AssertOrRerecordDescent(
  const std::filesystem::path& xml_path,
  const std::filesystem::path& trace_path
) {
  std::shared_ptr<
    aas::types::IClass
  > instance(
    test::common::xmlization::MustDeserializeFile(xml_path)
  );

  std::deque<std::wstring> parts;

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(instance)
    ) {
    parts.emplace_back(test::common::TraceMark(*something));
    parts.push_back(L"\n");
  }

  test::common::AssertContentEqualsExpectedOrRecord(
    aas::common::WstringToUtf8(
      test::common::JoinWstrings(parts, L"")
    ),
    trace_path
  );
}

void AssertOrRerecordDescentOnce(
  const std::shared_ptr<aas::types::IClass>& instance,
  const std::filesystem::path& trace_path
) {
  std::deque<std::wstring> parts;

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::DescentOnce(instance)
    ) {
    parts.emplace_back(test::common::TraceMark(*something));
    parts.push_back(L"\n");
  }

  test::common::AssertContentEqualsExpectedOrRecord(
    aas::common::WstringToUtf8(
      test::common::JoinWstrings(parts, L"")
    ),
    trace_path
  );
}

TEST_CASE("Test Descent over an Extension") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "extension",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    const std::filesystem::path parent(
      (
        DetermineDescentDir()
          / std::filesystem::relative(path, DetermineXmlDir())
      ).parent_path()
    );

    const std::filesystem::path trace_path(
      parent
        / (path.filename().string() + ".trace")
    );

    AssertOrRerecordDescent(path, trace_path);
  }
}

TEST_CASE("Test DescentOnce over an Extension") {
  const std::shared_ptr<
    aas::types::IExtension
  > instance(
    test::common::examples::LoadMaxExtension()
  );

  const std::filesystem::path trace_path(
    DetermineDescentOnceDir()
      / (
        aas::common::Concat(
          "Max",
          aas::stringification::to_string(instance->model_type()),
          ".trace"
        )
      )
  );

  AssertOrRerecordDescentOnce(instance, trace_path);
}
