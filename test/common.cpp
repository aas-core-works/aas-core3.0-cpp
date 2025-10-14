#include "./common.hpp"

#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/wstringification.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <queue>
#include <set>
#include <stdexcept>
#include <type_traits>

namespace aas = aas_core::aas_3_0;

namespace fs = std::filesystem;

namespace test {
namespace common {

static std::optional<std::string> GetEnv(const char *variable_name) {
  // NOTE (empwilli): MSVC complains about getenv being unsafe.
  // This should not be an issue as we don't run our tests in parallel, though,
  // so we use getenv_s in Windows. However, this function is a Microsoft
  // extension.
#ifdef _WIN32
  char buffer[256];
  size_t len;
  errno_t error = getenv_s(
    &len, buffer, sizeof(buffer), variable_name
  );
  if (error != 0) {
    throw std::runtime_error(
      aas::common::Concat(
        "The getenv_s returned an error code ",
        std::to_string(error),
        " when asking for the variable ",
        variable_name
      )
    );
  }

  if (len == 0) {
    return std::nullopt;
  }

  if (buffer[len - 1] != 0) {
    throw std::logic_error(
      aas::common::Concat(
        "Expected the last byte of buffer for getenv_s on ",
        variable_name,
        " to be zero, but got ",
        std::to_string(static_cast<std::uint8_t>(buffer[len - 1]))
      )
    );
  }

  return std::optional { std::string(buffer, len - 1) };
#else
  auto value = getenv(variable_name);

  if (value == nullptr) {
    return std::nullopt;
  }

  return std::optional { std::string(value) };
#endif
}

bool DetermineRecordMode() {
  const char* variable_name = "AAS_CORE_3_0_CPP_RECORD_MODE";

  auto result = GetEnv(variable_name);

  if (!result.has_value())
  {
    return false;
  }

  static const std::set<std::string> hot_value_set{
    "on", "ON", "On",
    "true", "TRUE", "True",
    "1",
    "yes", "Yes", "YES"
  };
  return hot_value_set.find(result.value()) != hot_value_set.end();
}

std::filesystem::path DetermineTestDataDir() {
  const char* variable_name = "AAS_CORE_3_0_CPP_TEST_DATA_DIR";

  auto result = GetEnv(variable_name);

  if (!result.has_value())
  {
    throw std::runtime_error(
      aas::common::Concat(
        "The environment variable ",
        variable_name,
        " has not been set."
      )
    );
  }

  return fs::path(result.value());
}

/**
 * Write `text` to the given `path` encoded as UTF-8.
 */
void MustWriteWstringAsUtf8(
  const std::filesystem::path& path,
  const std::wstring& text
) {
  std::ofstream ofs(path, std::ios::binary);

  const std::string encoded = aas::common::WstringToUtf8(text);
  ofs.write(encoded.data(), encoded.size());

  if (ofs.fail()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to write to ",
        path.string(),
        "; the fail bit of the file stream is set"
      )
    );
  }

  if (ofs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to write to ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }
}

/**
 * Read the content of the file `path` and decode it from UTF-8.
 */
std::wstring MustReadWstringAsUtf8(
  const std::filesystem::path& path
) {
  std::ifstream ifs(path, std::ios::binary);
  ifs.seekg(0, std::ios::end);
  std::streamoff size = ifs.tellg();
  if (size < 0) {
    throw std::runtime_error(
      aas::common::Concat(
        "Unexpected negative size of ",
        path.string(),
        ": ",
        std::to_string(size)
      )
    );
  }

  std::string buffer(static_cast<size_t>(size), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], size);

  if (ifs.fail()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read from ",
        path.string(),
        "; the fail bit of the file stream is set"
      )
    );
  }

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read from ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }

  try {
    return aas::common::Utf8ToWstring(buffer);
  } catch (const std::exception& exception) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to de-code ",
        path.string(),
        " as UTF-8: ",
        exception.what()
      )
    );
  }
}

/**
 * Write `text` to the given `path` as binary.
 */
void MustWriteString(
  const std::filesystem::path& path,
  const std::string& text
) {
  std::ofstream ofs(path, std::ios::binary);

  ofs.write(text.data(), text.size());

  if (ofs.fail()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to write to ",
        path.string(),
        "; the fail bit of the file stream is set"
      )
    );
  }

  if (ofs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to write to ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }
}

/**
 * Read the content of the file `path` as binary.
 */
std::string MustReadString(
  const std::filesystem::path& path
) {
  std::ifstream ifs(path, std::ios::binary);
  ifs.seekg(0, std::ios::end);
  std::streamoff size = ifs.tellg();
  if (size < 0) {
    throw std::runtime_error(
      aas::common::Concat(
        "Unexpected negative size of ",
        path.string(),
        ": ",
        std::to_string(size)
      )
    );
  }

  std::string buffer(static_cast<size_t>(size), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], size);

  if (ifs.fail()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read from ",
        path.string(),
        "; the fail bit of the file stream is set"
      )
    );
  }

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read from ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }

  return buffer;
}

std::wstring TraceMark(
  const aas_core::aas_3_0::types::IClass& that
) {
  const std::wstring model_type = aas::wstringification::to_wstring(
    that.model_type()
  );

  if (aas_core::aas_3_0::types::IsIdentifiable(that)) {
    const auto& identifiable(
      dynamic_cast<const aas::types::IIdentifiable&>(that)
    );

    return aas::common::Concat(
      model_type,
      L" with ID ",
      identifiable.id()
    );
  }

  if (aas_core::aas_3_0::types::IsReferable(that)) {
    const auto& referable(
      dynamic_cast<const aas::types::IReferable&>(that)
    );

    if (referable.id_short().has_value()) {
      return aas::common::Concat(
        model_type,
        L" with ID-short ",
        *(referable.id_short())
      );
    }
    return aas::common::Concat(
      model_type,
      L" with unspecified ID-short"
    );
  }

  return model_type;
}

bool StringEndsWith(
  const std::string& text,
  const std::string& suffix
) {
  if (text.size() < suffix.size()) {
    return false;
  }

  return (
    text.substr(text.size() - suffix.size(), suffix.size())
      == suffix
  );
}

std::deque<std::filesystem::path> FindFilesBySuffixRecursively(
  const std::filesystem::path& root,
  const std::string& suffix
) {
  if (!fs::exists(root)) {
    return std::deque<std::filesystem::path>();
  }

  if (!fs::is_directory(root)) {
    throw std::runtime_error(
      aas::common::Concat(
        "Expected to search for files with suffix recursively in ",
        root.string(),
        ", but it is not a directory"
      )
    );
  }

  std::deque<fs::path> result;

  for (
    const fs::directory_entry& entry
    : fs::recursive_directory_iterator(root)
    ) {
    if (
      StringEndsWith(
        entry.path().filename().string(),
        suffix
      )
      ) {
      result.push_back(entry.path());
    }
  }

  std::sort(result.begin(), result.end());

  return result;
}

std::deque<std::filesystem::path> ListSubdirectories(
  const std::filesystem::path& root
) {
  if (!fs::exists(root)) {
    throw std::runtime_error(
      aas::common::Concat(
        "The root directory which you wanted to list for subdirectories "
        "does not exist: ",
        root.string()
      )
    );
  }

  if (!fs::is_directory(root)) {
    throw std::runtime_error(
      aas::common::Concat(
        "The path that you specified as a root directory which you wanted to "
        "list for subdirectories is not a directory: ",
        root.string()
      )
    );
  }

  std::deque<fs::path> result;

  for (
    const fs::directory_entry& entry
    : fs::directory_iterator(root)
    ) {
    if (fs::is_directory(entry.path())) {
      result.push_back(entry.path());
    }
  }

  std::sort(result.begin(), result.end());
  return result;
}

void AssertContentEqualsExpectedOrRecord(
  const std::string& content,
  const std::filesystem::path& path
) {
  const bool record_mode = DetermineRecordMode();
  if (record_mode) {
    fs::create_directories(path.parent_path());

    MustWriteString(path, content);
  } else {
    INFO(
      aas::common::Concat(
        "Expected to compare the content against "
        "the recorded file: ",
        path.string(),
        ", but the file does not exist. If you want to re-record, "
        "please set the environment variable AAS_CORE_3_0_CPP_RECORD_MODE to 'ON'"
      )
    )
    REQUIRE(std::filesystem::exists(path));

    const std::string expected = MustReadString(path);
    INFO(
      aas::common::Concat(
        "Got unexpected content, which should have been equal "
        "to the content of the file: ",
        path.string()
      )
    )
    REQUIRE(content == expected);
  }
}

}  // namespace common
}  // namespace test
