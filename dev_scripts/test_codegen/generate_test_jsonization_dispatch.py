"""Generate the code to test the jsonization of concrete classes."""

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
            f'''\
#include "./common.hpp"
#include "./common_jsonization.hpp"
#include "./common_examples.generated.hpp"

#include <aas_core/aas_3_0/jsonization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;'''
        )
    ]  # type: List[Stripped]

    for cls in symbol_table.classes:
        if len(cls.concrete_descendants) == 0:
            continue

        # NOTE (mristin):
        # We can not de-serialize a JSON without a model type. Some classes indeed
        # lack a model type, although they are abstract. This is the case for
        # the classes where no meta-model property uses the abstract class, and only the
        # concrete descendants are used.
        if not cls.serialization.with_model_type:
            continue

        concrete_cls = cls.concrete_descendants[0]

        interface_name = cpp_naming.interface_name(cls.name)
        concrete_interface_name = cpp_naming.interface_name(concrete_cls.name)

        load_min = cpp_naming.function_name(
            Identifier(f"load_min_{concrete_cls.name}")
        )

        from_json = cpp_naming.function_name(
            Identifier(f"{cls.name}_from")
        )

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test the round-trip of an expected {interface_name}") {{
{I}const std::shared_ptr<
{II}aas::types::{concrete_interface_name}
{I}> concrete_instance(
{II}test::common::examples::{load_min}()
{I});

{I}const nlohmann::json json = aas::jsonization::Serialize(
{II}*concrete_instance
{I});

{I}aas::common::expected<
{II}std::shared_ptr<
{III}aas::types::{interface_name}
{II}>,
{II}aas::jsonization::DeserializationError
{I}> instance = aas::jsonization::{from_json}(
{II}json
{I});

{I}if (!instance.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Failed to deserialize a {interface_name} "
{IIII}"from a minimal {concrete_interface_name}: ",
{IIII}aas::common::WstringToUtf8(instance.error().path.ToWstring()),
{IIII}": ",
{IIII}aas::common::WstringToUtf8(instance.error().cause)
{III})
{II})
{II}CHECK(instance.has_value());
{I}}}

{I}const nlohmann::json another_json = aas::jsonization::Serialize(
{II}**instance
{I});

{I}const std::optional<
{II}std::string
{I}> patch_message = test::common::jsonization::CompareJsons(
{II}json,
{II}another_json
{I});
{I}if (patch_message.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Failed to make a round-trip of a {interface_name} "
{IIII}"over a minimal {concrete_interface_name}: ",
{IIII}*patch_message
{IIII})
{III})
{III}CHECK(!patch_message.has_value());
{I}}}
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

    target_pth = repo_root / "test/test_jsonization_dispatch.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
