/**
 * Test the virtual machine for matching regular expressions.
 */

#include "aas_core/aas_3_0/common.hpp"
#include "aas_core/aas_3_0/revm.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace aas = aas_core::aas_3_0;

TEST_CASE("Test character in an empty range") {
  std::vector<aas::revm::Range> ranges;

  REQUIRE(
    !aas::revm::CharacterInRanges(ranges, L'M')
  );
}

TEST_CASE("Test character in a single range") {
  std::vector<aas::revm::Range> ranges = {
    aas::revm::Range(L'A', L'Z')
  };

  REQUIRE(CharacterInRanges(ranges, L'M'));
  REQUIRE(!CharacterInRanges(ranges, L'a'));
}

TEST_CASE("Test character in multiple ranges") {
  std::vector<aas::revm::Range> ranges = {
    aas::revm::Range(L'A', L'C'),
    aas::revm::Range(L'D', L'F'),
    aas::revm::Range(L'G', L'I'),
    aas::revm::Range(L'J', L'L'),
    aas::revm::Range(L'M', L'N')
  };

  for (wchar_t character = L'A'; character < L'Z'; ++character) {
    const std::string message(
      aas::common::WstringToUtf8(
        L"Testing for character: " + std::wstring(1, character)
      )
    );

    if (character <= L'N') {
      INFO(message)
      REQUIRE(
        CharacterInRanges(ranges, character)
      );
    } else {
      INFO(message)
      REQUIRE(
        !CharacterInRanges(ranges, character)
      );
    }
  }
}

TEST_CASE("Test pattern ^a$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^a+b+$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionSplit>(0, 2)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'b')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionSplit>(2, 4)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(!aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(aas::revm::Match(program, L"ab"));
  REQUIRE(aas::revm::Match(program, L"aabb"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^a|b$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionSplit>(1, 3)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionJump>(4)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'b')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^a?$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionSplit>(1, 2)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^a*$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionSplit>(1, 3)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionJump>(0)
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^[a-b]$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionSet>(
      std::vector<aas::revm::Range>{
        aas::revm::Range(L'a', L'b')
      }
    )
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern ^[^a-b]$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionNotSet>(
      std::vector<aas::revm::Range>{
        aas::revm::Range(L'a', L'b')
      }
    )
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionEnd>()
  );
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
  );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(!aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
  REQUIRE(aas::revm::Match(program, L"c"));
  REQUIRE(!aas::revm::Match(program, L"cc"));
}

TEST_CASE("Test pattern ^.$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(std::make_unique<aas::revm::InstructionAny>());
  program.emplace_back(std::make_unique<aas::revm::InstructionEnd>());
  program.emplace_back(std::make_unique<aas::revm::InstructionMatch>());

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(!aas::revm::Match(program, L"aa"));
  REQUIRE(aas::revm::Match(program, L"b"));
  REQUIRE(!aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}

TEST_CASE("Test pattern with arbitrary suffix ^a.*$") {
  std::vector<std::unique_ptr<aas::revm::Instruction> > program;

  program.emplace_back(
    std::make_unique<aas::revm::InstructionChar>(L'a')
      );
  
  // NOTE (mristin):
  // Usually, the last two instructions would be `end` followed by a `match`.
  // However, we can optimize by having a single `match` without an `end` so that
  // we return early.
  program.emplace_back(
    std::make_unique<aas::revm::InstructionMatch>()
      );

  REQUIRE(!aas::revm::Match(program, L""));
  REQUIRE(aas::revm::Match(program, L"a"));
  REQUIRE(aas::revm::Match(program, L"aa"));
  REQUIRE(!aas::revm::Match(program, L"b"));
  REQUIRE(aas::revm::Match(program, L"ab"));
  REQUIRE(!aas::revm::Match(program, L"ba"));
}
