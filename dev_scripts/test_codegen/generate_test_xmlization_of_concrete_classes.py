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
#include "./common_xmlization.hpp"

#include <aas_core/aas_3_0/xmlization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;"""
        ),
        Stripped(
            f"""\
void AssertRoundTrip(
{I}const std::filesystem::path& path
) {{
{I}std::shared_ptr<
{II}aas::types::IClass
{I}> deserialized(
{II}test::common::xmlization::MustDeserializeFile(path)
{I});

{I}std::stringstream ss;
{I}aas::xmlization::Serialize(*deserialized, {{}}, ss);

{I}std::string expected_xml = test::common::MustReadString(path);

{I}INFO(aas::common::Concat("XML round-trip on ", path.string()))
{I}REQUIRE(
{II}test::common::xmlization::CanonicalizeXml(expected_xml)
{III}== test::common::xmlization::CanonicalizeXml(ss.str())
{I});
}}"""
        ),
        Stripped(
            f"""\
void AssertDeserializationFailure(
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
const std::filesystem::path& DetermineErrorDir() {{
{I}static aas::common::optional<std::filesystem::path> result;
{I}if (!result.has_value()) {{
{II}result = test::common::DetermineTestDataDir() / "XmlizationError";
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
TEST_CASE("Test the round-trip of an expected {cls_name}") {{
{I}const std::deque<std::filesystem::path> paths(
{II}test::common::FindFilesBySuffixRecursively(
{III}DetermineXmlDir()
{IIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIII}/ "Expected"
{IIII}/ {cpp_common.string_literal(xml_class_name)},
{III}".xml"
{II})
{I});

{I}for (const std::filesystem::path &path : paths) {{
{II}AssertRoundTrip(path);
{I}}}
}}"""
            )
        )

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test the de-serialization failure on an unexpected {cls_name}") {{
{I}for (
{II}const std::string &cause
{II}: test::common::xmlization::kCausesForDeserializationFailure
{I}) {{
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

{II}for (const std::filesystem::path &path : paths) {{
{III}const std::filesystem::path parent(
{IIII}(
{IIIII}DetermineErrorDir()
{IIIII}/ std::filesystem::relative(path, DetermineXmlDir())
{IIII}).parent_path()
{III});

{III}const std::filesystem::path error_path(
{IIII}parent
{IIII}/ (path.filename().string() + ".error")
{III});

{III}AssertDeserializationFailure(
{IIII}path,
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

    target_pth = repo_root / "test/test_xmlization_of_concrete_classes.generated.cpp"
    target_pth.write_text(writer.getvalue(), encoding="utf-8")

    return 0


if __name__ == "__main__":
    sys.exit(main())
