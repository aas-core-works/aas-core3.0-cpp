"""Generate the code to test the xmlization of concrete classes outside a container."""

import io
import os
import pathlib
import sys
from typing import List

from aas_core_codegen.common import Stripped, Identifier
from aas_core_codegen.cpp import (
    naming as cpp_naming,
)
from aas_core_codegen.cpp.common import (
    INDENT as I,
    INDENT2 as II,
    INDENT3 as III,
    INDENT4 as IIII,
    INDENT5 as IIIII,
)

import test_codegen.common
import test_codegen.test_data_io


def main() -> int:
    """Execute the main routine."""
    symbol_table = test_codegen.common.load_symbol_table()

    this_path = pathlib.Path(os.path.realpath(__file__))
    repo_root = this_path.parent.parent.parent

    warning = test_codegen.common.generate_warning_comment(
        this_path.relative_to(repo_root)
    )

    # noinspection PyListCreation
    blocks = [
        warning,
        Stripped(
            """\
#include "./common_examples.generated.hpp"

#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;"""
        ),
    ]  # type: List[Stripped]

    environment_cls = symbol_table.must_find_concrete_class(Identifier("Environment"))

    for cls in symbol_table.concrete_classes:
        interface_name = cpp_naming.interface_name(cls.name)
        load_min = cpp_naming.function_name(Identifier(f"load_min_{cls.name}"))

        # fmt: off
        container_cls = (
            test_codegen.test_data_io.determine_container_class(
                cls=cls,
                test_data_dir=repo_root / "test_data",
                environment_cls=environment_cls
            )
        )
        # fmt: on

        if container_cls is cls:
            continue

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test the round-trip of an expected {interface_name}") {{
{I}const std::shared_ptr<
{II}aas::types::{interface_name}
{I}> original_instance(
{II}test::common::examples::{load_min}()
{I});

{I}std::stringstream ss;
{I}aas::xmlization::Serialize(
{II}*original_instance,
{II}{{}},
{II}ss
{I});

{I}const std::string original_xml = ss.str();

{I}ss.seekp(0);

{I}aas::common::expected<
{II}std::shared_ptr<aas::types::IClass>,
{II}aas::xmlization::DeserializationError
{I}> deserialized = aas::xmlization::From(
{II}ss
{I});

{I}if (!deserialized.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Failed to make the round-trip Serialize-Deserialize "
{IIII}"a minimal instance of {interface_name}: ",
{IIII}aas::common::WstringToUtf8(
{IIIII}deserialized.error().path.ToWstring()
{IIII}),
{IIII}": ",
{IIII}aas::common::WstringToUtf8(
{IIIII}deserialized.error().cause
{IIII})
{III})
{II})
{II}REQUIRE(deserialized.has_value());
{I}}}

{I}std::stringstream another_ss;
{I}aas::xmlization::Serialize(
{II}*deserialized.value(),
{II}{{}},
{II}another_ss
{I});

{I}INFO(
{II}"Original XML and the XML at the end of "
{II}"the chain Serialize-Deserialize-Serialize for "
{II}"a minimal instance of {interface_name} "
{II}"must coincide"
{I})
{I}REQUIRE(
{II}original_xml
{II}== another_ss.str()
{I});
}}"""
            )
        )

    blocks.append(warning)

    writer = io.StringIO()
    for i, block in enumerate(blocks):
        if i > 0:
            writer.write("\n\n")

        writer.write(block)

    writer.write("\n")

    target_pth = (
        repo_root
        / "test/test_xmlization_of_concrete_classes_outside_container.generated.cpp"
    )
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
