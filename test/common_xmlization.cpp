#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/xmlization.hpp>

#include <fstream>

namespace aas = aas_core::aas_3_0;

namespace test {
namespace common {
namespace xmlization {

const std::vector<std::string> kCausesForDeserializationFailure = {
  "TypeViolation",
  "RequiredViolation",
  "EnumViolation",
  "NullViolation",
  "UnexpectedAdditionalProperty"
};

std::shared_ptr<
  aas_core::aas_3_0::types::IClass
> MustReadInstance(
  const std::filesystem::path& path
) {
  std::ifstream ifs(path);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > instance = aas::xmlization::From(
    ifs
  );

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read XML from ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }

  if (!instance.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to parse the instance from the XML file ",
        path.string(),
        ": ",
        aas::common::WstringToUtf8(instance.error().path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(instance.error().cause)
      )
    );
  }

  return *instance;
}

}  // namespace xmlization
}  // namespace common
}  // namespace test
