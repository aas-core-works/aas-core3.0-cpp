/**
 * Provide methods which are used throughout the jsonization tests.
 */
#ifndef AAS_CORE3_TEST_COMMON_JSONIZATION_H_
#define AAS_CORE3_TEST_COMMON_JSONIZATION_H_

#include <nlohmann/json.hpp>

#include <optional>
#include <vector>
#include <string>

namespace test {
namespace common {
namespace jsonization {

extern const std::vector<std::string> kCausesForDeserializationFailure;

/**
 * Read the content of the `path` and parse it as JSON.
 */
nlohmann::json MustReadJson(const std::filesystem::path& path);

/**
 * Compare two JSON values.
 *
 * Return the JSON patch, if there is any difference.
 */
std::optional<std::string> CompareJsons(
  const nlohmann::json& that,
  const nlohmann::json& other
);

}  // namespace jsonization
}  // namespace common
}  // namespace test


#endif //AAS_CORE3_TEST_COMMON_JSONIZATION_H_
