"""Generate the code to load minimal and maximal examples."""

import io
import os
import pathlib
import sys
from typing import List

import aas_core_codegen.naming
from aas_core_codegen import intermediate
from aas_core_codegen.common import Stripped, Identifier
from aas_core_codegen.cpp import (
    common as cpp_common,
    naming as cpp_naming,
)
from aas_core_codegen.cpp.common import (
    INDENT as I,
    INDENT2 as II,
    INDENT3 as III,
    INDENT4 as IIII,
)
from icontract import ensure

import test_codegen.common
import test_codegen.test_data_io


def _generate_load_min_max_definitions(
    cls: intermediate.ConcreteClass,
) -> List[Stripped]:
    """Generate the def. of loading minimal and maximal example, respectively."""
    interface_name = cpp_naming.interface_name(cls.name)

    load_min = cpp_naming.function_name(Identifier(f"load_min_{cls.name}"))
    load_max = cpp_naming.function_name(Identifier(f"load_max_{cls.name}"))

    return [
        Stripped(
            f"""\
/**
 * Load a minimal example of {interface_name} by
 * de-serializing it from an XML file.
 */
std::shared_ptr<
{I}aas_core::aas_3_0::types::{interface_name}
> {load_min}();"""
        ),
        Stripped(
            f"""\
/**
 * Load a maximal example of {interface_name} by
 * de-serializing it from an XML file.
 */
std::shared_ptr<
{I}aas_core::aas_3_0::types::{interface_name}
> {load_max}();"""
        ),
    ]


# fmt: off
@ensure(
    lambda result:
    result.endswith('\n'),
    "Trailing newline mandatory for valid end-of-files"
)
# fmt: on
def _generate_header(symbol_table: intermediate.SymbolTable, warning: str) -> str:
    """Generate the header for loading minimal and maximal examples."""
    blocks = [
        warning,
        # NOTE (mristin):
        # We de-serialize from an XML file instead of JSON to avoid problems
        # with missing `modelType` JSON properties for some classes such as
        # `Abstract_lang_string`.
        Stripped(
            """\
/**
* Load minimal and maximal examples from an XML file.
*/"""
        ),
        Stripped(
            """\
#ifndef AAS_CORE3_TEST_COMMON_EXAMPLES_GENERATED_HPP_
#define AAS_CORE3_TEST_COMMON_EXAMPLES_GENERATED_HPP_

#include <aas_core/aas_3_0/types.hpp>

namespace test {
namespace common {
namespace examples {"""
        ),
    ]

    for cls in symbol_table.concrete_classes:
        blocks.extend(_generate_load_min_max_definitions(cls=cls))

    blocks.extend(
        [
            Stripped(
                """\
}  // namespace examples
}  // namespace common
}  // namespace test

#endif // AAS_CORE3_TEST_COMMON_EXAMPLES_GENERATED_HPP_"""
            ),
            warning,
        ]
    )

    writer = io.StringIO()
    for i, block in enumerate(blocks):
        if i > 0:
            writer.write("\n\n")

        writer.write(block)

    writer.write("\n")

    return writer.getvalue()


def _generate_static_type_name(cls: intermediate.ConcreteClass) -> Stripped:
    """Generate the specialization of a templated struct to retrieve the type name."""
    interface_name = cpp_naming.interface_name(cls.name)

    return Stripped(
        f"""\
template<>
struct StaticTypeName<
{I}aas::types::{interface_name}
> {{
  static const char* name;
}};
const char* StaticTypeName<
{I}aas::types::{interface_name}
>::name = "{interface_name}";"""
    )


def _generate_load_min_max_implementations(
    cls: intermediate.ConcreteClass, container_cls: intermediate.ConcreteClass
) -> List[Stripped]:
    """Generate the impl. of loading minimal and maximal example, respectively."""
    interface_name = cpp_naming.interface_name(cls.name)

    load = cpp_naming.function_name(Identifier(f"load_{cls.name}"))
    load_min = cpp_naming.function_name(Identifier(f"load_min_{cls.name}"))
    load_max = cpp_naming.function_name(Identifier(f"load_max_{cls.name}"))

    xml_class_name = aas_core_codegen.naming.xml_class_name(cls.name)

    # NOTE (mristin):
    # So far, we do not really have a convention how to generate the contained-in
    # directory name, so we assume that we use camel case with a model type.
    container_model_type = aas_core_codegen.naming.json_model_type(container_cls.name)

    contained_in_dir_name = (
        "SelfContained"
        if container_cls is cls
        else f"ContainedIn{container_model_type}"
    )

    container_interface_name = cpp_naming.interface_name(container_cls.name)

    return [
        Stripped(
            f"""\
std::shared_ptr<
{I}aas::types::{interface_name}
> {load}(
{I}const std::filesystem::path& path
) {{
{I}

{I}std::shared_ptr<
{II}aas::types::IClass
{I}> abstract = test::common::xmlization::MustReadInstance(
{II}path
{I});

{I}std::shared_ptr<
{II}aas::types::{container_interface_name}
{I}> container(
{II}std::dynamic_pointer_cast<
{III}aas::types::{container_interface_name}
{II}>(
{III}abstract
{II})
{I});

{I}if (container == nullptr) {{
{II}throw std::runtime_error(
{III}aas::common::Concat(
{IIII}"Failed to cast the instance to {container_interface_name} from ",
{IIII}path.string()
{III})
{II});
{I}}}

{I}std::optional<
{II}std::shared_ptr<
{III}aas::types::{interface_name}
{II}>
{I}> first = FindFirstOfType<
{II}aas::types::{interface_name},
{II}aas::types::{container_interface_name}
{I}>(
{II}container
{I});

{I}if (!first.has_value()) {{
{II}throw std::runtime_error(
{III}aas::common::Concat(
{IIII}"Could not find an instance of {interface_name} "
{IIII}"in the XML loaded from ",
{IIII}path.string()
{III})
{II});
{I}}}

{I}return std::move(*first);
}}"""
        ),
        Stripped(
            f"""\
std::shared_ptr<
{I}aas::types::{interface_name}
> {load_min}() {{
{I}const std::filesystem::path path(
{II}test::common::DetermineTestDataDir()
{III}/ "Xml"
{III}/ {cpp_common.string_literal(contained_in_dir_name)}
{III}/ "Expected"
{III}/ {cpp_common.string_literal(xml_class_name)}
{III}/ "minimal.xml"
{I});

{I}return {load}(
{II}path
{I});
}}"""
        ),
        Stripped(
            f"""\
std::shared_ptr<
{I}aas::types::{interface_name}
> {load_max}() {{
{I}const std::filesystem::path path(
{II}test::common::DetermineTestDataDir()
{III}/ "Xml"
{III}/ {cpp_common.string_literal(contained_in_dir_name)}
{III}/ "Expected"
{III}/ {cpp_common.string_literal(xml_class_name)}
{III}/ "maximal.xml"
{I});

{I}return {load}(
{II}path
{I});
}}"""
        ),
    ]


# fmt: off
@ensure(
    lambda result:
    result.endswith('\n'),
    "Trailing newline mandatory for valid end-of-files"
)
# fmt: on
def _generate_implementation(
    symbol_table: intermediate.SymbolTable, warning: str, test_data_dir: pathlib.Path
) -> str:
    """Generate the header for loading minimal and maximal examples."""
    blocks = [
        warning,
        Stripped(
            """\
#include "./common_examples.generated.hpp"
#include "./common.hpp"
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/stringification.hpp>

#include <filesystem>

namespace aas = aas_core::aas_3_0;

namespace test {
namespace common {
namespace examples {"""
        ),
        Stripped(
            """\
template<typename T>
struct StaticTypeName;"""
        ),
    ]

    for cls in symbol_table.concrete_classes:
        blocks.append(_generate_static_type_name(cls))

    blocks.append(
        Stripped(
            f"""\
template<
{I}typename T,
{I}typename C
>
std::optional<
{I}std::shared_ptr<T>
> FindFirstOfType(
{I}const std::shared_ptr<C>& container
) {{
{I}std::shared_ptr<T> casted(
{II}std::dynamic_pointer_cast<T>(
{III}container
{II})
{I});
{I}if (casted != nullptr) {{
{II}return casted;
{I}}}

{I}for (
{II}const std::shared_ptr<aas::types::IClass>& something
{II}: aas::iteration::Descent(container)
{II}) {{
{II}casted = std::dynamic_pointer_cast<T>(something);
{II}if (casted != nullptr) {{
{III}return casted;
{II}}}
{I}}}

{I}throw std::logic_error(
{II}aas::common::Concat(
{III}"Could not find an instance of ",
{III}StaticTypeName<T>::name,
{III}"in the container of type ",
{III}aas::stringification::to_string(container->model_type())
{II})
{I});
}}"""
        )
    )

    environment_cls = symbol_table.must_find_concrete_class(Identifier("Environment"))

    for cls in symbol_table.concrete_classes:
        # fmt: off
        container_cls = (
            test_codegen.test_data_io.determine_container_class(
                cls=cls,
                test_data_dir=test_data_dir,
                environment_cls=environment_cls
            )
        )
        # fmt: on

        blocks.extend(
            _generate_load_min_max_implementations(cls=cls, container_cls=container_cls)
        )

    blocks.extend(
        [
            Stripped(
                """\
}  // namespace examples
}  // namespace common
}  // namespace test"""
            ),
            warning,
        ]
    )

    writer = io.StringIO()
    for i, block in enumerate(blocks):
        if i > 0:
            writer.write("\n\n")

        writer.write(block)

    writer.write("\n")

    return writer.getvalue()


def main() -> int:
    """Execute the main routine."""
    symbol_table = test_codegen.common.load_symbol_table()

    this_path = pathlib.Path(os.path.realpath(__file__))
    repo_root = this_path.parent.parent.parent

    warning = test_codegen.common.generate_warning_comment(
        this_path.relative_to(repo_root)
    )

    header = _generate_header(symbol_table=symbol_table, warning=warning)

    (repo_root / "test/common_examples.generated.hpp").write_text(
        header, encoding="utf-8"
    )

    implementation = _generate_implementation(
        symbol_table=symbol_table,
        warning=warning,
        test_data_dir=repo_root / "test_data",
    )
    (repo_root / "test/common_examples.generated.cpp").write_text(
        implementation, encoding="utf-8"
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
