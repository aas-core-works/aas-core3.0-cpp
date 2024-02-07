"""Generate the code to test the jsonization of concrete classes."""

import io
import os
import pathlib
import sys
from typing import List

import aas_core_codegen.naming
from aas_core_codegen import intermediate
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


# TODO (mristin, 2024-02-10): implement
# def _generate_for_self_contained(cls: intermediate.ConcreteClass) -> List[Stripped]:
#     """Generate the tests for a self-contained class."""
#     model_type_literal = golang_common.string_literal(
#         aas_core_codegen.naming.json_model_type(cls.name)
#     )
#
#     deserialization_function = golang_naming.function_name(
#         Identifier(f"{cls.name}_from_jsonable")
#     )
#
#     blocks = []  # type: List[Stripped]
#
#     test_name = golang_naming.function_name(
#         Identifier(f"Test_{cls.name}_round_trip_OK")
#     )
#
#     blocks.append(
#         Stripped(
#             f"""\
# func {test_name}(t *testing.T) {{
# {I}pths := aastesting.FindFilesBySuffixRecursively(
# {II}filepath.Join(
# {III}aastesting.TestDataDir,
# {III}"Json",
# {III}"SelfContained",
# {III}"Expected",
# {III}{model_type_literal},
# {II}),
# {II}".json",
# {I})
# {I}sort.Strings(pths)
#
# {I}for _, pth := range pths {{
# {II}jsonable := aastesting.MustReadJsonable(
# {III}pth,
# {II})
#
# {II}deserialized, deseriaErr := aasjsonization.{deserialization_function}(
# {III}jsonable,
# {II})
# {II}ok := assertNoDeserializationError(t, deseriaErr, pth)
# {II}if !ok {{
# {III}return
# {II}}}
#
# {II}anotherJsonable, seriaErr := aasjsonization.ToJsonable(deserialized)
# {II}ok = assertNoSerializationError(t, seriaErr, pth)
# {II}if !ok {{
# {III}return
# {II}}}
#
# {II}ok = assertSerializationEqualsDeserialization(
# {III}t,
# {III}jsonable,
# {III}anotherJsonable,
# {III}pth,
# {II})
# {II}if !ok {{
# {III}return
# {II}}}
# {I}}}
# }}"""
#         )
#     )
#
#     test_name = golang_naming.function_name(
#         Identifier(f"Test_{cls.name}_deserialization_fail")
#     )
#
#     blocks.append(
#         Stripped(
#             f"""\
# func {test_name}(t *testing.T) {{
# {I}for _, cause := range causesForDeserializationFailure {{
# {II}pths := aastesting.FindFilesBySuffixRecursively(
# {III}filepath.Join(
# {IIII}aastesting.TestDataDir,
# {IIII}"Json",
# {IIII}"SelfContained",
# {IIII}"Unexpected",
# {IIII}cause,
# {III}{model_type_literal},
# {III}),
# {III}".json",
# {II})
# {II}sort.Strings(pths)
#
# {II}for _, pth := range pths {{
# {III}jsonable := aastesting.MustReadJsonable(
# {IIII}pth,
# {III})
#
# {III}relPth, err := filepath.Rel(aastesting.TestDataDir, pth)
# {III}if err != nil {{
# {IIII}panic(
# {IIIII}fmt.Sprintf(
# {IIIIII}"Failed to compute the relative path of %s to %s: %s",
# {IIIIII}aastesting.TestDataDir, pth, err.Error(),
# {IIIII}),
# {IIII})
# {III}}}
#
# {III}expectedPth := filepath.Join(
# {IIII}aastesting.TestDataDir,
# {IIII}"DeserializationError",
# {IIII}filepath.Dir(relPth),
# {IIII}filepath.Base(relPth)+".error",
# {III})
#
# {III}_, deseriaErr := aasjsonization.{deserialization_function}(
# {IIII}jsonable,
# {III})
# {III}ok := assertDeserializationErrorEqualsExpectedOrRecord(
# {IIII}t, deseriaErr, pth, expectedPth,
# {III})
# {III}if !ok {{
# {IIII}return
# {III}}}
# {II}}}
# {I}}}
# }}"""
#         )
#     )
#
#     return blocks


# TODO (mristin, 2024-02-10): delete or implement
# def _generate_for_contained_in_environment(
#     cls: intermediate.ConcreteClass,
#     container_cls: intermediate.ConcreteClass,
# ) -> List[Stripped]:
#     """Generate the tests for a class contained in a container class."""
#     model_type_literal = golang_common.string_literal(
#         aas_core_codegen.naming.json_model_type(cls.name)
#     )
#
#     deserialization_function = golang_naming.function_name(
#         Identifier(f"{container_cls.name}_from_jsonable")
#     )
#
#     contained_in_literal = golang_common.string_literal(
#         f"ContainedIn{container_cls.name}"
#     )
#
#     blocks = []  # type: List[Stripped]
#
#     test_name = golang_naming.function_name(
#         Identifier(f"Test_{cls.name}_round_trip_OK")
#     )
#
#     blocks.append(
#         Stripped(
#             f"""\
# func {test_name}(t *testing.T) {{
# {I}pths := aastesting.FindFilesBySuffixRecursively(
# {II}filepath.Join(
# {III}aastesting.TestDataDir,
# {III}"Json",
# {III}{contained_in_literal},
# {III}"Expected",
# {III}{model_type_literal},
# {II}),
# {II}".json",
# {I})
# {I}sort.Strings(pths)
#
# {I}for _, pth := range pths {{
# {II}jsonable := aastesting.MustReadJsonable(
# {III}pth,
# {II})
#
# {II}deserialized, deseriaErr := aasjsonization.{deserialization_function}(
# {III}jsonable,
# {II})
# {II}ok := assertNoDeserializationError(t, deseriaErr, pth)
# {II}if !ok {{
# {III}return
# {II}}}
#
# {II}anotherJsonable, seriaErr := aasjsonization.ToJsonable(deserialized)
# {II}ok = assertNoSerializationError(t, seriaErr, pth)
# {II}if !ok {{
# {III}return
# {II}}}
#
# {II}ok = assertSerializationEqualsDeserialization(
# {III}t,
# {III}jsonable,
# {III}anotherJsonable,
# {III}pth,
# {II})
# {II}if !ok {{
# {III}return
# {II}}}
# {I}}}
# }}"""
#         )
#     )
#
#     test_name = golang_naming.function_name(
#         Identifier(f"Test_{cls.name}_deserialization_fail")
#     )
#
#     blocks.append(
#         Stripped(
#             f"""\
# func {test_name}(t *testing.T) {{
# {I}for _, cause := range causesForDeserializationFailure {{
# {II}pths := aastesting.FindFilesBySuffixRecursively(
# {III}filepath.Join(
# {IIII}aastesting.TestDataDir,
# {IIII}"Json",
# {IIII}{contained_in_literal},
# {IIII}"Unexpected",
# {IIII}cause,
# {IIII}{model_type_literal},
# {III}),
# {III}".json",
# {II})
# {II}sort.Strings(pths)
#
# {II}for _, pth := range pths {{
# {III}jsonable := aastesting.MustReadJsonable(
# {IIII}pth,
# {III})
#
# {III}relPth, err := filepath.Rel(aastesting.TestDataDir, pth)
# {III}if err != nil {{
# {IIII}panic(
# {IIIII}fmt.Sprintf(
# {IIIIII}"Failed to compute the relative path of %s to %s: %s",
# {IIIIII}aastesting.TestDataDir, pth, err.Error(),
# {IIIII}),
# {IIII})
# {III}}}
#
# {III}expectedPth := filepath.Join(
# {IIII}aastesting.TestDataDir,
# {IIII}"DeserializationError",
# {IIII}filepath.Dir(relPth),
# {IIII}filepath.Base(relPth) + ".error",
# {III})
#
# {III}_, deseriaErr := aasjsonization.{deserialization_function}(
# {IIII}jsonable,
# {III})
# {III}ok := assertDeserializationErrorEqualsExpectedOrRecord(
# {IIII}t, deseriaErr, pth, expectedPth,
# {III})
# {III}if !ok {{
# {IIII}return
# {III}}}
# {II}}}
# {I}}}
# }}"""
#         )
#     )
#
#     return blocks


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

#include <aas_core/aas_3_0/jsonization.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;'''
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
{I}const nlohmann::json json = test::common::MustReadJson(path);

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
{I}> deserialization_function
) {{
{I}const nlohmann::json json = test::common::MustReadJson(path);

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

{I}const std::filesystem::path error_path(
{II}path.parent_path()
{III}/ (path.filename().string() + ".error")
{I});

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
            "SelfContained" if container_cls is cls
            else f"ContainedIn{container_model_type}"
        )

        cls_name = cpp_naming.class_name(cls.name)

        blocks.append(
            Stripped(
                f"""\
TEST_CASE("Test the round-trip of an expected {cls_name}") {{
{I}const std::deque<std::filesystem::path> paths(
{II}test::common::FindFilesBySuffixRecursively(
{III}test::common::DetermineTestDataDir()
{IIII}/ "Json"
{IIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIII}/ "Expected"
{IIII}/ {cpp_common.string_literal(model_type)},
{III}".json"
{II})
{I});

{I}for (const std::filesystem::path& path : paths
{II}) {{
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
{II}) {{
{II}const std::deque<std::filesystem::path> paths(
{III}test::common::FindFilesBySuffixRecursively(
{IIII}test::common::DetermineTestDataDir()
{IIIII}/ "Json"
{IIIII}/ {cpp_common.string_literal(contained_in_dir_name)}
{IIIII}/ "Unexpected"
{IIIII}/ cause
{IIIII}/ {cpp_common.string_literal(model_type)},
{IIII}".json"
{III})
{II});

{II}for (const std::filesystem::path& path : paths
{III}) {{
{III}AssertDeserializationFailure<
{IIII}aas::types::{container_interface_name}
{III}>(path, aas::jsonization::{deserialization_function});
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
