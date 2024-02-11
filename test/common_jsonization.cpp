#include "./common_jsonization.hpp"

#include <aas_core/aas_3_0/common.hpp>

#include <fstream>

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

nlohmann::json MustReadJson(const std::filesystem::path& path) {
  std::ifstream ifs(path);

  nlohmann::json result;
  try {
    result = nlohmann::json::parse(ifs);
  } catch (nlohmann::json::parse_error& exception) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read JSON from ",
        path.string(),
        ": ",
        exception.what()
      )
    );
  }

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read JSON from ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }

  if (ifs.fail()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read JSON from ",
        path.string(),
        "; the fail bit of the file stream is set"
      )
    );
  }

  if (!ifs.eof()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read JSON from ",
        path.string(),
        "; the EOF bit is not set meaning that we did not parse all the content"
      )
    );
  }

  return result;
}

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
