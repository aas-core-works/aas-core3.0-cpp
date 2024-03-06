"""Generate the code to test the jsonization of concrete classes."""

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
#include "./common_jsonization.hpp"

#include <aas_core/aas_3_0/jsonization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;"""
        ),
        Stripped(
            f"""\
template<class ClassT>
void AssertRoundTrip(
{I}const std::filesystem::path& path,
{I}std::function<
{II}aas::common::expected<
{III}std::shared_ptr<ClassT>,
{III}aas::jsonization::DeserializationError
{II}>(const nlohmann::json&, bool)
{I}> deserialization_function
) {{
{I}const nlohmann::json json = test::common::jsonization::MustReadJson(path);

{I}aas::common::expected<
{II}std::shared_ptr<ClassT>,
{II}aas::jsonization::DeserializationError
{I}> deserialized = deserialization_function(json, false);

{I}if (!deserialized.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Failed to de-serialize from ",
{IIII}path.string(),
{IIII}": ",
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

{I}nlohmann::json another_json = aas::jsonization::Serialize(
{II}*(deserialized.value())
{I});

{I}std::optional<std::string> diff_message = test::common::jsonization::CompareJsons(
{II}json,
{II}another_json
{I});
{I}if (diff_message.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"The JSON round-trip from ",
{IIII}path.string(),
{IIII}" failed. There is a diff between the original JSON "
{IIII}"and the serialized one: ",
{IIII}*diff_message
{III})
{II})
{II}REQUIRE(!diff_message.has_value());
{I}}}
}}"""
        ),
        Stripped(
            f"""\
template<typename ClassT>
void AssertDeserializationFailure(
{I}const std::filesystem::path& path,
{I}std::function<
{II}aas::common::expected<
{III}std::shared_ptr<ClassT>,
{III}aas::jsonization::DeserializationError
{II}>(const nlohmann::json&, bool)
{I}> deserialization_function,
{I}const std::filesystem::path& error_path
) {{
{I}const nlohmann::json json = test::common::jsonization::MustReadJson(path);

{I}aas::common::expected<
{II}std::shared_ptr<ClassT>,
{II}aas::jsonization::DeserializationError
{I}> deserialized = deserialization_function(json, false);

{I}if (deserialized.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Expected the de-serialization to fail on ",
{IIII}path.string(),
{IIII}", but the de-serialization succeeded"
{III})
{II})
{II}REQUIRE(!deserialized.has_value());
{I}}}

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}aas::common::Concat(
{III}aas::common::WstringToUtf8(
{IIII}deserialized.error().path.ToWstring()
{III}),
{III}": ",
{III}aas::common::WstringToUtf8(
{IIII}deserialized.error().cause
{III})
{II}),
{II}error_path
{I});
}}"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineJsonDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "Json";
{I}}}

{I}return *result;
}}"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineErrorDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "JsonizationError";
{I}}}

{I}return *result;
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

        container_interface_name = cpp_naming.interface_name(container_cls.name)
        deserialization_function = cpp_naming.function_name(
            Identifier(f"{container_cls.name}_from")
        )

        model_type = aas_core_codegen.naming.json_model_type(cls.name)
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
TEST_CASE("Test the round-trip of an expected {cls_name}") {{
{I}const std::deque<std::filesystem::path> paths(
{II}test::common::FindFilesBySuffixRecursively(
{III}DetermineJsonDir()
{IIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIII}/ "Expected"
{IIII}/ {cpp_common.string_literal(model_type)},
{III}".json"
{II})
{I});

{I}for (const std::filesystem::path& path : paths) {{
{II}AssertRoundTrip<
{III}aas::types::{container_interface_name}
{II}>(path, aas::jsonization::{deserialization_function});
{I}}}
}}"""
            )
        )

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test the de-serialization failure on an unexpected {cls_name}") {{
{I}for (
{II}const std::string& cause
{II}: test::common::jsonization::kCausesForDeserializationFailure
{I}) {{
{II}const std::deque<std::filesystem::path> paths(
{III}test::common::FindFilesBySuffixRecursively(
{IIII}DetermineJsonDir()
{IIIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIIII}/ "Unexpected"
{IIIII}/ cause
{IIIII}/ {cpp_common.string_literal(model_type)},
{IIII}".json"
{III})
{II});

{II}for (const std::filesystem::path& path : paths) {{
{III}const std::filesystem::path parent(
{IIII}(
{IIIII}DetermineErrorDir()
{IIIII}/ std::filesystem::relative(path, DetermineJsonDir())
{IIII}).parent_path()
{III});

{III}const std::filesystem::path error_path(
{IIII}parent
{IIII}/ (path.filename().string() + ".error")
{III});

{III}AssertDeserializationFailure<
{IIII}aas::types::{container_interface_name}
{III}>(
{IIII}path,
{IIII}aas::jsonization::{deserialization_function},
{IIII}error_path
{III});
{II}}}
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

    target_pth = repo_root / "test/test_jsonization_of_concrete_classes.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
