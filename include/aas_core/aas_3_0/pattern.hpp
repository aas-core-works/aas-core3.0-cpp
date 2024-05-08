#ifndef AAS_CORE_AAS_3_0_PATTERN_GUARD_
#define AAS_CORE_AAS_3_0_PATTERN_GUARD_

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#include "aas_core/aas_3_0/revm.hpp"

#pragma warning(push, 0)
#include <memory>
#include <vector>
#pragma warning(pop)

namespace aas_core {
namespace aas_3_0 {

/**
 * \defgroup pattern Provide patterns to be matched using a multi-threaded virtual machine.
 *
 * The instructions should be supplied to aas_core::aas_3_0::revm::Match. While
 * we could have theoretically included this code in verification, we decided to keep
 * it separate for readability. You are not expected to use this module directly.
 * @{
 */
namespace pattern {

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesIdShortProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesVersionTypeProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesRevisionTypeProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDateTimeUtcProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesMimeTypeProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesRfc8089PathProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesBcp47Program;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXmlSerializableStringProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsAnyUriProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsBase64BinaryProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsBooleanProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDateProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDateTimeProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDecimalProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDoubleProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsDurationProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsFloatProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsGDayProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsGMonthProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsGMonthDayProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsGYearProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsGYearMonthProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsHexBinaryProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsTimeProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsIntegerProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsLongProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsIntProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsShortProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsByteProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsNonNegativeIntegerProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsPositiveIntegerProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsUnsignedLongProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsUnsignedIntProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsUnsignedShortProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsUnsignedByteProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsNonPositiveIntegerProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsNegativeIntegerProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kMatchesXsStringProgram;

extern const std::vector<
  std::unique_ptr<revm::Instruction>
> kIsBcp47ForEnglishProgram;

}  // namespace pattern
/**@}*/

}  // namespace aas_3_0
}  // namespace aas_core

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#endif  // AAS_CORE_AAS_3_0_PATTERN_GUARD_
