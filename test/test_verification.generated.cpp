// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_verification.py
// Do NOT edit or append.

#include "./common.hpp"
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/verification.hpp>
#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;

void AssertNoVerificationError(
  const std::filesystem::path& xml_path
) {
  std::shared_ptr<
    aas::types::IClass
  > instance(
    test::common::xmlization::MustDeserializeFile(xml_path)
  );

  std::vector<std::string> error_messages;
  for (
    const aas::verification::Error &error
    : aas::verification::RecursiveVerification(instance)
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

const std::filesystem::path& DetermineXmlDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "Xml";
  }

  return *result;
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

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid AdministrativeInformation") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "administrativeInformation",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Qualifier") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "qualifier",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid AssetAdministrationShell") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "assetAdministrationShell",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid AssetInformation") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "assetInformation",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Resource") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "resource",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid SpecificAssetId") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "specificAssetId",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Submodel") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "submodel",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid RelationshipElement") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "relationshipElement",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid SubmodelElementList") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "submodelElementList",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid SubmodelElementCollection") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "submodelElementCollection",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Property") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "property",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid MultiLanguageProperty") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "multiLanguageProperty",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Range") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "range",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid ReferenceElement") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "referenceElement",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Blob") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "blob",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid File") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "file",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid AnnotatedRelationshipElement") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "annotatedRelationshipElement",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Entity") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "entity",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid EventPayload") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "SelfContained"
        / "Expected"
        / "eventPayload",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid BasicEventElement") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "basicEventElement",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Operation") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "operation",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid OperationVariable") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "operationVariable",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Capability") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "capability",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid ConceptDescription") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "conceptDescription",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Reference") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "reference",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Key") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "key",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LangStringNameType") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "langStringNameType",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LangStringTextType") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "langStringTextType",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid Environment") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "SelfContained"
        / "Expected"
        / "environment",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid EmbeddedDataSpecification") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "embeddedDataSpecification",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LevelType") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "levelType",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid ValueReferencePair") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "valueReferencePair",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid ValueList") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "valueList",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LangStringPreferredNameTypeIec61360") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "langStringPreferredNameTypeIec61360",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LangStringShortNameTypeIec61360") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "langStringShortNameTypeIec61360",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid LangStringDefinitionTypeIec61360") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "langStringDefinitionTypeIec61360",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

TEST_CASE("Test verification of a valid DataSpecificationIec61360") {
  const std::deque<std::filesystem::path> paths(
    test::common::FindFilesBySuffixRecursively(
      DetermineXmlDir()
        / "ContainedInEnvironment"
        / "Expected"
        / "dataSpecificationIec61360",
      ".xml"
    )
  );

  for (const std::filesystem::path& path : paths) {
    AssertNoVerificationError(path);
  }
}

// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_verification.py
// Do NOT edit or append.
