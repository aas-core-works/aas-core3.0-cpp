"""Generate the code to test the ``XxxOrDefault`` methods."""

import io
import os
import pathlib
import sys
from typing import List, Optional

from aas_core_codegen import intermediate
from aas_core_codegen.common import Stripped, Identifier, indent_but_first_line
from aas_core_codegen.cpp import (
    naming as cpp_naming,
)
from aas_core_codegen.cpp.common import (
    INDENT as I,
    INDENT2 as II,
    INDENT3 as III,
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
#include "./common.hpp"
#include "./common_examples.generated.hpp"

#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineLogDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "XxxOrDefault";
{I}}}

{I}return *result;
}}"""
        ),
    ]  # type: List[Stripped]

    for cls in symbol_table.concrete_classes:
        cls_name = cpp_naming.class_name(cls.name)
        interface_name = cpp_naming.interface_name(cls.name)

        load_max = cpp_naming.function_name(Identifier(f"load_max_{cls.name}"))
        load_min = cpp_naming.function_name(Identifier(f"load_min_{cls.name}"))

        for method in cls.methods:
            if not method.name.endswith("_or_default"):
                continue

            assert method.returns is not None, (
                f"Unexpected no return type "
                f"from the method {method.name!r} of {cls.name!r}"
            )

            method_name = cpp_naming.method_name(method.name)

            serialization_snippet: Optional[Stripped] = None

            primitive_type = intermediate.try_primitive_type(method.returns)
            if primitive_type is intermediate.PrimitiveType.BYTEARRAY:
                serialization_snippet = Stripped(
                    f"""\
const std::string serialized(
{I}aas::stringification::Base64Encode(
{II}instance->{method_name}()
{I})
);"""
                )
            elif primitive_type is intermediate.PrimitiveType.STR:
                # NOTE (mristin):
                # We use std::wstring's so we have to convert to an UTF-8.
                serialization_snippet = Stripped(
                    f"""\
const std::string serialized(
{I}aas::common::WstringToUtf8(
{II}instance->{method_name}()
{I})
);"""
                )
            elif primitive_type is not None:
                serialization_snippet = Stripped(
                    f"""\
const std::string serialized(
{I}std::to_string(
{II}instance->{method_name}()
{I})
);"""
                )
            else:
                if isinstance(
                    method.returns, intermediate.OurTypeAnnotation
                ) and isinstance(method.returns.our_type, intermediate.Enumeration):
                    serialization_snippet = Stripped(
                        f"""\
const std::string serialized(
{I}aas::stringification::to_string(
{II}instance->{method_name}()
{I})
);"""
                    )

            if serialization_snippet is None:
                raise NotImplementedError(
                    "We have not implemented the serialization of the type "
                    f"{method.returns} in the tests. If you see this message, "
                    f"please revisit the testgen code and implement it."
                )

            blocks.append(
                Stripped(
                    f"""\
TEST_CASE("Test {method_name} on a min. {cls_name}") {{
{I}const std::shared_ptr<
{II}aas::types::{interface_name}
{I}> instance(
{II}test::common::examples::{load_min}()
{I});

{I}const std::filesystem::path log_path(
{II}DetermineLogDir()
{III}/ "{cls_name}.{method_name}.min.log"
{I});

{I}{indent_but_first_line(serialization_snippet, I)}

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}serialized,
{II}log_path
{I});
}}"""
                )
            )

            blocks.append(
                Stripped(
                    f"""\
TEST_CASE("Test {method_name} on a max. {cls_name}") {{
{I}const std::shared_ptr<
{II}aas::types::{interface_name}
{I}> instance(
{II}test::common::examples::{load_max}()
{I});

{I}const std::filesystem::path log_path(
{II}DetermineLogDir()
{III}/ "{cls_name}.{method_name}.max.log"
{I});

{I}{indent_but_first_line(serialization_snippet, I)}

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}serialized,
{II}log_path
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

    target_pth = repo_root / "test/test_x_or_default.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
