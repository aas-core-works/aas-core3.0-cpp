"""Generate the code to test the wstringification of enums."""

import io
import os
import pathlib
import sys
from typing import List

import aas_core_codegen.naming
from aas_core_codegen import intermediate
from aas_core_codegen.common import Stripped, Identifier, indent_but_first_line
from aas_core_codegen.cpp import (
    common as cpp_common,
    naming as cpp_naming,
)
from aas_core_codegen.cpp.common import (
    INDENT as I,
    INDENT2 as II,
    INDENT3 as III,
)

import test_codegen.common
import test_codegen.test_data_io


def _generate_test_round_trip_for_model_type(
        symbol_table: intermediate.SymbolTable
) -> Stripped:
    """
    Generate the test case for valid de/wstringification of model type enum.

    This enum does not explicitly exist in the meta-model, so we have to generate
    the test for it separately.
    """
    enum_name = cpp_naming.enum_name(Identifier("Model_type"))

    stmts = []  # type: List[Stripped]

    must_from_wstring = cpp_naming.function_name(
        Identifier(f"must_model_type_from_wstring")
    )

    for cls in symbol_table.concrete_classes:
        literal_name = cpp_naming.enum_literal_name(cls.name)
        literal_value = aas_core_codegen.naming.json_model_type(cls.name)

        stmts.append(
            Stripped(
                f"""\
REQUIRE(
{I}aas::types::{enum_name}::{literal_name}
{I}== aas::wstringification::{must_from_wstring}(
{II}{cpp_common.wstring_literal(literal_value)}
{I})
);"""
            )
        )

        stmts.append(
            Stripped(
                f"""\
REQUIRE(
{I}aas::wstringification::to_wstring(
{II}aas::types::{enum_name}::{literal_name}
{I})
{I}== {cpp_common.wstring_literal(literal_value)}
);"""
            )
        )

    body = "\n\n".join(stmts)

    return Stripped(
        f"""\
TEST_CASE("Test {enum_name} round-trip") {{
{I}{indent_but_first_line(body, I)}
}}"""
    )


def _generate_test_failure_for_model_type(
        symbol_table: intermediate.SymbolTable
) -> Stripped:
    """
    Generate the test case for invalid de-wstringification of model type enum.

    This enum does not explicitly exist in the meta-model, so we have to generate
    the test for it separately.
    """
    enum_name = cpp_naming.enum_name(Identifier("Model_type"))

    from_wstring = cpp_naming.function_name(
        Identifier(f"model_type_from_wstring")
    )

    must_from_wstring = cpp_naming.function_name(
        Identifier(f"must_model_type_from_wstring")
    )

    return Stripped(
        f"""\
TEST_CASE("Test failure on {enum_name}") {{
{I}CHECK(
{II}!aas::wstringification::{from_wstring}(
{III}L"Totally utterly invalid"
{II}).has_value()
{I});

{I}REQUIRE_THROWS_WITH(
{II}aas::wstringification::{must_from_wstring}(
{III}L"Totally utterly invalid"
{II}),
{II}"Unexpected {enum_name} literal: Totally utterly invalid"
{I});
}}"""
    )


def _generate_test_round_trip_for(enum: intermediate.Enumeration) -> Stripped:
    """Generate the test of the valid de/serialization for an enum."""
    enum_name = cpp_naming.enum_name(enum.name)

    stmts = []  # type: List[Stripped]

    must_from_wstring = cpp_naming.function_name(
        Identifier(f"must_{enum.name}_from_wstring")
    )

    for literal in enum.literals:
        literal_name = cpp_naming.enum_literal_name(literal.name)

        stmts.append(
            Stripped(
                f"""\
REQUIRE(
{I}aas::types::{enum_name}::{literal_name}
{I}== aas::wstringification::{must_from_wstring}(
{II}{cpp_common.wstring_literal(literal.value)}
{I})
);"""
            )
        )

        stmts.append(
            Stripped(
                f"""\
REQUIRE(
{I}aas::wstringification::to_wstring(
{II}aas::types::{enum_name}::{literal_name}
{I})
{I}== {cpp_common.wstring_literal(literal.value)}
);"""
            )
        )

    body = "\n\n".join(stmts)

    return Stripped(
        f"""\
TEST_CASE("Test {enum_name} round-trip") {{
{I}{indent_but_first_line(body, I)}
}}"""
    )


def _generate_test_failure_for(enum: intermediate.Enumeration) -> Stripped:
    """Generate the de-wstringification of an invalid literal."""
    enum_name = cpp_naming.enum_name(enum.name)

    from_wstring = cpp_naming.function_name(
        Identifier(f"{enum.name}_from_wstring")
    )

    must_from_wstring = cpp_naming.function_name(
        Identifier(f"must_{enum.name}_from_wstring")
    )

    return Stripped(
        f"""\
TEST_CASE("Test failure on {enum_name}") {{
{I}CHECK(
{II}!aas::wstringification::{from_wstring}(
{III}L"Totally utterly invalid"
{II}).has_value()
{I});

{I}REQUIRE_THROWS_WITH(
{II}aas::wstringification::{must_from_wstring}(
{III}L"Totally utterly invalid"
{II}),
{II}"Unexpected {enum_name} literal: Totally utterly invalid"
{I});
}}"""
    )


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
#include "aas_core/aas_3_0/wstringification.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;'''
        ),
        _generate_test_round_trip_for_model_type(symbol_table=symbol_table),
        _generate_test_failure_for_model_type(symbol_table=symbol_table)
    ]  # type: List[Stripped]

    for enum in symbol_table.enumerations:
        if len(enum.literals) == 0:
            continue

        blocks.append(_generate_test_round_trip_for(enum=enum))

        blocks.append(_generate_test_failure_for(enum=enum))

    blocks.append(warning)

    writer = io.StringIO()
    for i, block in enumerate(blocks):
        if i > 0:
            writer.write("\n\n")

        writer.write(block)

    writer.write("\n")

    target_pth = repo_root / "test/test_wstringification_of_enums.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
