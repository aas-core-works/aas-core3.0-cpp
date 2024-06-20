#ifndef AAS_CORE_AAS_3_0_VERIFICATION_GUARD_
#define AAS_CORE_AAS_3_0_VERIFICATION_GUARD_

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#include "aas_core/aas_3_0/common.hpp"
#include "aas_core/aas_3_0/iteration.hpp"
#include "aas_core/aas_3_0/pattern.hpp"
#include "aas_core/aas_3_0/types.hpp"

#pragma warning(push, 0)
#include <set>
#pragma warning(pop)

namespace aas_core {
namespace aas_3_0 {

/**
 * \defgroup verification Verify that instances conform to the meta-model constraints.
 * @{
 */
namespace verification {

// region Forward declarations
class Iterator;
class IVerification;

namespace impl {
class IVerificator;
}  // namespace impl
// endregion Forward declarations

/**
 * Represent a verification error in an instance.
 */
struct Error {
  /**
   * Human-readable description of the error
   */
  std::wstring cause;

  /**
   * Path to the erroneous value
   */
  iteration::Path path;

  explicit Error(std::wstring a_cause);
  Error(std::wstring a_cause, iteration::Path a_path);
};  // struct Error

/**
 * \brief Iterate over the verification errors.
 *
 * The user is expected to take ownership of the errors if they need to be further
 * processed.
 *
 * Unlike STL, this is <em>not</em> a light-weight iterator. We implement
 * a "yielding" iterator by leveraging code generation so that we always keep
 * the model stack as well as the properties verified thus far.
 *
 * This means that copy-construction and equality comparisons are much more heavy-weight
 * than you'd usually expect from an STL iterator. For example, if you want to sort
 * the errors by some criterion, you are most probably faster if you populate a vector,
 * and then sort the vector.
 *
 * Also, given that this iterator is not light-weight, you should in almost all cases
 * avoid the postfix increment (it++) and prefer the prefix one (++it) as the postfix
 * increment would create an iterator copy every time.
 *
 * We follow the C++ standard, and assume that comparison between the two iterators
 * over two different collections results in undefined behavior. See
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2948.html and
 * https://stackoverflow.com/questions/4657513/comparing-iterators-from-different-containers.
 */
class Iterator {
  using iterator_category = std::forward_iterator_tag;
  /// The difference is meaningless, but has to be defined.
  using difference_type = std::ptrdiff_t;
  using value_type = Error;
  using pointer = const Error*;
  using reference = const Error&;

 public:
  explicit Iterator(
    std::unique_ptr<impl::IVerificator> verificator
  ) :
  verificator_(std::move(verificator)) {
      // Intentionally empty.
  }

  Iterator(const Iterator& other);
  Iterator(Iterator&& other);

  Iterator& operator=(const Iterator& other);
  Iterator& operator=(Iterator&& other);

  reference operator*() const;
  pointer operator->() const;

  // Prefix increment
  Iterator& operator++();

  // Postfix increment
  Iterator operator++(int);

  friend bool operator==(const Iterator& a, const Iterator& b);
  friend bool operator!=(const Iterator& a, const Iterator& b);

 private:
  std::unique_ptr<impl::IVerificator> verificator_;
};

bool operator==(const Iterator& a, const Iterator& b);

bool operator!=(const Iterator& a, const Iterator& b);

/// \cond HIDDEN
namespace impl {
class IVerificator {
 public:
  virtual void Start() = 0;
  virtual void Next() = 0;
  virtual bool Done() const = 0;

  virtual const Error& Get() const = 0;
  virtual Error& GetMutable() = 0;
  virtual long Index() const = 0;

  virtual std::unique_ptr<IVerificator> Clone() const = 0;

  virtual ~IVerificator() = default;
};  // class IVerificator
}  // namespace impl
/// \endcond

class IVerification {
 public:
  virtual Iterator begin() const = 0;
  virtual const Iterator& end() const = 0;
  virtual ~IVerification() = default;
};  // class IVerification

/**
 * \brief Verify that the instance conforms to the meta-model constraints.
 *
 * Do not proceed to verify the instances referenced from
 * the given instance.
 *
 * Range-based loops should fit the vast majority of the use cases:
 * \code
 * std::shared_ptr<types::Environment> env = ...;
 * for (const Error& error : NonRecursiveVerification(env)) {
 *   report_somehow(error);
 * }
 * \endcode
 *
 * We use const references to shared pointers here for efficiency. Since
 * we do not make a copy of \p that shared pointer, it is very important that
 * the given shared pointer outlives the verification, lest cause undefined behavior.
 * See these StackOverflow questions:
 * * https://stackoverflow.com/questions/12002480/passing-stdshared-ptr-to-constructors/12002668#12002668
 * * https://stackoverflow.com/questions/3310737/should-we-pass-a-shared-ptr-by-reference-or-by-value
 * * https://stackoverflow.com/questions/37610494/passing-const-shared-ptrt-versus-just-shared-ptrt-as-parameter
 */
class NonRecursiveVerification : public IVerification {
 public:
  NonRecursiveVerification(
    const std::shared_ptr<types::IClass>& instance
  );

  Iterator begin() const override;
  const Iterator& end() const override;

  ~NonRecursiveVerification() override = default;
 private:
  const std::shared_ptr<types::IClass>& instance_;
};  // class NonRecursiveVerification

/**
 * \brief Verify that the instance conforms to the meta-model constraints.
 *
 * Also verify recursively all the instances referenced from
 * the given instance.
 *
 * Range-based loops should fit the vast majority of the use cases:
 * \code
 * std::shared_ptr<types::Environment> env = ...;
 * for (const Error& error : RecursiveVerification(env)) {
 *   report_somehow(error);
 * }
 * \endcode
 *
 * We use const references to shared pointers here for efficiency. Since
 * we do not make a copy of \p that shared pointer, it is very important that
 * the given shared pointer outlives the verification, lest cause undefined behavior.
 * See these StackOverflow questions:
 * * https://stackoverflow.com/questions/12002480/passing-stdshared-ptr-to-constructors/12002668#12002668
 * * https://stackoverflow.com/questions/3310737/should-we-pass-a-shared-ptr-by-reference-or-by-value
 * * https://stackoverflow.com/questions/37610494/passing-const-shared-ptrt-versus-just-shared-ptrt-as-parameter
 */
class RecursiveVerification : public IVerification {
 public:
  RecursiveVerification(
    const std::shared_ptr<types::IClass>& instance
  );

  Iterator begin() const override;
  const Iterator& end() const override;

  ~RecursiveVerification() override = default;
 private:
  const std::shared_ptr<types::IClass>& instance_;
};  // class RecursiveVerification

// region Verification functions

/// \brief Check that \p text is a valid short ID.
bool MatchesIdShort(
  const std::wstring& text
);

/// \brief Check that \p text is a valid version string.
bool MatchesVersionType(
  const std::wstring& text
);

/// \brief Check that \p text is a valid revision string.
bool MatchesRevisionType(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:dateTime`.
///
/// The time zone must be fixed to UTC. We verify only that the `text` matches
/// a pre-defined pattern. We *do not* verify that the day of month is
/// correct nor do we check for leap seconds.
///
/// See: https://www.w3.org/TR/xmlschema-2/#dateTime
bool MatchesXsDateTimeUtc(
  const std::wstring& text
);

/**
 *  \brief Check that \p text is a `xs:dateTime` with time zone set to UTC.
 *
 *  The `text` is assumed to match a pre-defined pattern for `xs:dateTime` with
 *  the time zone set to UTC. In this function, we check for days of month (e.g.,
 *  February 29th).
 *
 *  See: https://www.w3.org/TR/xmlschema-2/#dateTime
 */
bool IsXsDateTimeUtc(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of MIME type.
///
/// The definition has been taken from:
/// https://www.rfc-editor.org/rfc/rfc7231#section-3.1.1.1,
/// https://www.rfc-editor.org/rfc/rfc7230#section-3.2.3 and
/// https://www.rfc-editor.org/rfc/rfc7230#section-3.2.6.
bool MatchesMimeType(
  const std::wstring& text
);

/// \brief Check that \p text is a path conforming to the pattern of RFC 8089.
///
/// The definition has been taken from:
/// https://datatracker.ietf.org/doc/html/rfc8089
bool MatchesRfc8089Path(
  const std::wstring& text
);

/// \brief Check that \p text is a valid BCP 47 language tag.
///
/// See: https://en.wikipedia.org/wiki/IETF_language_tag
bool MatchesBcp47(
  const std::wstring& text
);

/// \brief Check that the \p lang_strings do not have overlapping
/// types::IAbstractLangString::language's
template<
  typename LangStringT,
  typename std::enable_if<
    std::is_base_of<types::IAbstractLangString, LangStringT>::value
  >::type* = nullptr
>
bool LangStringsHaveUniqueLanguages(
  const std::vector<
    std::shared_ptr<LangStringT>
  >& lang_strings
) {
  // NOTE (mristin):
  // See: https://stackoverflow.com/questions/1349734/why-would-anyone-use-set-instead-of-unordered-set
  // For small sets, std::set is often faster than std::unordered_set.
  std::set<std::wstring> language_set;

  for (const std::shared_ptr<LangStringT>& lang_string : lang_strings) {
    const std::wstring& language = lang_string->language();

    if (language_set.find(language) != language_set.end()) {
      return false;
    }
    language_set.insert(language);
  }

  return true;
}

/// \brief Check that types::IQualifier::type's of \p qualifiers are unique.
bool QualifierTypesAreUnique(
  const std::vector<
    std::shared_ptr<types::IQualifier>
  >& qualifiers
);

/// \brief Check that \p text conforms to the pattern of the Constraint AASd-130.
///
/// Ensures that encoding is possible and interoperability between different
/// serializations is possible.
bool MatchesXmlSerializableString(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:anyURI`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#anyURI and
/// https://datatracker.ietf.org/doc/html/rfc3987
bool MatchesXsAnyUri(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:base64Binary`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#base64Binary
bool MatchesXsBase64Binary(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:boolean`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#boolean
bool MatchesXsBoolean(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:date`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#date
bool MatchesXsDate(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:dateTime`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#dateTime
bool MatchesXsDateTime(
  const std::wstring& text
);

/// \brief Check that \p text is a `xs:dateTime` with time zone set to UTC.
///
/// The `text` is assumed to match a pre-defined pattern for `xs:dateTime` with
/// the time zone set to UTC. In this function, we check for days of month (e.g.,
/// February 29th).
///
/// See: https://www.w3.org/TR/xmlschema-2/#dateTime
bool IsXsDateTime(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:decimal`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#decimal
bool MatchesXsDecimal(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:double`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#double
bool MatchesXsDouble(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:duration`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#duration
bool MatchesXsDuration(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:float`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#float
bool MatchesXsFloat(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:gDay`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#gDay
bool MatchesXsGDay(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:gMonth`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#gMonth
bool MatchesXsGMonth(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:gMonthDay`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#gMonthDay
bool MatchesXsGMonthDay(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:gYear`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#gYear
bool MatchesXsGYear(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:gYearMonth`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#gYearMonth
bool MatchesXsGYearMonth(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:hexBinary`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#hexBinary
bool MatchesXsHexBinary(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:time`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#time
bool MatchesXsTime(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:integer`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#integer
bool MatchesXsInteger(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:long`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#long
bool MatchesXsLong(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:int`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#int
bool MatchesXsInt(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:short`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#short
bool MatchesXsShort(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:byte`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#byte
bool MatchesXsByte(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:nonNegativeInteger`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#nonNegativeInteger
bool MatchesXsNonNegativeInteger(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:positiveInteger`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#positiveInteger
bool MatchesXsPositiveInteger(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:unsignedLong`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#unsignedLong
bool MatchesXsUnsignedLong(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:unsignedInt`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#unsignedInt
bool MatchesXsUnsignedInt(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:unsignedShort`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#unsignedShort
bool MatchesXsUnsignedShort(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:unsignedByte`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#unsignedByte
bool MatchesXsUnsignedByte(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:nonPositiveInteger`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#nonPositiveInteger
bool MatchesXsNonPositiveInteger(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:negativeInteger`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#negativeInteger
bool MatchesXsNegativeInteger(
  const std::wstring& text
);

/// \brief Check that \p text conforms to the pattern of an `xs:string`.
///
/// See: https://www.w3.org/TR/xmlschema-2/#string
bool MatchesXsString(
  const std::wstring& text
);

/// \brief Check that the \p value conforms to its \p value_type.
bool ValueConsistentWithXsdType(
  const std::wstring& value,
  types::DataTypeDefXsd value_type
);

/**
 * \brief Check that \p value is a valid `xs:date`.
 *
 * Year 1 BCE is the last leap BCE year.
 * See: https://www.w3.org/TR/xmlschema-2/#dateTime.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:date`
 */
bool IsXsDate(const std::wstring& text);

/**
 * \brief Check that \p value is a valid `xs:double`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:double`
 */
bool IsXsDouble(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:float`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:float`
 */
bool IsXsFloat(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:gMonthDay`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:gMonthDay`
 */
bool IsXsGMonthDay(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:long`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:long`
 */
bool IsXsLong(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:int`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:int`
 */
bool IsXsInt(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:short`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:short`
 */
bool IsXsShort(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:byte`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:byte`
 */
bool IsXsByte(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:unsignedLong`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:unsignedLong`
 */
bool IsXsUnsignedLong(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:unsignedInt`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:unsignedInt`
 */
bool IsXsUnsignedInt(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:unsignedShort`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:unsignedShort`
 */
bool IsXsUnsignedShort(const std::wstring& value);

/**
 * \brief Check that \p value is a valid `xs:unsignedByte`.
 *
 * \param value to be checked
 * \return true if \p value is a valid `xs:unsignedByte`
 */
bool IsXsUnsignedByte(const std::wstring& value);

/// \brief Check that the target of the model reference matches the \p expected_type.
bool IsModelReferenceTo(
  const std::shared_ptr<types::IReference>& reference,
  types::KeyTypes expected_type
);

/// \brief Check that the target of the reference matches a constants::kAasReferables.
bool IsModelReferenceToReferable(
  const std::shared_ptr<types::IReference>& reference
);

/// \brief Check that the types::IReferable::id_short's among the \p referables are
/// unique.
template<
  typename ReferableT,
  typename std::enable_if<
    std::is_base_of<types::IReferable, ReferableT>::value
  >::type* = nullptr
>bool IdShortsAreUnique(
  const std::vector<
    std::shared_ptr<ReferableT>
  >& referables
) {
  std::set<std::wstring> id_short_set;

  for (const std::shared_ptr<types::IReferable> referable : referables) {
    const common::optional<std::wstring>& id_short = referable->id_short();

    if (id_short.has_value()) {
      if (id_short_set.find(*id_short) != id_short_set.end()) {
        return false;
      }

      id_short_set.insert(*id_short);
    }
  }

  return true;
}

/// \brief Check that the types::IReferable::id_short's among all the
/// \p input_variables, \p output_variables
/// and \p inoutput_variables are unique.
bool IdShortsOfVariablesAreUnique(
  const common::optional<
    std::vector<
      std::shared_ptr<types::IOperationVariable>
    >
  >& input_variables,
  const common::optional<
    std::vector<
      std::shared_ptr<types::IOperationVariable>
    >
  >& output_variables,
  const common::optional<
    std::vector<
      std::shared_ptr<types::IOperationVariable>
    >
  >& inoutput_variables
);

/// \brief Check that the extension names are unique.
bool ExtensionNamesAreUnique(
  const std::vector<
    std::shared_ptr<types::IExtension>
  >& extensions
);

/// \brief Check that all semantic IDs are identical, if specified.
bool SubmodelElementsHaveIdenticalSemanticIds(
  const std::vector<
    std::shared_ptr<types::ISubmodelElement>
  >& elements
);

/// \brief Check that the run-time type of the \p element coincides with
/// \p element_type.
bool SubmodelElementIsOfType(
  const std::shared_ptr<types::ISubmodelElement>& element,
  types::AasSubmodelElements element_type
);

/// \brief Check that all the \p elements have the \p value_type.
bool PropertiesOrRangesHaveValueType(
  const std::vector<
    std::shared_ptr<types::ISubmodelElement>
  >& elements,
  types::DataTypeDefXsd value_type
);

/// \brief Check that the two references are equal by comparing their key values.
bool ReferenceKeyValuesEqual(
  const std::shared_ptr<types::IReference>& that,
  const std::shared_ptr<types::IReference>& other
);

/// \brief Check that the types::IDataSpecificationIec61360::data_type is defined
/// appropriately for all data specifications whose content is given as IEC 61360.
bool DataSpecificationIec61360sForPropertyOrValueHaveAppropriateDataType(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the types::IDataSpecificationIec61360::data_type is defined
/// appropriately for all data specifications whose content is given as IEC 61360.
bool DataSpecificationIec61360sForReferenceHaveAppropriateDataType(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the types::IDataSpecificationIec61360::data_type is defined
/// appropriately for all data specifications whose content is given as IEC 61360.
bool DataSpecificationIec61360sForDocumentHaveAppropriateDataType(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the types::IDataSpecificationIec61360::data_type is defined for all
/// data specifications whose content is given as IEC 61360.
bool DataSpecificationIec61360sHaveDataType(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the types::IDataSpecificationIec61360::value is defined
/// for all data specifications whose content is given as IEC 61360.
bool DataSpecificationIec61360sHaveValue(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the types::IDataSpecificationIec61360::definition is defined
/// for all data specifications whose content is given as IEC 61360 at least in English.
bool DataSpecificationIec61360sHaveDefinitionAtLeastInEnglish(
  const std::vector<
    std::shared_ptr<types::IEmbeddedDataSpecification>
  >& embedded_data_specifications
);

/// \brief Check that the \p text corresponds to a BCP47 code for english.
bool IsBcp47ForEnglish(
  const std::wstring& text
);

// endregion Verification functions

// region Verification of constrained primitives

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyNonEmptyXmlSerializableString(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyDateTimeUtc(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyDuration(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyBlobType(
  const std::vector<std::uint8_t>& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyIdentifier(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyValueTypeIec61360(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyNameType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyVersionType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyRevisionType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyLabelType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyMessageTopicType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyBcp47LanguageTag(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyContentType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyPathType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyQualifierType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyValueDataType(
  const std::wstring& that
);

/**
 * \brief Verify that the invariants hold for \p that value.
 *
 * The \p that value should outlive the verification.
 *
 * \param that value to be verified
 * \return Iterable over constraint violations
 */
std::unique_ptr<IVerification> VerifyIdShortType(
  const std::wstring& that
);

// endregion Verification of constrained primitives

}  // namespace verification
/**@}*/

}  // namespace aas_3_0
}  // namespace aas_core

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#endif  // AAS_CORE_AAS_3_0_VERIFICATION_GUARD_
