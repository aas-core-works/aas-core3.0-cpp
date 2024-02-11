#include "./common_examples.hpp"
#include "./common.hpp"

#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/jsonization.hpp>
#include <aas_core/aas_3_0/stringification.hpp>

#include <filesystem>

namespace aas = aas_core::aas_3_0;

namespace test {
namespace common {
namespace examples {

template<typename T>
struct StaticTypeName;

template<>
struct StaticTypeName<aas::types::IExtension> {
  static const char* name;
};
const char* StaticTypeName<aas::types::IExtension>::name = "IExtension";

template<
  typename T,
  typename C
>
std::optional<
  std::shared_ptr<T>
> FindFirstOfType(
  const std::shared_ptr<C>& container
) {
  std::shared_ptr<T> casted(
    std::dynamic_pointer_cast<T>(
      container
    )
  );
  if (casted != nullptr) {
    return casted;
  }

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(container)
    ) {
    casted = std::dynamic_pointer_cast<T>(something);
    if (casted != nullptr) {
      return casted;
    }
  }

  throw std::logic_error(
    aas::common::Concat(
      "Could not find an instance of ",
      StaticTypeName<T>::name,
      "in the container of type ",
      aas::stringification::to_string(container->model_type())
    )
  );
}

std::shared_ptr<
  aas_core::aas_3_0::types::IExtension
> LoadMinExtension() {
  const std::filesystem::path path(
    test::common::DetermineTestDataDir()
      / "Json"
      / "ContainedInEnvironment"
      / "Expected"
      / "Extension"
      / "minimal.json"
  );

  nlohmann::json json = test::common::MustReadJson(path);

  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > container = aas::jsonization::EnvironmentFrom(json);

  if (!container.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to load an instance of IEnvironment from ",
        path.string()
      )
    );
  }

  std::optional<
    std::shared_ptr<
      aas::types::IExtension
    >
  > first = FindFirstOfType<
    aas::types::IExtension,
    aas::types::IEnvironment
  >(
    *container
  );

  if (!first.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Could not find an instance of IExtension "
        "in the container loaded from ",
        path.string()
      )
    );
  }

  return std::move(*first);
}

std::shared_ptr<
  aas::types::IExtension
> LoadMaxExtension() {
  const std::filesystem::path path(
    test::common::DetermineTestDataDir()
      / "Json"
      / "ContainedInEnvironment"
      / "Expected"
      / "Extension"
      / "maximal.json"
  );

  nlohmann::json json = test::common::MustReadJson(path);

  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > container = aas::jsonization::EnvironmentFrom(json);

  if (!container.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to load an instance of IEnvironment from ",
        path.string()
      )
    );
  }

  std::optional<
    std::shared_ptr<
      aas::types::IExtension
    >
  > first = FindFirstOfType<aas::types::IExtension>(
    *container
  );

  if (!first.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Could not find an instance of IExtension "
        "in the container loaded from ",
        path.string()
      )
    );
  }

  return std::move(*first);
}

}  // namespace examples
}  // namespace common
}  // namespace test
