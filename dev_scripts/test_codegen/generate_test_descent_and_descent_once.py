"""Generate the code to test the ``Descent`` and ``DescentOnce`` iterations."""

import io
import os
import pathlib
import sys
from typing import List

import aas_core_codegen.naming
from aas_core_codegen.common import Stripped, Identifier
from aas_core_codegen.cpp import (
    naming as cpp_naming,
    common as cpp_common,
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
#include "./common.hpp"
#include "./common_examples.generated.hpp"
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/iteration.hpp>
#include <aas_core/aas_3_0/stringification.hpp>
#include <aas_core/aas_3_0/types.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <deque>

namespace aas = aas_core::aas_3_0;"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineXmlDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "Xml";
{I}}}

{I}return *result;
}}"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineDescentDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "Descent";
{I}}}

{I}return *result;
}}"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineDescentOnceDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "DescentOnce";
{I}}}

{I}return *result;
}}"""
        ),
        Stripped(
            f"""\
void AssertOrRerecordDescent(
{I}const std::filesystem::path& xml_path,
{I}const std::filesystem::path& trace_path
) {{
{I}std::shared_ptr<
{II}aas::types::IClass
{I}> instance(
{II}test::common::xmlization::MustDeserializeFile(xml_path)
{I});

{I}std::deque<std::wstring> parts;

{I}for (
{II}const std::shared_ptr<aas::types::IClass>& something
{II}: aas::iteration::Descent(instance)
{I}) {{
{II}parts.emplace_back(test::common::TraceMark(*something));
{II}parts.push_back(L"\\n");
{I}}}

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}aas::common::WstringToUtf8(
{III}test::common::JoinWstrings(parts, L"")
{II}),
{II}trace_path
{I});
}}"""
        ),
        Stripped(
            f"""\
void AssertOrRerecordDescentOnce(
{I}const std::shared_ptr<aas::types::IClass>& instance,
{I}const std::filesystem::path& trace_path
) {{
{I}std::deque<std::wstring> parts;

{I}for (
{II}const std::shared_ptr<aas::types::IClass>& something
{II}: aas::iteration::DescentOnce(instance)
{I}) {{
{II}parts.emplace_back(test::common::TraceMark(*something));
{II}parts.push_back(L"\\n");
{I}}}

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}aas::common::WstringToUtf8(
{III}test::common::JoinWstrings(parts, L"")
{II}),
{II}trace_path
{I});
}}"""
        ),
    ]  # type: List[Stripped]

    environment_cls = symbol_table.must_find_concrete_class(Identifier("Environment"))

    for cls in symbol_table.concrete_classes:
        # fmt: off
        container_cls = (
            test_codegen.test_data_io.determine_container_class(
                cls=cls,
                test_data_dir=repo_root / "test_data",
                environment_cls=environment_cls
            )
        )
        # fmt: on

        xml_class_name = aas_core_codegen.naming.xml_class_name(cls.name)

        # NOTE (mristin):
        # So far, we do not really have a convention how to generate the contained-in
        # directory name, so we assume that we use camel case with a model type.
        container_model_type = aas_core_codegen.naming.json_model_type(
            container_cls.name
        )
        contained_in_dir_name = (
            "SelfContained"
            if container_cls is cls
            else f"ContainedIn{container_model_type}"
        )

        cls_name = cpp_naming.class_name(cls.name)

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test Descent over an {cls_name}") {{
{I}const std::deque<std::filesystem::path> paths(
{II}test::common::FindFilesBySuffixRecursively(
{III}DetermineXmlDir()
{IIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIII}/ "Expected"
{IIII}/ {cpp_common.string_literal(xml_class_name)},
{III}".xml"
{II})
{I});

{I}for (const std::filesystem::path& path : paths) {{
{II}const std::filesystem::path parent(
{III}(
{IIII}DetermineDescentDir()
{IIIII}/ std::filesystem::relative(path, DetermineXmlDir())
{III}).parent_path()
{II});

{II}const std::filesystem::path trace_path(
{III}parent
{IIII}/ (path.filename().string() + ".trace")
{II});

{II}AssertOrRerecordDescent(path, trace_path);
{I}}}
}}"""
            )
        )

        interface_name = cpp_naming.interface_name(cls.name)
        load_max = cpp_naming.function_name(Identifier(f"load_max_{cls.name}"))

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test DescentOnce over an {cls_name}") {{
{I}const std::shared_ptr<
{II}aas::types::{interface_name}
{I}> instance(
{II}test::common::examples::{load_max}()
{I});

{I}const std::filesystem::path trace_path(
{II}DetermineDescentOnceDir()
{III}/ "Max{cls_name}.trace"
{I});

{I}AssertOrRerecordDescentOnce(instance, trace_path);
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

    target_pth = repo_root / "test/test_descent_and_descent_once.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
