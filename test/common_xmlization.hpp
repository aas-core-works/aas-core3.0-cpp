/**
 * Provide methods which are used throughout the jsonization tests.
 */
#ifndef AAS_CORE3_TEST_COMMON_XMLIZATION_H_
#define AAS_CORE3_TEST_COMMON_XMLIZATION_H_

#include <aas_core/aas_3_0/types.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace test {
namespace common {
namespace xmlization {

extern const std::vector<std::string> kCausesForDeserializationFailure;

/**
 * Read the content of the `path` and parse it as XML representation of an instance.
 */
std::shared_ptr<
  aas_core::aas_3_0::types::IClass
> MustReadInstance(
  const std::filesystem::path& path
);

/**
 * Canonicalize the `xml` by stripping away any XML text between the stop and start
 * nodes.
 */
std::string CanonicalizeXml(
  const std::string& xml
);

}  // namespace xmlization
}  // namespace common
}  // namespace test


#endif // AAS_CORE3_TEST_COMMON_XMLIZATION_H_
