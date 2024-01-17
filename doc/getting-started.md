# Getting Started

@tableofcontents

Here's a quick intro to get you started with the SDK. See how you can:

* [Install the SDK](#Installation) so that you can use it in your projects,
* [Programmatically create and modify an AAS model](#create-get-set),
* [Transform enumerators to and from strings](#stringification),
* [Iterate over a model](#iterate),
* [Verify a model](#verify),
* [De/serialize a model from and to JSON](#jsonization),
* [De/serialize a model from and to XML](#xmlization), and
* [Enhance model instances with your custom data](#enhancing).

## Installation {#Installation}

Saying that installing C++ dependencies is opaque and confusing would be an understatement.
There are many compilers, many packagers (vcpkg, conan, *etc.*) and many build systems (CMake, Make, Bazel *etc.*).
We admit that we are by no means experts in C++ compilation.
We give our best to modestly document how to install our SDK and include it in other projects.

At this point, we kindly invite the experts in this area to help us out with packaging and adaptation for different build systems.

### Dependencies

We want to keep the dependencies minimal, and optimally the SDK should have none except the standard library.
Unfortunately, and unlike some other mainstream languages, the C++ standard library provides no JSON and XML parsing out-of-the-box.

What is more, the union types in form of [std::expected] and nullables as [std::optional] were introduced much later in the language, in C++23 and C++17, respectively.
We wanted the SDK to stand the test of time, and already use those novel features.
We also acknowledge that older settings depending on C++11 must be supported for successful adoption.

Therefore, we rely on four external libraries:
* [nlohmann/json] which we use for JSON parsing,
* [expat] which we use for XML parsing,
* [tl::optional] which we use as a fill-in for [std::optional] on older compilers, and
* [tl::expected] which is a fill-in for [std::expected] also for older compilers.

You need to install these dependencies such that CMake can find them using [find_package].

[find_package]: https://cmake.org/cmake/help/latest/command/find_package.html

### Compilation from Source

To compile your project with our SDK as source code dependency, make sure to first install the dependencies mentioned above.

Then clone the repository, and point to it when you call cmake by:

```
cmake -DAAS_CORE_DIR=/path/to/aas/repository
```

Our SDK is exported as `aas_core3` to CMake.
You have to find it first in your `CMakeLists.txt`:

```
find_package(AAS_CORE3 0 REQUIRED)
```

Then you can then link it to your executables in the `CMakeLists.txt` of your project with:

```
add_executable(some_cool_program main.cpp)
target_link_libraries(some_cool_program aas_core3)
```

## AAS Model {#create-get-set}

### From the meta-model to C++

We map the meta-model to C++ like follows:

* **Booleans**. We represent booleans as `bool`.
* **Strings**. We decided to use wide strings, [std::wstring]. In particular, wide strings allows us to use [std::regex].
* **Byte arrays**. The byte arrays are modeled as `[std::vector]<[std::uint8_t]>`.
* **Enumerators**. We define enumerators as [enum class], and fix their underlying type to [std::uint32_t].
  Since the type of the enumerators is fixed, you can always cast them to a number of pre-defined size.
* **Classes**. For each class in the meta-model, we introduce two C++ classes. First, a purely virtual interface
  class (*e.g.,* @ref aas_core::aas_3_0::types::ISubmodel). Then, a concrete class (*e.g.,* @ref aas_core::aas_3_0::types::Submodel). All the pointers in the model use interfaces. We use concrete classes only for construction.
* **References**. We use [std::shared_ptr] to manage all the references in the model.
* **Optional values**. If you are compiling with C++17 and above, we represent optional values with `std::optional`.
  Otherwise, we rely on [tl::optional]. We re-export the one of the two which we use with `using` directive as   `aas_core::aas_3_0::common::optional`.
* **Aggregations**. We model the aggregations of the meta-model as [std::vector]'s in the C++.

### `IClass`

We introduce the most general class, @ref aas_core::aas_3_0::types::IClass, to group all the functionality of the model instances.
This class does not exist in the original AAS meta-model.

[std::uint8_t]: https://en.cppreference.com/w/cpp/types/integer
[std::uint32_t]: https://en.cppreference.com/w/cpp/types/integer
[enum class]: https://en.cppreference.com/w/cpp/language/enum
[std::wstring]: https://cplusplus.com/reference/string/wstring/
[std::regex]: https://cplusplus.com/reference/regex/
[std::shared_ptr]: https://en.cppreference.com/w/cpp/memory/shared_ptr
[tl::optional]: https://github.com/TartanLlama/optional
[std::vector]: https://en.cppreference.com/w/cpp/container/vector

### `model_type` as faster RTTI

The AAS meta-model is given as a deep class hierarchy, with ample multiple inheritance.
Therefore, we have to use [virtual inheritance] in C++ as well.

This makes the dynamic type determination based on [C++ Runtime Type Information (RTTI)] quite slow, see [this StackOverflow question about RTTI efficiency].
In addition, as C++ does not support type switches.
If you have to dynamically act upon the runtime type, you have to write a long sequence of `if (...) else if (...) else if (...) `'s.

To alleviate that problem, each instance of the model is given an immutable getter of the runtime type as @ref aas_core::aas_3_0::types::IClass::model_type.
The model type will hold one of the concrete literals of the enumerator @ref aas_core::aas_3_0::types::ModelType, corresponding to the runtime type of model instance. 

[virtual inheritance]: https://en.wikipedia.org/wiki/Virtual_inheritance 
[this StackOverflow question about RTTI efficiency]: https://stackoverflow.com/questions/579887/how-expensive-is-rtti
[Runtime Type Information (RTTI)]: https://en.wikipedia.org/wiki/Run-time_type_information

@note @parblock
Unfortunately, [virtual inheritance] also makes fast static pointer casts impossible, even when you know upfront the runtime type of pointer.
You have to use [std::dynamic_pointer_cast] instead, which is slower as it relies on RTTI.
When you need to cast among different model types, @ref aas_core::aas_3_0::types::IClass::model_type will still spare you a sequence of `if-else if-else if`'s.

In any case, fast upcasting from a concrete class to a more abstract one is still possible with [std::static_pointer_cast].
@endparblock

The SDK also gives you functions `IsXxx` to check for a single model type in order to avoid RTTI.
See, for example, @ref aas_core::aas_3_0::types::IsSubmodel.

[std::dynamic_pointer_cast]: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

### Getters and Setters

Each attribute of a class is represented with a getter, a mutable getter and a setter.
The getter returns a constant value of the attribute, usually a constant reference or a primitive value (*e.g.,* for booleans).
The mutable getter returns a mutable reference.

@note @parblock Since we use [std::shared_ptr], the "const-ness" of the getters does not propagate to the referenced object.
This might be unfortunate in many cases where you want to convey the immutability through "const-ness", but C++ is alas not versatile enough. @endparblock

The namespace @ref aas_core::aas_3_0::types contains all the classes of the meta-model.
You can simply use their constructors to create an AAS model.
Usually you start bottom-up, all the way up to the @ref aas_core::aas_3_0::types::IEnvironment.

@note @parblock You will often have to cast when initializing or passing around aggregations, as C++ containers, including [std::vector], are invariant.
See: https://softwareengineering.stackexchange.com/questions/399277/why-doesnt-c-support-covariance-in-stl-containers-like-c-or-java
@endparblock

### Getters with a Default Value

For optional properties which come with a default value, we provide special getters, `{property name}OrDefault`.
If the property has not been set, this getter will give you the default value. Otherwise, if the model sets the property, the value of the property will be returned.

For example, see @ref aas_core::aas_3_0::types::ISubmodel::KindOrDefault.

### Wide Strings and UTF-8

Being an old language which predates unicode, C++ is a bit messy when it comes to dealing with unicode strings.
At the moment, there is no standard way how to represent, encode and decode unicode strings to and from different formats.

We decided to use [std::wstring], which seemed as a solution to fit the widest range of the use cases.
However, wide strings do not fit *all* the use cases.
For example, whenever you output something to a console or a text file, it is usually encoded in UTF-8.
You serialize then a wide string and encode it as UTF-8 [std::string], and *vice versa*.

The SDK provides two functions, @ref aas_core::aas_3_0::common::WstringToUtf8 and @ref aas_core::aas_3_0::common::Utf8ToWstring, to convert between UTF-8 and wide strings.
Both functions rely on basic functions provided by your operating system.
If you are on Windows, they simply wrap [MultiByteToWideChar] and [WideCharToMulitByte].
In Linux, they will use [std::codecvt].

[MultiByteToWideChar]: https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
[WideCharToMulitByte]: https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
[std::codecvt]: https://en.cppreference.com/w/cpp/locale/codecvt

### Example: Create an Environment with a Submodel

Here is a very rudimentary example where we show how to create an environment which contains a submodel.

The submodel will contain two elements, a property and a blob.

(We will alias the namespace @ref aas_core::aas_3_0 as `aas` for readability. You might or might not want to write your  code like that; the aliasing is not necessary.)

```c++
#include <aas_core/aas_3_0/types.hpp>

namespace aas = aas_core::aas_3_0;

int main() {
  // We create the first element with the constructor.
  std::shared_ptr<aas::types::IProperty> some_element(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );

  // We use setters to set the optional attributes.
  some_element->set_id_short(L"someElement");
  some_element->set_value(L"1984");

  // We create the second element.
  std::shared_ptr<aas::types::IBlob> another_element(
    new aas::types::Blob(
      L"application/octet-stream"
    )
  );

  another_element->set_id_short(L"anotherElement");
  another_element->set_value(
    std::vector<std::uint8_t>{
      0xDE, 0xAD, 0xBE, 0xEF
    }
  );

  // We also directly access the element attributes.
  another_element->mutable_value()->at(3) = 0xAD;

  // We nest the elements in a submodel.
  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );

  // We have to cast since C++ containers are invariant.
  // Dynamic casts are necessary due to virtual inheritance.
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      std::dynamic_pointer_cast<aas::types::ISubmodelElement>(
        some_element
      ),
      std::dynamic_pointer_cast<aas::types::ISubmodelElement>(
        another_element
      )
    }
  );

  // We now create the environment to wrap it all up.
  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );

  environment->set_submodels(
    std::vector<std::shared_ptr<aas::types::ISubmodel> >{
      submodel
    }
  );

  // We can directly access the properties from the children
  // as well.
  std::dynamic_pointer_cast<aas::types::IBlob>(
    environment
      ->submodels()
      ->at(0)
      ->submodel_elements()
      ->at(1)
  )->mutable_value()->at(3) = 0xEF;

  // Now we can do something with the environment...
  
  return 0; 
}
```

## Enumerators from and to Strings

Enumerators are basically integer numbers in C++.
This is efficient when you manipulate them, but makes the enumerator cumbersome to work with when you need to use them in user interfaces, embed them in documents *etc.*
The SDK provides two modules, @ref aas_core::aas_3_0::stringification and @ref aas_core::aas_3_0::wstringification, to convert enumerators to and from strings and wide strings, respectively.

All enumerators are converted using the overloaded functions @ref aas_core::aas_3_0::stringification::to_string and @ref aas_core::aas_3_0::wstringification::to_wstring, respectively.

To parse an enumerator from a string to a literal, use `XxxFromString` and `XxxFromWstring`, respectively.
For example, see @ref aas_core::aas_3_0::stringification::ModelTypeFromString.

If you want to iterate over all literals of an enumerator, the SDK provides `kOverXxx` in @ref aas_core::aas_3_0::iteration.
For example, @ref aas_core::aas_3_0::iteration::kOverAasSubmodelElements.

## Iterate {#iterate}

The SDK provides various ways how you can loop through the elements of the model.
The following sections will look into each one of the approaches.

The iteration code resides in @ref aas_core::aas_3_0::iteration.

### `Descent` and `DescentOnce`

The SDK provides two iterables, @ref aas_core::aas_3_0::iteration::Descent and @ref aas_core::aas_3_0::iteration::DescentOnce.
You give them an instance of the model, and they provide `begin()` and `end()` functions.
This is particularly practical when you directly pass them into [range-based for loops].

[range-based for loops]: https://en.cppreference.com/w/cpp/language/range-for

As their names suggest, `Descent` iterates recursively over all the instances referenced by the original instance.
In contrast, `DescentOnce` iterates only over the instanced referenced by the original instance, and does not proceed recursively.

Here is a short example how you can iterate over all the instances of a model, and output their model types:

```C++
#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodelElement> another_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  another_property->set_id_short(L"anotherProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > yet_another_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  yet_another_property->set_id_short(L"yetAnotherProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property,
      another_property,
      yet_another_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We descend recursively over all the instances.
  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(environment)
  ) {
    std::cout
      << aas::stringification::to_string(something->model_type())
      << std::endl;
  }
  
  // Prints:
  // Submodel
  // Property
  // Property
  // Property
  
  return 0;
}
```

### Filtering with `Descent` and `DescentOnce`

Filtering with `Descent` and `DescentOnce` is fairly easy, as we can write the filtering code within a for loop.

Here is a short snippet, adapted from the previous example, where we filter for all properties which have `int` as their value type:

```C++
#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> bool_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  bool_property->set_id_short(L"boolProperty");

  std::shared_ptr<aas::types::ISubmodelElement> int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  int_property->set_id_short(L"intProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > another_int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  another_int_property->set_id_short(L"anotherIntProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      bool_property,
      int_property,
      another_int_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We descend recursively over all the instances
  // and filter for the int properties.

  for (
    const std::shared_ptr<aas::types::IClass>& something
    : aas::iteration::Descent(environment)
  ) {
    // We use model_type getter instead of RTTI since
    // this is faster.
    if (aas::types::IsProperty(*something)) {
      // We have to use dynamic pointer cast due to virtual
      // inheritance.
      auto property = std::dynamic_pointer_cast<
        aas::types::IProperty
      >(something);

      if (
        property->value_type()
          == aas::types::DataTypeDefXsd::kInt
      ) {
        std::cout << aas::common::WstringToUtf8(
          *(property->id_short())
        ) << std::endl;
      }
    }
  }
  
  // Prints:
  // intProperty
  // anotherIntProperty
  
  return 0;
}
```

### Visitors

For loops are practical for short iterations.
When you need to apply an action for each different model type, your loop body will probably become unbearably long.
Such iterations are much better solved by using the [visitor pattern].
You specify for each model type a separate method in the visitor, and the visitor automatically dispatches the calls based on the type.

[visitor pattern]: https://en.wikipedia.org/wiki/Visitor_pattern

**`IVisitor`**. The SDK provides an abstract, purely virtual, interface @ref aas_core::aas_3_0::visitation::IVisitor.
If you want to steer how the children references from an instance are handled, then implement this interface.

**`AbstractVisitor`**. Most of the time, you want to process each instance of a model in isolation, so you do not want to call the visitting methods on the children references.
The abstract visitor @ref aas_core::aas_3_0::visitation::AbstractVisitor will take care of the children references.
You merely have to specify the operation for each model type.

**`PassThroughVisitor`**. If you want to process only some of the model types, use the class @ref aas_core::aas_3_0::visitation::PassThroughVisitor.
This class will iterate recursively over all the referenced instances, and simply perform no action for instances of model types for which you did not override the corresponding visiting method.

Here is an example with a @ref aas_core::aas_3_0::visitation::PassThroughVisitor, which has been adapted from the previous filtering example:

```C++
#include <aas_core/aas_3_0/types.hpp>
#include <aas_core/aas_3_0/visitation.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

class MyVisitor : public aas::visitation::PassThroughVisitor {
 public:
  MyVisitor(
    std::ostream& ostream
  ) :
    ostream_(&ostream) {
    // Intentionally empty.
  }

  void VisitProperty(
    const std::shared_ptr<aas::types::IProperty>& that
  ) override {
    if (
      that->value_type()
        == aas::types::DataTypeDefXsd::kInt
    ) {
      (*ostream_) << aas::common::WstringToUtf8(
        *(that->id_short())
      ) << std::endl;
    }
  }

  ~MyVisitor() = default;

 private:
  std::ostream* ostream_;
};


int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> bool_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  bool_property->set_id_short(L"boolProperty");

  std::shared_ptr<aas::types::ISubmodelElement> int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  int_property->set_id_short(L"intProperty");

  std::shared_ptr<
    aas::types::ISubmodelElement
  > another_int_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  another_int_property->set_id_short(L"anotherIntProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      bool_property,
      int_property,
      another_int_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We instantiate the visitor.
  MyVisitor my_visitor(std::cout);

  // We visit the environment.
  my_visitor.Visit(environment);

  // Prints:
  // intProperty
  // anotherIntProperty

  return 0;
}
```

## Verify {#verify}

Our SDK allows you to verify that a model satisfies the constraints of the meta-model.
The verification logic is concentrated in @ref aas_core::aas_3_0::verification.

Similar to descent, the verification is represented as an iterable over verification errors.
For a recursive verification over the instance and its references, use @ref aas_core::aas_3_0::verification::RecursiveVerification.
If you only want to check the instance and its immediate references, use @ref aas_core::aas_3_0::verification::NonRecursiveVerification.

The reporting of errors is probably easiest in a [range-based for loop].
If you want to report only a pre-determined number of errors, you can simply break out of the loop.

@note @parblock
We use `long` to handle verifying years in `xs:Date`, `xs:DateTime` *etc.*
For example, to determine if the year is a leap year.
This is important for astronomical applications which might use more than 64-bit year numbers – we could not find any production-ready `BigInt` implementation which can be easily included.
Please let us know if you need astronomical years outside the 64-bit range.

Note that some of the other SDKs suffer from the same issue (TypeScript), while others do not (Python, C# and Golang), which avoid the issue by using `BigInt`'s.
@endparblock

Here is an example which verifies an environment against the meta-model constraints:

```C++
#include <aas_core/aas_3_0/types.hpp>
#include <aas_core/aas_3_0/verification.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
    // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  
  // We intentionally introduce a mistake.
  some_property->set_id_short(L"!@#$% invalid ID short (*&^_+");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We verify the environment and report the errors.
  for (
    const aas::verification::Error& error
    : aas::verification::RecursiveVerification(environment)
  ) {
    std::cout
      << aas::common::WstringToUtf8(error.path.ToWstring())
      << ": "
      << aas::common::WstringToUtf8(error.cause)
      << std::endl;
  }
  
  // Prints (without newlines):
  // .submodels[0].submodel_elements[0]: ID-short of Referables shall
  // only feature letters, digits, underscore (``_``); starting
  // mandatory with a letter. *I.e.* ``[a-zA-Z][a-zA-Z0-9_]*``.

  return 0;
}
```

### Reflection?

The path to the erroneous value is kept in @ref aas_core::aas_3_0::iteration::Path, based on the enumeration @ref aas_core::aas_3_0::iteration::Property.

We did not implement the reflection at the moment since we did not have a use case for it.
If you need reflection, please contact the developers.
It should be a small step going from paths to de-referencing to getters and setters.

## JSON De/serialization {#jsonization}

Our SDK handles the de/serialization of the AAS models from and to JSON format through the @ref aas_core::aas_3_0::jsonization.

Since C++ does not support JSON de/serialization in the standard library, we use [nlohmann/json] as the underlying representation of JSON values.
We thus do not directly de/serialize from and to strings, but rely on [nlohmann/json] for intermediate handling.

[nlohmann/json]: https://github.com/nlohmann/json

### Serialize

The model instances are converted to [nlohmann/json] using @ref aas_core::aas_3_0::jsonization::Serialize.

Here is an example code:

```C++
#include <aas_core/aas_3_0/jsonization.hpp>
#include <aas_core/aas_3_0/types.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{some_property}
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We serialize the environment to JSON.
  nlohmann::json json = aas::jsonization::Serialize(
    *environment
  );

  // We can then serialize JSON to text.
  std::cout << json << std::endl;

  // Prints (without newlines):
  // {"submodels":[{
  // "id":"some-unique-global-identifier",
  // "modelType":"Submodel",
  // "submodelElements":[{
  // "idShort":"someProperty",
  // "modelType":"Property",
  // "valueType":"xs:int"
  // }]}]}  

  return 0;
}
```

### De-serialize

To translate [nlohmann/json] value to a model instance, the SDK offers `XxxFrom` de-serialization functions.
For example, to de-serialize an environment, call @ref aas_core::aas_3_0::jsonization::EnvironmentFrom.

The de-serialization functions return a `aas_core::aas_3_0::common::expected`, a union type holding either the de-serialized instance or an error.

<a name="note-expected"></a>
@note @parblock 
As a matter of fact, we did not implement `aas_core::aas_3_0::common::expected` ourselves.
If your compiler supports C++23, [std::expected] will be used underneath.
Otherwise, a fill-in in form of [tl::expected] is used.
@endparblock


[std::expected]: https://en.cppreference.com/w/cpp/utility/expected
[tl::expected]: https://github.com/TartanLlama/expected

Here is an example:

```C++
#include <aas_core/aas_3_0/jsonization.hpp>
#include <aas_core/aas_3_0/types.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
  // We specify the JSON as text.
  std::string text(
    "{\"submodels\":[{"
    "\"id\":\"some-unique-global-identifier\","
    "\"modelType\":\"Submodel\","
    "\"submodelElements\":[{"
    "\"idShort\":\"someProperty\","
    "\"modelType\":\"Property\","
    "\"valueType\":\"xs:int\""
    "}]}]}\n"
  );

  // We parse text as JSON.
  nlohmann::json json = nlohmann::json::parse(
    text
  );

  // We parse the JSON as environment.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > environment = aas::jsonization::EnvironmentFrom(json);

  // environment.has_value() will be true.

  // Now do something with *environment...
  return 0;
}  
```

Here is another example of how to report the errors:

```C++
#include <aas_core/aas_3_0/jsonization.hpp>
#include <aas_core/aas_3_0/types.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace aas = aas_core::aas_3_0;

int main() {
  // We specify the text which is valid JSON,
  // but not a valid representation of a model.
  std::string text(
    "{\"submodels\": 42}"
  );

    // We parse text as JSON.
  nlohmann::json json = nlohmann::json::parse(
    text
  );

  // We try to parse the JSON as environment.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::jsonization::DeserializationError
  > environment = aas::jsonization::EnvironmentFrom(json);

  // We check for the error, and if any, we report it.
  if (!environment.has_value()) {
    std::cerr
      << aas::common::WstringToUtf8(
        environment.error().path.ToWstring()
      )
      << ": "
      << aas::common::WstringToUtf8(
        environment.error().cause
      );
    
    return 1;
  }
  
  // Prints to stderr:
  // .submodels: Expected an array, but got: number
  
  return 0;
}
```

## XML De/serialization {#xmlization}
The code that de/serializes AAS models from and to XML documents lives in @ref aas_core::aas_3_0::xmlization.

### Serialization
You serialize a model using the overloaded functions @ref aas_core::aas_3_0::xmlization::Serialize.

Apart from the object, you pass it the writing options (@ref aas_core::aas_3_0::xmlization::WritingOptions) and the stream to write to.
The default options should cover the majority of the use cases in the field.
The stream is assumed to be encoded in UTF-8, *i.e.,* we write the text encoded in UTF-8.

Here is an example:

```C++
#include "aas_core/aas_3_0/types.hpp"
#include "aas_core/aas_3_0/xmlization.hpp"

namespace aas = aas_core::aas_3_0;

int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kInt
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{some_property}
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We serialize the environment to a stream
  // as XML.
  aas::xmlization::Serialize(
    *environment,
    aas::xmlization::WritingOptions(),
    std::cout
  );

  // Prints (without the newlines):
  // <?xml version="1.0" encoding="utf-8"?>
  // <environment xmlns="https://admin-shell.io/aas/3/0">
  // <submodels>
  // <submodel>
  // <id>some-unique-global-identifier</id>
  // <submodelElements>
  // <property>
  // <idShort>someProperty</idShort>
  // <valueType>xs:int</valueType>
  // </property>
  // </submodelElements>
  // </submodel>
  // </submodels>
  // </environment>

  return 0;
}
```

### De-serialization

De-serialization goes in the opposite direction, with overloaded functions `XxxFrom`.
For example, @ref aas_core::aas_3_0::xmlization::EnvironmentFrom.

The model instances are de-serialized from a text stream encoded in UTF-8.
There is an argument to steer the reading, @ref aas_core::aas_3_0::xmlization::ReadingOptions.
The defaults are picked such that they cover the majority of the cases.

C++ does not provide any XML parsing in the standard library, so we use [Expat], but the user does not interact with it directly.

[Expat]: https://libexpat.github.io/

Similar to JSON, the result of the parsing is returned as `aas_core::aas_3_0::common::expected`.
This is a union type between a model instance and the de-serialization error.
Please see [the previous note in JSON de-serialization](#note-expected) related to how this union type is actually implemented

Here is an example how to de-serialize successfully a model instance from a text string:

```C++
#include "aas_core/aas_3_0/xmlization.hpp"

namespace aas = aas_core::aas_3_0;

int main() {
  std::string text(
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<environment xmlns=\"https://admin-shell.io/aas/3/0\">"
    "<submodels>"
    "<submodel>"
    "<id>some-unique-global-identifier</id>"
    "<submodelElements>"
    "<property>"
    "<idShort>someProperty</idShort>"
    "<valueType>xs:int</valueType>"
    "</property>"
    "</submodelElements>"
    "</submodel>"
    "</submodels>"
    "</environment>"
  );

  // We prepare the input stream.
  std::stringstream istream;
  istream << text;
  istream.seekp(0);

  // We de-serialize the environment from
  // an XML text.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::xmlization::DeserializationError
  > environment = aas::xmlization::EnvironmentFrom(
    istream
  );

  if (!environment.has_value()) {
    // Report the error, see the next example.
  }
  
  // Do something with the `*environment`...
  
  return 0;
}
```

Here is another example how you can report the error:

```
#include "aas_core/aas_3_0/xmlization.hpp"

namespace aas = aas_core::aas_3_0;

int main() {
  // We define a valid XML, but invalid representation
  // of an AAS instance. Submodels should be a list,
  // not a number.
  std::string text(
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<environment xmlns=\"https://admin-shell.io/aas/3/0\">"
    "<submodels>"
    "42"
    "</submodels>"
    "</environment>"
  );

  // We prepare the input stream.
  std::stringstream istream;
  istream << text;
  istream.seekp(0);

  // We try to de-serialize the environment from
  // an XML text.
  aas::common::expected<
    std::shared_ptr<aas::types::IEnvironment>,
    aas::xmlization::DeserializationError
  > environment = aas::xmlization::EnvironmentFrom(
    istream
  );

  if (!environment.has_value()) {
    std::cerr
      << environment.error().path.ToWstring()
      << ": "
      << environment.error().cause
      << std::endl;
    
    return 1;
  }
  
  // Prints (without newlines):
  // environment/submodels/*[0]: Expected
  // a start element opening an instance 
  // of ISubmodel, but got an XML text
  
  return 0;
}
```

## Enhancing {#enhancing}

In any complex application, creating, modifying and de/serializing AAS instances is not enough.
You have to insert your custom application-specific data to the model in order for the model to be useful.

Take, for example, parent-child relationship. 
The current library ignores it, and there is no easy way for you to find out to which @ref aas_core::aas_3_0::types::ISubmodel a particular @ref aas_core::aas_3_0::types::ISubmodelElement belongs to.

We did want to keep the types as simple as possible — the parent-child relationships can get tricky very soon if you have multiple environments with shared submodels *etc.*
Instead of overcomplicating the code and making it barely traceable, we decided to keep it simple and frugal in features.

However, that is little solace if you are developing an GUI editor where you know for sure that there will be only one environment, and where parent-child relationships are crucial for so many tasks.
What is more, parent-child relationships are not the only data that need to be intertwined — you probably want history, localized caches *etc.*

### Hasthable?

There are different ways how application-specific data can be synced with the model.
One popular technique is to use [Hashtable]'s and simply map model instances to your custom nuggets of data.
This works well if the data is read-only, and you can spare the cycles for the lookups (which is often acceptable as they run on average in time complexity `O(1)` anyhow).

[Hashtable]: https://learn.microsoft.com/en-US/dotnet/api/system.collections.hashtable

Otherwise, if you need to modify the data, maintaining the consistency between the [Hashtable] and your nuggets of information becomes difficult.
For example, if you forget to remove the entries from the [Hashtable] when you remove the instances from the model, you might clog your garbage collector.

### Wrapping

Hence, if you modify the data, you need to keep it close to the model instance.
In dynamic languages, such as Python and JavaScript, you can simply add your custom fields to the object.
This does not work in such a static language like C++.

One solution, usually called [Decorator pattern], is to *wrap* or *decorate* the instances with your application-specific data.
The decorated objects should satisfy both the interface of the original model and provide a way to retrieve your custom nuggets of information.

[Decorator pattern]: https://en.wikipedia.org/wiki/Decorator_pattern

Writing wrappers for many classes in the AAS meta-model is a tedious task.
We therefore pre-generated the most of the boilerplate code in @ref aas_core::aas_3_0::enhancing.

In the context of decoration, we call your specific data *enhancements*.
First, you need to specify how individual instances are enhanced, *i.e.* how to produce enhancements for each one of them.
We call this an *enhancement factory*.
Second, you need to recursively wrap your instances with the given enhancement factory.

The @ref aas_core::aas_3_0::enhancing is generic and can work with any form of enhancement classes.
You need to specify your enhancement factory as a [std::function] which takes an instance of @ref aas_core::aas_3_0::types::IClass as input and returns a [std::shared_ptr] to an enhancement, or `null`, if you do not want to enhance the particular instance.

[std::function]: https://en.cppreference.com/w/cpp/utility/functional/function

The wrapping and unwrapping is specified in the generic function @ref aas_core::aas_3_0::enhancing::Wrap.
This function takes the instance that you want to wrap (*i.e.,* decorate) and the enhancement factory, and recursively wraps it with the produced enhancements.
The wrapped instance will be finally returned.

@note @parblock
Make sure that you replace the previous reference to your *unwrapped* instance with the wrapped one.
Otherwise, you will get null-pointer exceptions and/or segmentation faults at unwrapping.
In particular, be careful if other parts of the program already referenced the unwrapped instance, and fix such references.
@endparblock

You retrieve the enhancement with @ref aas_core::aas_3_0::enhancing::Unwrap from a model instance.
If the given instance has not been wrapped, `nullptr` is returned.

We also provide a shortcut function, @ref aas_core::aas_3_0::enhancing::MustUnwrap, which always return a pointer to the enhancement, but throws an exception if the instance has not been previously wrapped.

### Example: Parent-Child Enhancement

Let us now consider the aforementioned example.
We want to keep track of parent-child relationships in a model.

The following code snippets first constructs an environment for illustration.
Then we specify the enhancement such that each instance is initialized with the parent set to `nullptr`.
Finally, we modify the enhancements such that they reflect the actual parent-child relationships.

```C++
#include "aas_core/aas_3_0/enhancing.hpp"
#include "aas_core/aas_3_0/types.hpp"

namespace aas = aas_core::aas_3_0;

// We define the enhancement, a parent-child relationship.
struct ParentEnhancement {
  aas::common::optional<
    std::shared_ptr<aas::types::IClass>
  > parent;
};

// We define the enhancement factory.
std::shared_ptr<ParentEnhancement> ProduceParentEnhancement(
  const std::shared_ptr<aas::types::IClass>&
) {
  return std::make_shared<ParentEnhancement>();
}

int main () {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We enhance the environment recursively.
  environment = aas::enhancing::Wrap<
    ParentEnhancement
  >(
    environment,
    ProduceParentEnhancement
  );

  // We set the parent in the enhancement.
  std::queue<
    std::shared_ptr<aas::types::IClass>
  > queue;

  queue.push(environment);

  while (!queue.empty()) {
    std::shared_ptr<aas::types::IClass> instance(
      std::move(queue.front())
    );
    queue.pop();

    for (
      const std::shared_ptr<
        aas::types::IClass
      > child
      : aas::iteration::DescentOnce(instance)
      ) {
      std::shared_ptr<ParentEnhancement> enhancement(
        aas::enhancing::MustUnwrap<ParentEnhancement>(
          child
        )
      );

      enhancement->parent = instance;
    }
  }

  // We retrieve the parent of the first submodel.
  std::shared_ptr<ParentEnhancement> enhancement(
    aas::enhancing::MustUnwrap<ParentEnhancement>(
      environment->submodels()->at(0)
    )
  );

  std::cout
    << (
      (*(enhancement->parent)).get()
      == environment.get()
    ) 
    << std::endl;
  );
  
  // Prints:
  // 1
}
```

Note that this approach is indeed more maintainable than the one with [Hashtable], but you still need to take extra care.
If you create new submodels and insert them into the environment, you have to make sure that you wrap them appropriately.
If you move a submodel from one environment to another, you have to update the parent link manually *etc.*

### Example: Selective Enhancement

We demonstrate now how you can selectively enhance only some instances in the @ref aas_core::aas_3_0::types::IEnvironment.

For example, let us assign a unique identifier to all instances which are referable, *i.e.*, implement @ref aas_core::aas_3_0::types::IReferable.
All the other instances are not enhanced.

```
#include "aas_core/aas_3_0/enhancing.hpp"
#include "aas_core/aas_3_0/types.hpp"

namespace aas = aas_core::aas_3_0;

int main() {
  // We prepare the environment.
  std::shared_ptr<aas::types::ISubmodelElement> some_property(
    new aas::types::Property(
      aas::types::DataTypeDefXsd::kBoolean
    )
  );
  some_property->set_id_short(L"someProperty");

  std::shared_ptr<aas::types::ISubmodel> submodel(
    new aas::types::Submodel(
      L"some-unique-global-identifier"
    )
  );
  submodel->set_submodel_elements(
    std::vector<
      std::shared_ptr<aas::types::ISubmodelElement>
    >{
      some_property
    }
  );

  std::shared_ptr<aas::types::IEnvironment> environment(
    new aas::types::Environment()
  );
  environment->set_submodels(
    std::vector<
      std::shared_ptr<aas::types::ISubmodel>
    >{submodel}
  );

  // We set up the enhancing factory.
  long next_id = 0;

  std::function<
    std::shared_ptr<IdEnhancement>(
      const std::shared_ptr<aas::types::IClass>&
    )
  > enhancement_factory = [&](
    const std::shared_ptr<aas::types::IClass>& that
  ) {
    // We only enhance the IReferable's.
    if (aas::types::IsReferable(*that)) {
      auto result = std::make_shared<IdEnhancement>(
        next_id
      );
      ++next_id;
      return result;
    }

    return std::shared_ptr<
      IdEnhancement
    >(nullptr);
  };

  // We enhance the environment recursively.
  environment = aas::enhancing::Wrap<
    IdEnhancement
  >(
    environment,
    enhancement_factory
  );

  // The environment is not a referable,
  // so no wrapping expected.
  std::cout
    << (
      aas::enhancing::Unwrap<IdEnhancement>(
        environment
      )
      == nullptr
    );
    
  // Prints:
  // 1

  // We output the IDs.
  std::stringstream ss;

  for (
    const std::shared_ptr<aas::types::IClass> something
    : aas::iteration::Descent(environment)
  ) {
    if (aas::types::IsReferable(*something)) {
      ss
        << aas::stringification::to_string(
          something->model_type()
        )
        << " with our ID "
        << aas::enhancing::MustUnwrap<IdEnhancement>(
          something
        )->id
        << std::endl;
    }
  }
  
  // Prints:
  // Submodel with our ID 1
  // Property with our ID 0
  
  return 0;
}
```

## No Re-wraps Allowed

We disallow re-wraps of already wrapped instances to avoid costly iterations over the object trees, and throw an exception.
Additionally, we want to prevent bugs in many settings where the enhancement factory assigns unique identifiers to instances or performs non-idempotent operations.
