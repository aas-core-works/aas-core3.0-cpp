/**
* Load minimal and maximal examples from the disk.
*/

#ifndef AAS_CORE3_TEST_COMMON_EXAMPLES_HPP_
#define AAS_CORE3_TEST_COMMON_EXAMPLES_HPP_

#include <aas_core/aas_3_0/types.hpp>

namespace test {
namespace common {
namespace examples {

/**
 * Load a minimal example of IExtension by deserializing it
 * from a JSON file.
 */
std::shared_ptr<
  aas_core::aas_3_0::types::IExtension
> LoadMinExtension();

/**
 * Load a maximal example of IExtension by deserializing it
 * from a JSON file.
 */
std::shared_ptr<
  aas_core::aas_3_0::types::IExtension
> LoadMaxExtension();

}  // namespace examples
}  // namespace common
}  // namespace test

#endif // AAS_CORE3_TEST_COMMON_EXAMPLES_HPP_