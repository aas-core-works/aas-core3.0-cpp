#include "./common_jsonization.hpp"

#include <aas_core/aas_3_0/common.hpp>

namespace aas = aas_core::aas_3_0;

namespace test {
namespace common {
namespace jsonization {

const std::vector<std::string> kCausesForDeserializationFailure = {
  "TypeViolation",
  "RequiredViolation",
  "EnumViolation",
  "NullViolation",
  "UnexpectedAdditionalProperty"
};


std::optional<std::string> CompareJsons(
  const nlohmann::json& that,
  const nlohmann::json& other
) {
  const nlohmann::json patch = nlohmann::json::diff(that, other);

  if (!patch.is_array()) {
    throw std::logic_error(
      aas::common::Concat(
        "Expected the patch to be an array, but got ",
        patch.type_name()
      )
    );
  }

  if (patch.size() != 0) {
    return nlohmann::to_string(patch);
  }

  return std::nullopt;
}

}  // namespace jsonization
}  // namespace common
}  // namespace test
