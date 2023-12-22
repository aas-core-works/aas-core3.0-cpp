#ifndef AAS_CORE_AAS_3_0_ITERATION_GUARD_
#define AAS_CORE_AAS_3_0_ITERATION_GUARD_

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#include "aas_core/aas_3_0/types.hpp"

#pragma warning(push, 0)
#include <deque>
#include <iterator>
#include <memory>
#include <string>
#pragma warning(pop)

namespace aas_core {
namespace aas_3_0 {

/**
 * \defgroup iteration Define functions and structures to iterate over instances.
 * @{ 
*/
namespace iteration {

// region Pathing

/**
 * Define the properties over all the classes to compactly represent the paths.
 */
enum class Property : std::uint32_t {
  kAdministration = 0,
  kAnnotations = 1,
  kAssetAdministrationShells = 2,
  kAssetInformation = 3,
  kAssetKind = 4,
  kAssetType = 5,
  kCategory = 6,
  kConceptDescriptions = 7,
  kContentType = 8,
  kCreator = 9,
  kDataSpecification = 10,
  kDataSpecificationContent = 11,
  kDataType = 12,
  kDefaultThumbnail = 13,
  kDefinition = 14,
  kDerivedFrom = 15,
  kDescription = 16,
  kDirection = 17,
  kDisplayName = 18,
  kEmbeddedDataSpecifications = 19,
  kEntityType = 20,
  kExtensions = 21,
  kExternalSubjectId = 22,
  kFirst = 23,
  kGlobalAssetId = 24,
  kId = 25,
  kIdShort = 26,
  kInoutputVariables = 27,
  kInputVariables = 28,
  kIsCaseOf = 29,
  kKeys = 30,
  kKind = 31,
  kLanguage = 32,
  kLastUpdate = 33,
  kLevelType = 34,
  kMax = 35,
  kMaxInterval = 36,
  kMessageBroker = 37,
  kMessageTopic = 38,
  kMin = 39,
  kMinInterval = 40,
  kName = 41,
  kNom = 42,
  kObservableReference = 43,
  kObservableSemanticId = 44,
  kObserved = 45,
  kOrderRelevant = 46,
  kOutputVariables = 47,
  kPath = 48,
  kPayload = 49,
  kPreferredName = 50,
  kQualifiers = 51,
  kReferredSemanticId = 52,
  kRefersTo = 53,
  kRevision = 54,
  kSecond = 55,
  kSemanticId = 56,
  kSemanticIdListElement = 57,
  kShortName = 58,
  kSource = 59,
  kSourceOfDefinition = 60,
  kSourceSemanticId = 61,
  kSpecificAssetIds = 62,
  kState = 63,
  kStatements = 64,
  kSubjectId = 65,
  kSubmodelElements = 66,
  kSubmodels = 67,
  kSupplementalSemanticIds = 68,
  kSymbol = 69,
  kTemplateId = 70,
  kText = 71,
  kTimeStamp = 72,
  kTopic = 73,
  kTyp = 74,
  kType = 75,
  kTypeValueListElement = 76,
  kUnit = 77,
  kUnitId = 78,
  kValue = 79,
  kValueFormat = 80,
  kValueId = 81,
  kValueList = 82,
  kValueReferencePairs = 83,
  kValueType = 84,
  kValueTypeListElement = 85,
  kVersion = 86
};

std::wstring PropertyToWstring(
  Property property
);

/**
 * Represent a segment of a path to some value.
 */
class ISegment {
 public:
  virtual std::wstring ToWstring() const = 0;
  virtual std::unique_ptr<ISegment> Clone() const = 0;
  virtual ~ISegment() = default;
};  // class ISegment

/**
 * Represent a property access on a path.
 */
struct PropertySegment : public ISegment {
  /**
   * Enumeration of the property
   */
  Property property;

  PropertySegment(
    Property a_property
  );

  std::wstring ToWstring() const override;
  std::unique_ptr<ISegment> Clone() const override;

  ~PropertySegment() override = default;
};  // struct PropertySegment

/**
 * Represent an index access on a path.
 */
struct IndexSegment : public ISegment {
  /**
   * Index of the item
   */
  size_t index;

  explicit IndexSegment(
    size_t an_index
  );

  std::wstring ToWstring() const override;
  std::unique_ptr<ISegment> Clone() const override;

  ~IndexSegment() override = default;
};  // struct IndexSegment

/**
 * \brief Represent a path to some value.
 *
 * This is a path akin to C++ expressions. It is not to be confused with different
 * paths used in the specification. This path class is meant to help with reporting. 
 * For example, we can use this path to let the user know when there is
 * a verification error in a model which can concern instances, but also properties
 * and items in the lists.
 */
struct Path {
  // NOTE (mristin):
  // We did not implement the reflection at the moment since we did not have a use
  // case for it. If you need reflection, please contact the developers. It should
  // be a small step going from paths to dereferencing to getters and setters.

  std::deque<std::unique_ptr<ISegment> > segments;

  Path();
  Path(const Path& other);
  Path(Path&& other);
  Path& operator=(const Path& other);
  Path& operator=(Path&& other);

  std::wstring ToWstring() const;
};  // struct Path

// endregion Pathing

// region Iterators and descent

/// \cond HIDDEN
namespace impl {
class IIterator {
 public:
  virtual void Start() = 0;
  virtual void Next() = 0;
  virtual bool Done() const = 0;
  virtual const std::shared_ptr<types::IClass>& Get() const = 0;
  virtual long Index() const = 0;

  /// Prepend the segments to the path reflecting where this iterator points to.
  virtual void PrependToPath(Path* path) const = 0;

  virtual std::unique_ptr<IIterator> Clone() const = 0;

  virtual ~IIterator() = default;
};  // class IIterator
}  // namespace impl
/// \endcond

/**
 * \brief Iterate over an AAS instance.
 *
 * Unlike STL, this is <em>not</em> a light-weight iterator. We implement
 * a "yielding" iterator by leveraging code generation so that we always keep
 * the model stack as well as the properties iterated thus far.
 *
 * This means that copy-construction and equality comparisons are much more heavy-weight
 * than you'd usually expect from an STL iterator. For example, if you want to sort
 * model instances, you are most probably faster if you populate a vector, and then
 * sort the vector.
 *
 * Also, given that this iterator is not light-weight, you should in almost all cases
 * avoid the postfix increment (it++) and prefer the prefix one (++it) as the postfix
 * increment would create an iterator copy every time.
 *
 * The value of the iterator is intentionally constant reference to a shared pointer.
 * This merely means that you can not change the <em>pointer</em> while you are
 * iterating. The pointed instances, however, is freely mutable. This way you can make
 * further shared pointers, or include the pointed instances in other collections
 * different from the original container. On the other hand, the normal case, where
 * the pointer is only de-referenced, remains efficient as no copy of
 * the shared pointer is created.
 *
 * We follow the C++ standard, and assume that comparison between the two iterators
 * over two different instances results in undefined behavior. See
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2948.html and
 * https://stackoverflow.com/questions/4657513/comparing-iterators-from-different-containers.
 *
 * Since we use const references to shared pointers here you can also share ownership
 * over instances in your own external containers. Making a copy of a shared pointer
 * will automatically increase reference count, even though there is a constant
 * reference. Since we do not make copies of the shared pointers, it is very important
 * that the given shared pointers outlive the iteration, lest cause undefined behavior.
 *
 * Changing the references <em>during</em> the iteration invalidates the iterators and
 * results in undefined behavior. This is similar to many of the containers in the STL,
 * see: https://stackoverflow.com/questions/6438086/iterator-invalidation-rules-for-c-containers
 *
 * See these StackOverflow questions for performance related to shared pointers and
 * constant references to shared pointers (copying <em>versus</em> referencing):
 * * https://stackoverflow.com/questions/12002480/passing-stdshared-ptr-to-constructors/12002668#12002668
 * * https://stackoverflow.com/questions/3310737/should-we-pass-a-shared-ptr-by-reference-or-by-value
 * * https://stackoverflow.com/questions/37610494/passing-const-shared-ptrt-versus-just-shared-ptrt-as-parameter
 *
 * The following StackOverflow question and answers go into more detail how const-ness
 * and shared pointers fit together:
 * https://stackoverflow.com/questions/36271663/why-does-copying-a-const-shared-ptr-not-violate-const-ness
 */
class Iterator {
  using iterator_category = std::forward_iterator_tag;
  /// The difference is meaningless, but has to be defined.
  using difference_type = std::ptrdiff_t;
  using value_type = std::shared_ptr<types::IClass>;
  using pointer = const std::shared_ptr<types::IClass>*;
  using reference = const std::shared_ptr<types::IClass>&;

 public:
  Iterator(const Iterator& other);
  Iterator(Iterator&& other);

  Iterator& operator=(const Iterator& other);
  Iterator& operator=(Iterator&& other);

  reference operator*() const;
  pointer operator->();

  // Prefix increment
  Iterator& operator++();

  // Postfix increment
  Iterator operator++(int);

  friend bool operator==(const Iterator& a, const Iterator& b);
  friend bool operator!=(const Iterator& a, const Iterator& b);

  friend class Descent;
  friend class DescentOnce; 
  friend Path MaterializePath(const Iterator& iterator);
  friend void PrependToPath(const Iterator& iterator, Path* path);

 private:
  explicit Iterator(
    std::unique_ptr<impl::IIterator> implementation
  ) :
    implementation_(std::move(implementation)) {
      // Intentionally empty.
  }

  std::unique_ptr<impl::IIterator> implementation_;
};

bool operator==(const Iterator& a, const Iterator& b);

bool operator!=(const Iterator& a, const Iterator& b);

/**
 * \brief Materialize the path that the \p iterator points to.
 *
 * We assume that you always want a copy of the path, rather than inspect
 * the path during the iteration.
 *
 * \param iterator for which we want to materialize the path
 * \return Path referring to the pointed instance
 */
Path MaterializePath(const Iterator& iterator);

/**
 * Build a facade over an instance to iterate over instances referenced from it.
 */
class IDescent {
 public:
  virtual Iterator begin() const = 0;
  virtual const Iterator& end() const = 0;
  virtual ~IDescent() = default;
};  // class IDescent

/**
 * \brief Provide a recursive iterable over all the instances referenced from
 * an instance.
 *
 * Please see the notes in the class Iterator regarding the constant reference to
 * a shared pointer. In short, the instance should outlive the descent, so make
 * sure you do not destroy it during the descent.
 *
 * Range-based loops should fit the vast majority of the use cases:
 * \code
 * std::shared_ptr<types::Environment> env = ...;
 * for (
 *   const std::shared_ptr<types::IClass>& instance
 *   : Descent(env)
 * ) {
 *   do_something(instance);
 * }
 * \endcode
 *
 * \param that instance to be iterated over recursively
 * \return Iterable over referenced instances
 */
class Descent : public IDescent {
 public:
  Descent(
    std::shared_ptr<types::IClass> instance
  );

  Iterator begin() const override;
  const Iterator& end() const override;

  ~Descent() override = default;

 private:
  std::shared_ptr<types::IClass> instance_;
};  // class Descent

/**
 * \brief Provide a non-recursive iterable over the instances referenced from
 * an instance.
 *
 * Please see the notes in the class Iterator regarding the constant reference to
 * a shared pointer. In short, the instance should outlive the descent, so make
 * sure you do not destroy it during the descent.
 *
 * Range-based loops should fit the vast majority of the use cases:
 * \code
 * std::shared_ptr<types::Environment> env = ...;
 * for (
 *   const std::shared_ptr<types::IClass>& instance
 *   : DescentOnce(env)
 * ) {
 *   do_something(instance);
 * }
 * \endcode
 */
class DescentOnce : public IDescent {
 public:
  DescentOnce(
    std::shared_ptr<types::IClass> instance
  );

  Iterator begin() const override;
  const Iterator& end() const override;

  ~DescentOnce() override = default;

 private:
  std::shared_ptr<types::IClass> instance_;
};  // class DescentOnce

// endregion Iterators and descent

// region Over enumerations

/**
 * \brief Give a container for all the literals of types::ModellingKind.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::ModellingKind> kOverModellingKind;

/**
 * \brief Give a container for all the literals of types::QualifierKind.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::QualifierKind> kOverQualifierKind;

/**
 * \brief Give a container for all the literals of types::AssetKind.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::AssetKind> kOverAssetKind;

/**
 * \brief Give a container for all the literals of types::AasSubmodelElements.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::AasSubmodelElements> kOverAasSubmodelElements;

/**
 * \brief Give a container for all the literals of types::EntityType.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::EntityType> kOverEntityType;

/**
 * \brief Give a container for all the literals of types::Direction.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::Direction> kOverDirection;

/**
 * \brief Give a container for all the literals of types::StateOfEvent.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::StateOfEvent> kOverStateOfEvent;

/**
 * \brief Give a container for all the literals of types::ReferenceTypes.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::ReferenceTypes> kOverReferenceTypes;

/**
 * \brief Give a container for all the literals of types::KeyTypes.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::KeyTypes> kOverKeyTypes;

/**
 * \brief Give a container for all the literals of types::DataTypeDefXsd.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::DataTypeDefXsd> kOverDataTypeDefXsd;

/**
 * \brief Give a container for all the literals of types::DataTypeIec61360.
 *
 * This container is practical when you want to show the literals in a GUI or a CLI.
 */
extern const std::vector<types::DataTypeIec61360> kOverDataTypeIec61360;

// endregion Over enumerations

}  // namespace iteration
/**@}*/

}  // namespace aas_3_0
}  // namespace aas_core

// This code has been automatically generated by aas-core-codegen.
// Do NOT edit or append.

#endif  // AAS_CORE_AAS_3_0_ITERATION_GUARD_
