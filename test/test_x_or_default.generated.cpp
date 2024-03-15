// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_x_or_default.py
// Do NOT edit or append.

#include "./common.hpp"
#include "./common_examples.generated.hpp"

#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;

const std::filesystem::path& DetermineLogDir() {
  static aas::common::optional<std::filesystem::path> result;
  if (!result.has_value()) {
    result = test::common::DetermineTestDataDir() / "XxxOrDefault";
  }

  return *result;
}

TEST_CASE("Test ValueTypeOrDefault on a min. Extension") {
  const std::shared_ptr<
    aas::types::IExtension
  > instance(
    test::common::examples::LoadMinExtension()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Extension.ValueTypeOrDefault.min.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->ValueTypeOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test ValueTypeOrDefault on a max. Extension") {
  const std::shared_ptr<
    aas::types::IExtension
  > instance(
    test::common::examples::LoadMaxExtension()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Extension.ValueTypeOrDefault.max.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->ValueTypeOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test KindOrDefault on a min. Qualifier") {
  const std::shared_ptr<
    aas::types::IQualifier
  > instance(
    test::common::examples::LoadMinQualifier()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Qualifier.KindOrDefault.min.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->KindOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test KindOrDefault on a max. Qualifier") {
  const std::shared_ptr<
    aas::types::IQualifier
  > instance(
    test::common::examples::LoadMaxQualifier()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Qualifier.KindOrDefault.max.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->KindOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test KindOrDefault on a min. Submodel") {
  const std::shared_ptr<
    aas::types::ISubmodel
  > instance(
    test::common::examples::LoadMinSubmodel()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Submodel.KindOrDefault.min.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->KindOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test KindOrDefault on a max. Submodel") {
  const std::shared_ptr<
    aas::types::ISubmodel
  > instance(
    test::common::examples::LoadMaxSubmodel()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Submodel.KindOrDefault.max.log"
  );

  const std::string serialized(
    aas::stringification::to_string(
      instance->KindOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test OrderRelevantOrDefault on a min. SubmodelElementList") {
  const std::shared_ptr<
    aas::types::ISubmodelElementList
  > instance(
    test::common::examples::LoadMinSubmodelElementList()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "SubmodelElementList.OrderRelevantOrDefault.min.log"
  );

  const std::string serialized(
    std::to_string(
      instance->OrderRelevantOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test OrderRelevantOrDefault on a max. SubmodelElementList") {
  const std::shared_ptr<
    aas::types::ISubmodelElementList
  > instance(
    test::common::examples::LoadMaxSubmodelElementList()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "SubmodelElementList.OrderRelevantOrDefault.max.log"
  );

  const std::string serialized(
    std::to_string(
      instance->OrderRelevantOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. Property") {
  const std::shared_ptr<
    aas::types::IProperty
  > instance(
    test::common::examples::LoadMinProperty()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Property.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. Property") {
  const std::shared_ptr<
    aas::types::IProperty
  > instance(
    test::common::examples::LoadMaxProperty()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Property.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. MultiLanguageProperty") {
  const std::shared_ptr<
    aas::types::IMultiLanguageProperty
  > instance(
    test::common::examples::LoadMinMultiLanguageProperty()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "MultiLanguageProperty.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. MultiLanguageProperty") {
  const std::shared_ptr<
    aas::types::IMultiLanguageProperty
  > instance(
    test::common::examples::LoadMaxMultiLanguageProperty()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "MultiLanguageProperty.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. Range") {
  const std::shared_ptr<
    aas::types::IRange
  > instance(
    test::common::examples::LoadMinRange()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Range.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. Range") {
  const std::shared_ptr<
    aas::types::IRange
  > instance(
    test::common::examples::LoadMaxRange()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Range.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. ReferenceElement") {
  const std::shared_ptr<
    aas::types::IReferenceElement
  > instance(
    test::common::examples::LoadMinReferenceElement()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "ReferenceElement.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. ReferenceElement") {
  const std::shared_ptr<
    aas::types::IReferenceElement
  > instance(
    test::common::examples::LoadMaxReferenceElement()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "ReferenceElement.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. Blob") {
  const std::shared_ptr<
    aas::types::IBlob
  > instance(
    test::common::examples::LoadMinBlob()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Blob.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. Blob") {
  const std::shared_ptr<
    aas::types::IBlob
  > instance(
    test::common::examples::LoadMaxBlob()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "Blob.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a min. File") {
  const std::shared_ptr<
    aas::types::IFile
  > instance(
    test::common::examples::LoadMinFile()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "File.CategoryOrDefault.min.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

TEST_CASE("Test CategoryOrDefault on a max. File") {
  const std::shared_ptr<
    aas::types::IFile
  > instance(
    test::common::examples::LoadMaxFile()
  );

  const std::filesystem::path log_path(
    DetermineLogDir()
      / "File.CategoryOrDefault.max.log"
  );

  const std::string serialized(
    aas::common::WstringToUtf8(
      instance->CategoryOrDefault()
    )
  );

  test::common::AssertContentEqualsExpectedOrRecord(
    serialized,
    log_path
  );
}

// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_x_or_default.py
// Do NOT edit or append.
