// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_xmlization_dispatch.py
// Do NOT edit or append.

#include "./common_examples.generated.hpp"

#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;

TEST_CASE("Test the round-trip of an expected IHasSemantics") {
  const std::shared_ptr<
    aas::types::IRelationshipElement
  > original_instance(
    test::common::examples::LoadMinRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IHasSemantics
  > abstract = std::dynamic_pointer_cast<
    aas::types::IHasSemantics
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IRelationshipElement " 
    "serialized as IHasSemantics "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IHasExtensions") {
  const std::shared_ptr<
    aas::types::IRelationshipElement
  > original_instance(
    test::common::examples::LoadMinRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IHasExtensions
  > abstract = std::dynamic_pointer_cast<
    aas::types::IHasExtensions
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IRelationshipElement " 
    "serialized as IHasExtensions "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IReferable") {
  const std::shared_ptr<
    aas::types::IRelationshipElement
  > original_instance(
    test::common::examples::LoadMinRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IReferable
  > abstract = std::dynamic_pointer_cast<
    aas::types::IReferable
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IRelationshipElement " 
    "serialized as IReferable "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IIdentifiable") {
  const std::shared_ptr<
    aas::types::IAssetAdministrationShell
  > original_instance(
    test::common::examples::LoadMinAssetAdministrationShell()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IAssetAdministrationShell: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IIdentifiable
  > abstract = std::dynamic_pointer_cast<
    aas::types::IIdentifiable
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IAssetAdministrationShell " 
    "serialized as IIdentifiable "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IHasKind") {
  const std::shared_ptr<
    aas::types::ISubmodel
  > original_instance(
    test::common::examples::LoadMinSubmodel()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of ISubmodel: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IHasKind
  > abstract = std::dynamic_pointer_cast<
    aas::types::IHasKind
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of ISubmodel " 
    "serialized as IHasKind "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IHasDataSpecification") {
  const std::shared_ptr<
    aas::types::IAdministrativeInformation
  > original_instance(
    test::common::examples::LoadMinAdministrativeInformation()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IAdministrativeInformation: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IHasDataSpecification
  > abstract = std::dynamic_pointer_cast<
    aas::types::IHasDataSpecification
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IAdministrativeInformation " 
    "serialized as IHasDataSpecification "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IQualifiable") {
  const std::shared_ptr<
    aas::types::IRelationshipElement
  > original_instance(
    test::common::examples::LoadMinRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IQualifiable
  > abstract = std::dynamic_pointer_cast<
    aas::types::IQualifiable
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IRelationshipElement " 
    "serialized as IQualifiable "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected ISubmodelElement") {
  const std::shared_ptr<
    aas::types::IRelationshipElement
  > original_instance(
    test::common::examples::LoadMinRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::ISubmodelElement
  > abstract = std::dynamic_pointer_cast<
    aas::types::ISubmodelElement
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IRelationshipElement " 
    "serialized as ISubmodelElement "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IRelationshipElement") {
  const std::shared_ptr<
    aas::types::IAnnotatedRelationshipElement
  > original_instance(
    test::common::examples::LoadMinAnnotatedRelationshipElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IAnnotatedRelationshipElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IRelationshipElement
  > abstract = std::dynamic_pointer_cast<
    aas::types::IRelationshipElement
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IAnnotatedRelationshipElement " 
    "serialized as IRelationshipElement "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IDataElement") {
  const std::shared_ptr<
    aas::types::IBlob
  > original_instance(
    test::common::examples::LoadMinBlob()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IBlob: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IDataElement
  > abstract = std::dynamic_pointer_cast<
    aas::types::IDataElement
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IBlob " 
    "serialized as IDataElement "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IEventElement") {
  const std::shared_ptr<
    aas::types::IBasicEventElement
  > original_instance(
    test::common::examples::LoadMinBasicEventElement()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IBasicEventElement: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IEventElement
  > abstract = std::dynamic_pointer_cast<
    aas::types::IEventElement
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IBasicEventElement " 
    "serialized as IEventElement "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IAbstractLangString") {
  const std::shared_ptr<
    aas::types::ILangStringDefinitionTypeIec61360
  > original_instance(
    test::common::examples::LoadMinLangStringDefinitionTypeIec61360()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of ILangStringDefinitionTypeIec61360: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IAbstractLangString
  > abstract = std::dynamic_pointer_cast<
    aas::types::IAbstractLangString
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of ILangStringDefinitionTypeIec61360 " 
    "serialized as IAbstractLangString "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

TEST_CASE("Test the round-trip of an expected IDataSpecificationContent") {
  const std::shared_ptr<
    aas::types::IDataSpecificationIec61360
  > original_instance(
    test::common::examples::LoadMinDataSpecificationIec61360()
  );

  std::stringstream ss;
  aas::xmlization::Serialize(
    *original_instance,
    {},
    ss
  );

  const std::string original_xml = ss.str();

  ss.seekp(0);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ss
  );

  if (!deserialized.has_value()) {
    INFO(
      aas::common::Concat(
        "Failed to make the round-trip Serialize-Deserialize "
        "a minimal instance of IDataSpecificationIec61360: ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    )
    REQUIRE(deserialized.has_value());
  }

  std::shared_ptr<
    aas::types::IDataSpecificationContent
  > abstract = std::dynamic_pointer_cast<
    aas::types::IDataSpecificationContent
  >(deserialized.value());

  std::stringstream another_ss;
  aas::xmlization::Serialize(
    *abstract,
    {},
    another_ss
  );

  INFO(
    "Original XML and the XML at the end of "
    "the chain Serialize-Deserialize-Serialize for "
    "a minimal instance of IDataSpecificationIec61360 " 
    "serialized as IDataSpecificationContent "
    "must coincide"
  )
  REQUIRE(
    original_xml
    == another_ss.str()
  );
}

// This code has been automatically generated by:
// dev_scripts/test_codegen/generate_test_xmlization_dispatch.py
// Do NOT edit or append.
