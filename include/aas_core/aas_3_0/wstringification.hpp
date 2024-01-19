#ifndef AAS_CORE_AAS_3_0_WSTRINGIFICATION_GUARD_
#define AAS_CORE_AAS_3_0_WSTRINGIFICATION_GUARD_

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#include "aas_core/aas_3_0/common.hpp"
#include "aas_core/aas_3_0/types.hpp"

#pragma warning(push, 0)
#include <string>
#include <vector>
#pragma warning(pop)

namespace aas_core {
namespace aas_3_0 {

/**
 * \defgroup wstringification De/wstringify to and from wstrings (where applicable).
 * @{
 */
namespace wstringification {

/**
 * Try to parse the \p text as a model type literal.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::ModelType> ModelTypeFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a model type literal.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::ModelType MustModelTypeFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p model_type to text.
 *
 * \param model_type to be converted into text
 * \return text representation of \p model_type
 * \throw std::invalid_argument if \p model_type invalid
 */
std::wstring to_wstring(
  types::ModelType model_type
);

/**
 * Try to parse the \p text as a literal of
 * types::ModellingKind.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::ModellingKind> ModellingKindFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::ModellingKind.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::ModellingKind MustModellingKindFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::ModellingKind to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::ModellingKind literal
);

/**
 * Try to parse the \p text as a literal of
 * types::QualifierKind.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::QualifierKind> QualifierKindFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::QualifierKind.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::QualifierKind MustQualifierKindFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::QualifierKind to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::QualifierKind literal
);

/**
 * Try to parse the \p text as a literal of
 * types::AssetKind.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::AssetKind> AssetKindFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::AssetKind.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::AssetKind MustAssetKindFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::AssetKind to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::AssetKind literal
);

/**
 * Try to parse the \p text as a literal of
 * types::AasSubmodelElements.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::AasSubmodelElements> AasSubmodelElementsFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::AasSubmodelElements.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::AasSubmodelElements MustAasSubmodelElementsFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::AasSubmodelElements to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::AasSubmodelElements literal
);

/**
 * Try to parse the \p text as a literal of
 * types::EntityType.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::EntityType> EntityTypeFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::EntityType.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::EntityType MustEntityTypeFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::EntityType to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::EntityType literal
);

/**
 * Try to parse the \p text as a literal of
 * types::Direction.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::Direction> DirectionFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::Direction.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::Direction MustDirectionFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::Direction to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::Direction literal
);

/**
 * Try to parse the \p text as a literal of
 * types::StateOfEvent.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::StateOfEvent> StateOfEventFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::StateOfEvent.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::StateOfEvent MustStateOfEventFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::StateOfEvent to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::StateOfEvent literal
);

/**
 * Try to parse the \p text as a literal of
 * types::ReferenceTypes.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::ReferenceTypes> ReferenceTypesFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::ReferenceTypes.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::ReferenceTypes MustReferenceTypesFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::ReferenceTypes to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::ReferenceTypes literal
);

/**
 * Try to parse the \p text as a literal of
 * types::KeyTypes.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::KeyTypes> KeyTypesFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::KeyTypes.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::KeyTypes MustKeyTypesFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::KeyTypes to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::KeyTypes literal
);

/**
 * Try to parse the \p text as a literal of
 * types::DataTypeDefXsd.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::DataTypeDefXsd> DataTypeDefXsdFromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::DataTypeDefXsd.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::DataTypeDefXsd MustDataTypeDefXsdFromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::DataTypeDefXsd to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::DataTypeDefXsd literal
);

/**
 * Try to parse the \p text as a literal of
 * types::DataTypeIec61360.
 *
 * \param text to be parsed
 * \return literal, or nothing, if \p text invalid
 */
common::optional<types::DataTypeIec61360> DataTypeIec61360FromWstring(
  const std::wstring& text
);

/**
 * Parse the \p text as a literal of
 * types::DataTypeIec61360.
 *
 * \param text to be parsed
 * \return literal
 * \throw std::invalid_argument if \p text invalid
 */
types::DataTypeIec61360 MustDataTypeIec61360FromWstring(
  const std::wstring& text
);

/**
 * Translate the enumeration literal \p literal
 * of types::DataTypeIec61360 to text.
 *
 * \param literal to be converted into text
 * \return text representation of \p literal
 * \throw std::invalid_argument if \p literal invalid
 */
std::wstring to_wstring(
  types::DataTypeIec61360 literal
);

/**
 * Encode the \p bytes with base64 to a std::wstring.
 *
 * \param bytes to be encoded
 * \return base64-encoding of \p bytes
 */
std::wstring Base64Encode(
  const std::vector<std::uint8_t>& bytes
);

}  // namespace wstringification
/**@}*/

}  // namespace aas_3_0
}  // namespace aas_core

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#endif  // AAS_CORE_AAS_3_0_WSTRINGIFICATION_GUARD_
