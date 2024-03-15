#ifndef TEST_COMMON_
#define TEST_COMMON_

#include <aas_core/aas_3_0/types.hpp>

#include <nlohmann/json.hpp>

#include <deque>
#include <filesystem>

/**
* Provide common functionalities shared across the tests.
*/
namespace test {
namespace common {

/**
 * Set to true if we run tests in the record mode according to the environment
 * variable `AAS_CORE_3_0_CPP_RECORD_MODE`.
 *
 * \remark It is tedious to record manually all the expected error messages. Therefore
 * we include this variable to steer the automatic recording. We intentionally
 * intertwine the recording code with the test code to keep them close to each other
 * so that they are easier to maintain.
 */
bool DetermineRecordMode();

/**
 * Point to the directory with the test data according to the environment
 * variable `AAS_CORE_3_0_CPP_TEST_DATA_DIR`.
 */
std::filesystem::path DetermineTestDataDir();

/**
 * Represent `that` instance as a human-readable line of an iteration trace.
 */
std::wstring TraceMark(
  const aas_core::aas_3_0::types::IClass& that
);

/**
 * Find files beneath the `root` recursively which have the `suffix`.
 *
 * If the `root` does not exist, return an empty deque.
 *
 * If the `root` is not a directory, throw a runtime error.
 *
 * The files are sorted.
 */
std::deque<std::filesystem::path> FindFilesBySuffixRecursively(
  const std::filesystem::path& root,
  const std::string& suffix
);

/**
 * Check that the content coincides with the file or re-record if in record mode.
 */
void AssertContentEqualsExpectedOrRecord(
  const std::string& content,
  const std::filesystem::path& path
);

template<typename T>
std::string JoinStrings(const T& container, const std::string& delimiter) {
  static_assert(std::is_same<typename T::value_type, std::string>::value);

  size_t size = 0;
  for (const std::string& part : container
    ) {
    size += part.size();
  }
  size += delimiter.size() * (container.size() - 1);

  std::string result;
  result.reserve(size);

  auto it = container.begin();
  for (size_t i = 0; i < container.size() - 1; ++i) {
    result.append(*it);
    result.append(delimiter);
    ++it;
  }

  result.append(*it);

  return result;
}

template<typename T>
std::wstring JoinWstrings(const T& container, const std::wstring& delimiter) {
  if (container.size() == 0) {
    return L"";
  }

  static_assert(std::is_same<typename T::value_type, std::wstring>::value);

  size_t size = 0;
  for (const std::wstring& part : container
    ) {
    size += part.size();
  }
  size += delimiter.size() * (container.size() - 1);

  std::wstring result;
  result.reserve(size);

  auto it = container.begin();
  for (size_t i = 0; i < container.size() - 1; ++i) {
    result.append(*it);
    result.append(delimiter);
    ++it;
  }

  result.append(*it);

  return result;
}

std::string MustReadString(
  const std::filesystem::path& path
);

}  // namespace common
}  // namespace test

#endif