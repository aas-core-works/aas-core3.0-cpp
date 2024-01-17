/**
 * Test examples presented in the documentation.
 */

#include "aas_core/aas_3_0/common.hpp"
#include "aas_core/aas_3_0/enhancing.hpp"
#include "aas_core/aas_3_0/iteration.hpp"
#include "aas_core/aas_3_0/jsonization.hpp"
#include "aas_core/aas_3_0/stringification.hpp"
#include "aas_core/aas_3_0/types.hpp"
#include "aas_core/aas_3_0/visitation.hpp"
#include "aas_core/aas_3_0/verification.hpp"
#include "aas_core/aas_3_0/xmlization.hpp"

#include <queue>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sstream>

namespace aas = aas_core::aas_3_0;

TEST_CASE("Test create/get/set") {
  // We create the first element with the constructor.
  std::shared_ptr<aas::types::IProperty> some_element(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );

  // We use setters to set the optional attributes.
  some_element->set_id_short(L"someElement");
  some_element->set_value(L"1984");

  // We create the second element.
  std::shared_ptr<aas::types::IBlob> another_element(
    new aas::types::Blob(
      L"application/octet-stream"
    )
  );

  another_element->set_id_short(L"anotherElement");
  another_element->set_value(
    std::vector<std::uint8_t>{
      0xDE, 0xAD, 0xBE, 0xEF
    }
  );

  // We also directly access the element attributes.
  another_element->mutable_value()->at(3) = 0xAD;

  // We nest the elements in a submodel.
  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );

  // We have to cast since C++ containers are invariant.
  // Dynamic casts are necessary due to virtual inheritance.
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      std::dynamic_pointer_cast<aas::types::ISubmodelElement>(
        some_element
      ),
      std::dynamic_pointer_cast<aas::types::ISubmodelElement>(
        another_element
      )
    }
  );

  // We now create the environment to wrap it all up.
  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );

  environment->set_submodels(
    std::vector<std::shared_ptr<aas::types::ISubmodel> >{
      submodel
    }
  );

  // We can directly access the properties from the children
  // as well.
  std::dynamic_pointer_cast<aas::types::IBlob>(
    environment
      ->submodels()
      ->at(0)
      ->submodel_elements()
      ->at(1)
  )->mutable_value()->at(3) = 0xEF;

  // Now we can do something with the environment...
}

TEST_CASE("Test descent") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodelElement> another_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  another_property->set_id_short(L"anotherProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > yet_another_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  yet_another_property->set_id_short(L"yetAnotherProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property,
      another_property,
      yet_another_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  std::ostringstream ss;

  // We descend recursively over all the instances.
  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(environment)
    ) {
    ss
      << aas::stringification::to_string(something->model_type())
      << std::endl;
  }

  REQUIRE(
    ss.str()
      == R"V0G0N(Submodel
Property
Property
Property
)V0G0N"
  );

  // We descend non-recursively for only one level.
  ss.str("");
  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::DescentOnce(environment)
    ) {
    ss
      << aas::stringification::to_string(something->model_type())
      << std::endl;
  }

  REQUIRE(
    ss.str()
      == R"V0G0N(Submodel
)V0G0N"
  );
}

TEST_CASE("Test filtering with descent") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> bool_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  bool_property->set_id_short(L"boolProperty");

  std::shared_ptr<aas::types::ISubmodelElement> int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  int_property->set_id_short(L"intProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > another_int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  another_int_property->set_id_short(L"anotherIntProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      bool_property,
      int_property,
      another_int_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  std::ostringstream ss;

  // We descend recursively over all the instances
  // and filter for the int properties.

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(environment)
    ) {
    // We use model_type getter instead of RTTI since
    // this is faster.
    if (aas::types::IsProperty(*something)) {
      // We have to use dynamic pointer cast due to virtual
      // inheritance.
      auto property = std::dynamic_pointer_cast<
        aas::types::IProperty
      >(something);

      if (
        property->value_type()
          == aas::types::DataTypeDefXsd::kInt
        ) {
        ss << aas::common::WstringToUtf8(
          *(property->id_short())
        ) << std::endl;
      }
    }
  }

  REQUIRE(
    ss.str()
      == R"V0G0N(intProperty
anotherIntProperty
)V0G0N"
  );
}

class MyVisitor : public aas::visitation::PassThroughVisitor {
 public:
  MyVisitor(
    std::ostream& ostream
  ) :
    ostream_(&ostream) {
    // Intentionally empty.
  }

  void VisitProperty(
    const std::shared_ptr<aas::types::IProperty>& that
  ) override {
    if (
      that->value_type()
        == aas::types::DataTypeDefXsd::kInt
      ) {
      (*ostream_) << aas::common::WstringToUtf8(
        *(that->id_short())
      ) << std::endl;
    }
  }

  ~MyVisitor() = default;

 private:
  std::ostream* ostream_;
};

TEST_CASE("Test filtering with pass-thru visitor") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> bool_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  bool_property->set_id_short(L"boolProperty");

  std::shared_ptr<aas::types::ISubmodelElement> int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  int_property->set_id_short(L"intProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > another_int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  another_int_property->set_id_short(L"anotherIntProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      bool_property,
      int_property,
      another_int_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We instantiate the visitor.
  std::stringstream ss;
  MyVisitor my_visitor(ss);

  // We visit the environment.
  my_visitor.Visit(environment);

  REQUIRE(
    ss.str()
      == R"V0G0N(intProperty
anotherIntProperty
)V0G0N"
  );
}

TEST_CASE("Test JSON serialization") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{some_property}
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We serialize the environment to JSON.
  nlohmann::json json = aas::jsonization::Serialize(
    *environment
  );

  // We can then serialize JSON to text.
  std::stringstream ss;

  ss << json << std::endl;

  REQUIRE(
    ss.str()
      == "{\"submodels\":[{"
         "\"id\":\"some-unique-global-identifier\","
         "\"modelType\":\"Submodel\","
         "\"submodelElements\":[{"
         "\"idShort\":\"someProperty\","
         "\"modelType\":\"Property\","
         "\"valueType\":\"xs:int\""
         "}]}]}\n"
  );
}

TEST_CASE("Test JSON de-serialization") {
  // We specify the JSON as text.
  std::string text(
    "{\"submodels\":[{"
    "\"id\":\"some-unique-global-identifier\","
    "\"modelType\":\"Submodel\","
    "\"submodelElements\":[{"
    "\"idShort\":\"someProperty\","
    "\"modelType\":\"Property\","
    "\"valueType\":\"xs:int\""
    "}]}]}\n"
  );

  // We parse text as JSON.
  nlohmann::json json = nlohmann::json::parse(
    text
  );

  // We parse the JSON as environment.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > environment = aas::jsonization::EnvironmentFrom(json);

  if (!environment.has_value()) {
    std::string message(
      aas::common::Concat(
        "Expected no error, but got: ",
        aas::common::WstringToUtf8(environment.error().path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(environment.error().cause)
      )
    );
    INFO(message)
    REQUIRE(environment.has_value());
  }

  // We serialize back to test the round-trip.
  nlohmann::json another_json = aas::jsonization::Serialize(
    *(*environment)
  );

  std::stringstream ss;
  ss << another_json << std::endl;

  REQUIRE(ss.str() == text);
}

TEST_CASE("Test JSON de-serialization error") {
  // We specify the text which is valid JSON,
  // but not a valid representation of a model.
  std::string text(
    "{\"submodels\": 42}"
  );

  // We parse text as JSON.
  nlohmann::json json = nlohmann::json::parse(
    text
  );

  // We try to parse the JSON as environment.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > environment = aas::jsonization::EnvironmentFrom(json);

  REQUIRE(!environment.has_value());

  REQUIRE(
    environment.error().path.ToWstring()
      == L".submodels"
  );

  REQUIRE(
    environment.error().cause
      == L"Expected an array, but got: number"
  );
}

TEST_CASE("Test XML serialization") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{some_property}
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We serialize the environment to a stream
  // as XML.
  std::ostringstream ss;

  aas::xmlization::Serialize(
    *environment,
    aas::xmlization::WritingOptions(),
    ss
  );

  REQUIRE(
    ss.str()
      == "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
         "<environment xmlns=\"https://admin-shell.io/aas/3/0\">"
         "<submodels>"
         "<submodel>"
         "<id>some-unique-global-identifier</id>"
         "<submodelElements>"
         "<property>"
         "<idShort>someProperty</idShort>"
         "<valueType>xs:int</valueType>"
         "</property>"
         "</submodelElements>"
         "</submodel>"
         "</submodels>"
         "</environment>"
  );
}

TEST_CASE("Test XML de-serialization") {
  std::string text(
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<environment xmlns=\"https://admin-shell.io/aas/3/0\">"
    "<submodels>"
    "<submodel>"
    "<id>some-unique-global-identifier</id>"
    "<submodelElements>"
    "<property>"
    "<idShort>someProperty</idShort>"
    "<valueType>xs:int</valueType>"
    "</property>"
    "</submodelElements>"
    "</submodel>"
    "</submodels>"
    "</environment>"
  );

  // We prepare the input stream.
  std::stringstream istream;
  istream << text;
  istream.seekp(0);

  // We de-serialize the environment from
  // an XML text.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::xmlization::DeserializationError
  > environment = aas::xmlization::EnvironmentFrom(
    istream
  );

  if (!environment.has_value()) {
    std::string message(
      aas::common::Concat(
        "Expected no error, but got: ",
        aas::common::WstringToUtf8(environment.error().path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(environment.error().cause)
      )
    );
    INFO(message)
    REQUIRE(environment.has_value());
  }

  // We perform the round-trip.
  std::ostringstream ostream;
  aas::xmlization::Serialize(
    *(*environment),
    aas::xmlization::WritingOptions(),
    ostream
  );

  REQUIRE(ostream.str() == text);
}

TEST_CASE("Test XML de-serialization of invalid XML") {
  std::string text(
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<environment xmlns=\"https://admin-shell.io/aas/3/0\">"
    "<submodels>"
    "42"
    "</submodels>"
    "</environment>"
  );

  // We prepare the input stream.
  std::stringstream istream;
  istream << text;
  istream.seekp(0);

  // We de-serialize the environment from
  // an XML text.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::xmlization::DeserializationError
  > environment = aas::xmlization::EnvironmentFrom(
    istream
  );

  REQUIRE(!environment.has_value());

  REQUIRE(
    environment.error().path.ToWstring()
      == L"environment/submodels/*[0]"
  );

  REQUIRE(
    environment.error().cause
      == (
        L"Expected a start element opening "
        L"an instance of ISubmodel, but got "
        L"an XML text"
      )
  );
}

TEST_CASE("Test verification") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );

  // We intentionally introduce a mistake.
  some_property->set_id_short(L"!@#$% invalid ID short (*&^_+");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We verify the environment and report the errors.
  std::stringstream ss;

  for (
    const aas::verification::Error& error
    : aas::verification::RecursiveVerification(environment)
    ) {
    ss
      << aas::common::WstringToUtf8(error.path.ToWstring())
      << ": "
      << aas::common::WstringToUtf8(error.cause);
  }

  REQUIRE(
    ss.str()
      == ".submodels[0].submodel_elements[0]: ID-short of Referables shall "
         "only feature letters, digits, underscore (``_``); starting "
         "mandatory with a letter. *I.e.* ``[a-zA-Z][a-zA-Z0-9_]*``."
  );
}

struct ParentEnhancement {
  aas::common::optional<
    std::shared_ptr<aas::types::IClass>
  > parent;
};

std::shared_ptr<ParentEnhancement> ProduceParentEnhancement(
  const std::shared_ptr<aas::types::IClass>&
) {
  return std::make_shared<ParentEnhancement>();
}

TEST_CASE("Test enhancement with a parent") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We enhance the environment recursively.
  environment = aas::enhancing::Wrap<
    ParentEnhancement
  >(
    environment,
    ProduceParentEnhancement
  );

  // We set the parent in the enhancement.
  std::queue<
    std::shared_ptr<aas::types::IClass>
  > queue;

  queue.push(environment);

  while (!queue.empty()) {
    std::shared_ptr<aas::types::IClass> instance(
      std::move(queue.front())
    );
    queue.pop();

    for (
      const std::shared_ptr<
        aas::types::IClass
      >& child
      : aas::iteration::DescentOnce(instance)
      ) {
      std::shared_ptr<ParentEnhancement> enhancement(
        aas::enhancing::MustUnwrap<ParentEnhancement>(
          child
        )
      );

      enhancement->parent = instance;
    }
  }

  // We retrieve the parent of the first submodel.
  std::shared_ptr<ParentEnhancement> enhancement(
    aas::enhancing::MustUnwrap<ParentEnhancement>(
      environment->submodels()->at(0)
    )
  );

  REQUIRE(
    (*(enhancement->parent)).get()
      == environment.get()
  );
}

struct IdEnhancement {
  long id;

  IdEnhancement(
    long an_id
  ) :
    id(an_id) {
    // Intentionally empty.
  }
};

TEST_CASE("Test selective enhancement of IReferable's") {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We set up the enhancing factory.
  long next_id = 0;

  std::function<
    std::shared_ptr<IdEnhancement>(
      const std::shared_ptr<aas::types::IClass>&
    )
  > enhancement_factory = [&](
    const std::shared_ptr<aas::types::IClass>& that
  ) {
    if (aas::types::IsReferable(*that)) {
      auto result = std::make_shared<IdEnhancement>(
        next_id
      );
      ++next_id;
      return result;
    }

    return std::shared_ptr<
      IdEnhancement
    >(nullptr);
  };

  // We enhance the environment recursively.
  environment = aas::enhancing::Wrap<
    IdEnhancement
  >(
    environment,
    enhancement_factory
  );

  // The environment is not a referable,
  // so no wrapping expected.
  REQUIRE(
    aas::enhancing::Unwrap<IdEnhancement>(
      environment
    )
      == nullptr
  );

  // We output the IDs.
  std::stringstream ss;

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(environment)
    ) {
    if (aas::types::IsReferable(*something)) {
      ss
        << aas::stringification::to_string(
          something->model_type()
        )
        << " with our ID "
        << aas::enhancing::MustUnwrap<IdEnhancement>(
          something
        )->id
        << std::endl;
    } else {
      REQUIRE(
        aas::enhancing::Unwrap<IdEnhancement>(
          something
        )
          == nullptr
      );
    }

  }

  REQUIRE(
    ss.str() == R"V0G0N(Submodel with our ID 1
Property with our ID 0
)V0G0N"
  );
}
