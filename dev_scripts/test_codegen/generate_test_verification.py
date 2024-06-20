"""Generate the code to test the verification of both the positives and negatives."""

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
    INDENT6 as IIIIII,
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
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/verification.hpp>
#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;"""
        ),
        Stripped(
            f"""\
void AssertNoVerificationError(
{I}const std::filesystem::path& xml_path
) {{
{I}std::shared_ptr<
{II}aas::types::IClass
{I}> instance(
{II}test::common::xmlization::MustDeserializeFile(xml_path)
{I});

{I}std::vector<std::string> error_messages;
{I}for (
{II}const aas::verification::Error &error
{II}: aas::verification::RecursiveVerification(instance)
{I}) {{
{II}error_messages.emplace_back(
{III}aas::common::Concat(
{IIII}aas::common::WstringToUtf8(error.path.ToWstring()),
{IIII}": ",
{IIII}aas::common::WstringToUtf8(error.cause)
{III})
{II});
{I}}}

{I}if (!error_messages.empty()) {{
{II}std::vector<std::string> parts;
{II}parts.emplace_back("Expected no error messages from ");
{II}parts.emplace_back(xml_path.string());
{II}parts.emplace_back(", but got:\\n");
{II}parts.emplace_back(test::common::JoinStrings(error_messages, "\\n"));

{II}INFO(test::common::JoinStrings(parts, ""))
{II}CHECK(error_messages.empty());
{I}}}
}}"""
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
const std::vector<std::string> kCausesForVerificationFailure = {{
{I}"ConstraintViolation",
{I}"InvalidMinMaxExample",
{I}"InvalidValueExample",
{I}"MaxLengthViolation",
{I}"MinLengthViolation",
{I}"PatternViolation",
{I}"SetViolation"  
}};"""
        ),
        Stripped(
            f"""\
const std::filesystem::path& DetermineErrorDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "VerificationError";
{I}}}

{I}return *result;
}}"""
        ),
        Stripped(
            f"""\
void AssertVerificationFailure(
{I}const std::filesystem::path& path,
{I}const std::filesystem::path& error_path
) {{
{I}std::ifstream ifs(path, std::ios::binary);

{I}aas::common::expected<
{II}std::shared_ptr<aas::types::IClass>,
{II}aas::xmlization::DeserializationError
{I}> deserialized = aas::xmlization::From(
{II}ifs
{I});

{I}if (!deserialized.has_value()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Expected to de-serialize ",
{IIII}path.string(),
{IIII}", but the de-serialization failed: ",
{IIII}aas::common::WstringToUtf8(deserialized.error().path.ToWstring()),
{IIII}aas::common::WstringToUtf8(deserialized.error().cause)
{III})
{II})
{II}REQUIRE(!deserialized.has_value());
{I}}}

{I}std::vector<std::string> error_messages;
{I}for (
{II}const aas::verification::Error& error
{II}: aas::verification::RecursiveVerification(deserialized.value())
{II}) {{
{II}error_messages.emplace_back(
{III}aas::common::Concat(
{IIII}aas::common::WstringToUtf8(error.path.ToWstring()),
{IIII}": ",
{IIII}aas::common::WstringToUtf8(error.cause)
{III})
{II});
{I}}}

{I}if (error_messages.empty()) {{
{II}INFO(
{III}aas::common::Concat(
{IIII}"Expected error messages from ",
{IIII}path.string(),
{IIII}", but got none"
{III})
{II})
{II}REQUIRE(!error_messages.empty());
{I}}}

{I}const std::string joined_error_messages = test::common::JoinStrings(
{II}error_messages,
{II}"\\n"
{I});

{I}test::common::AssertContentEqualsExpectedOrRecord(
{II}joined_error_messages,
{II}error_path
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
TEST_CASE("Test verification of a valid {cls_name}") {{
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
{II}AssertNoVerificationError(path);
{I}}}
}}"""
            )
        )

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test verification of invalid cases for {cls_name}") {{
{I}for (const std::string& cause : kCausesForVerificationFailure) {{
{II}const std::deque<std::filesystem::path> paths(
{III}test::common::FindFilesBySuffixRecursively(
{IIII}DetermineXmlDir()
{IIIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIIII}/ "Unexpected"
{IIIII}/ cause
{IIIII}/ {cpp_common.string_literal(xml_class_name)},
{IIII}".xml"
{III})
{II});

{II}for (const std::filesystem::path& path : paths) {{
{III}const std::filesystem::path parent(
{IIII}(
{IIIII}DetermineErrorDir()
{IIIIII}/ std::filesystem::relative(path, DetermineXmlDir())
{IIII}).parent_path()
{III});

{III}const std::filesystem::path error_path(
{IIII}parent
{IIIII}/ (path.filename().string() + ".errors")
{III});

{III}AssertVerificationFailure(path, error_path);
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

    target_pth = repo_root / "test/test_verification.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
