/**
 * We provide here a playground implementation of:
 * https://swtch.com/~rsc/regexp/regexp2.html
 *
 * The additional instructions are inspired from:
 * https://www.codeproject.com/Articles/5256833/Regex-as-a-Tiny-Threaded-Virtual-Machine
 *
 * We leave this code checked into the repository so that the future developers can
 * get familiar with the technique and play with it. The final implementation moves
 * to the code base and relies on internal project dependencies (such as
 * `aas::common::Concat`), so it is less amenable for a playground.
 */

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

enum class InstructionKind : std::uint8_t {
  Char,
  Set,
  NotSet,
  Any,
  Match,
  Jump,
  Split,
  End
};

struct Instruction {
  virtual InstructionKind kind() const = 0;
  virtual ~Instruction() = default;
};

std::string RepresentWCharacter(wchar_t character) {
  switch (character) {
    case L'\\':return "\\\\";
    case L'"':return "\\\"";
    case L'\'':return "\\'";
    case L'\t':return "\\t";
    case L'\n':return "\\n";
    case L'\r':return "\\r";
    default:break;
  }

  if (26 <= character && character <= 126) {
    return std::string(1, static_cast<char>(character));
  }

  static const char* digits = "0123456789ABCDEF";
  size_t digit_count = sizeof(wchar_t) * 2;

  std::string result;
  result.resize(digit_count + 2);
  result[0] = L'\\';
  result[1] = L'u';

  for (size_t i = 0, j = (digit_count - 1) * 4; i < digit_count; ++i, j -= 4) {
    const size_t digit_i = (character >> j) & 0x0f;
    result[i + 2] = digits[digit_i];
  }

  return result;
}

std::string RepresentWString(const std::wstring& text) {
  std::stringstream ss;
  for (const wchar_t character : text) {
    ss << RepresentWCharacter(character);
  }
  return ss.str();
}

/**
 * If the character on the String Pointer does not match the `character`, stop this
 * thread as it failed. Otherwise, move the String Pointer to the next character,
 * and the Program Counter to the next instruction.
 */
struct InstructionChar : Instruction {
  wchar_t character;

  explicit InstructionChar(wchar_t a_character) :
    character(a_character) {
    // Intentionally empty.
  }

  InstructionKind kind() const override { return InstructionKind::Char; }

  ~InstructionChar() override = default;
};

std::string to_string(const InstructionChar& instruction) {
  return "char '" + RepresentWCharacter(instruction.character) + "'";
}

/**
 * Define a character range.
 */
struct Range {
  wchar_t first;
  wchar_t last;

  Range(wchar_t a_first, wchar_t a_last) :
    first(a_first),
    last(a_last) {
    #ifdef DEBUG
    if (a_first > a_last) {
      throw std::invalid_argument(
        "First character is larger than the last character in a character range"
        );
    }
    #endif
  }
};

std::string to_string(const Range& range) {
  if (range.first == range.last) {
    return RepresentWCharacter(range.first);
  }

  return RepresentWCharacter(range.first) + "-" + RepresentWCharacter(range.last);
}

/**
 * If the character on the String Pointer *is not* in the given set, stop this
 * thread as it failed. Otherwise, move the String Pointer to the next character,
 * and the Program Counter to the next instruction.
 */
struct InstructionSet : Instruction {
  std::vector<Range> ranges;

  explicit InstructionSet(std::vector<Range> a_ranges) :
    ranges(std::move(a_ranges)) {
    #ifdef DEBUG
    if (ranges.empty()) {
      throw std::invalid_argument(
        "Unexpected NotSet instruction with empty ranges"
      );
    }

    for (size_t i = 1; i < ranges.size(); ++i) {
      if (ranges[i - 1].last >= ranges[i].first) {
        throw std::invalid_argument(
          "The ranges for an InstructionNotSet are unexpectedly either "
          "not sorted or overlapping"
          );
      }
    }
    #endif
  }

  InstructionKind kind() const override { return InstructionKind::Set; }

  ~InstructionSet() override = default;
};

std::string to_string(const InstructionSet& instruction) {
  std::stringstream ss;
  ss << "set '";

  for (const auto& range : instruction.ranges) {
    ss << to_string(range);
  }

  ss << "'";
  return ss.str();
}

/**
 * If the character on the String Pointer *is* in the given set, stop this
 * thread as it failed. Otherwise, move the String Pointer to the next character,
 * and the Program Counter to the next instruction.
 */
struct InstructionNotSet : Instruction {
  std::vector<Range> ranges;

  explicit InstructionNotSet(std::vector<Range> a_ranges) :
    ranges(std::move(a_ranges)) {
    #ifdef DEBUG
    if (ranges.empty()) {
      throw std::invalid_argument(
        "Unexpected NotSet instruction with empty ranges"
      );
    }

    for (size_t i = 1; i < ranges.size(); ++i) {
      if (ranges[i - 1].last >= ranges[i].first) {
        throw std::invalid_argument(
          "The ranges for an InstructionNotSet are unexpectedly either "
          "not sorted or overlapping"
          );
      }
    }
    #endif
  }

  InstructionKind kind() const override { return InstructionKind::NotSet; }

  ~InstructionNotSet() override = default;
};

std::string to_string(const InstructionNotSet& instruction) {
  std::stringstream ss;
  ss << "not-set '";

  for (const auto& range : instruction.ranges) {
    ss << to_string(range);
  }

  ss << "'";
  return ss.str();
}

/**
 * Match any character.
 */
struct InstructionAny : Instruction {
  InstructionKind kind() const override { return InstructionKind::Any; }

  ~InstructionAny() override = default;
};

std::string to_string(const InstructionAny&) {
  return "any";
}

/**
 * Stop this thread and signal that we found a match.
 */
struct InstructionMatch : Instruction {
  InstructionKind kind() const override { return InstructionKind::Match; }

  ~InstructionMatch() override = default;
};

std::string to_string(const InstructionMatch&) {
  return "match";
}

/**
 * Jump to the indicated position in the program.
 */
struct InstructionJump : Instruction {
  size_t target;

  explicit InstructionJump(size_t a_target) :
    target(a_target) {
    // Intentionally empty.
  }

  InstructionKind kind() const override { return InstructionKind::Jump; }

  ~InstructionJump() override = default;
};

std::string to_string(const InstructionJump& instruction) {
  return "jump " + std::to_string(instruction.target);
}

/**
 * Split the program in two threads, both jumping to different locations. The string
 * pointer is kept as-is.
 */
struct InstructionSplit : Instruction {
  size_t first_target;
  size_t second_target;

  explicit InstructionSplit(size_t a_first_target, size_t a_second_target) :
    first_target(a_first_target),
    second_target(a_second_target) {
    // Intentionally empty.
  }

  InstructionKind kind() const override { return InstructionKind::Split; }

  ~InstructionSplit() override = default;
};

std::string to_string(const InstructionSplit& instruction) {
  return (
    "split "
      + std::to_string(instruction.first_target) + ", "
      + std::to_string(instruction.second_target)
  );
}

/**
 * Match the end-of-input.
 */
struct InstructionEnd : Instruction {
  InstructionKind kind() const override { return InstructionKind::End; }

  ~InstructionEnd() override = default;
};

std::string to_string(const InstructionEnd&) {
  return "end";
}

std::string to_string(const Instruction& instruction) {
  switch (instruction.kind()) {
    case InstructionKind::Char:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionChar&
          >(instruction)
      );

    case InstructionKind::Set:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionSet&
          >(instruction)
      );

    case InstructionKind::NotSet:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionNotSet&
          >(instruction)
      );

    case InstructionKind::Any:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionAny&
          >(instruction)
      );

    case InstructionKind::Match:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionMatch&
          >(instruction)
      );

    case InstructionKind::Jump:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionJump&
          >(instruction)
      );

    case InstructionKind::Split:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionSplit&
          >(instruction)
      );

    case InstructionKind::End:
      return to_string(
        static_cast<   // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          const InstructionEnd&
          >(instruction)
      );

    default:
      throw std::logic_error(
        "Unhandled instruction kind: "
          + std::to_string(
            static_cast<std::uint8_t>(instruction.kind())
          )
      );
  }
}

std::string to_string(
  const std::vector<std::unique_ptr<Instruction> >& instructions
) {
  std::stringstream ss;
  for (size_t i = 0; i < instructions.size(); ++i) {
    ss
      << std::setw(4) << i << ": "
      << to_string(*instructions[i]) << std::endl;
  }

  return ss.str();
}

bool CharacterInRanges(
  const std::vector<Range>& ranges,
  wchar_t character
) {
  if (ranges.empty()) {
    return false;
  }

  if (ranges.size() == 1) {
    return (ranges[0].first <= character && character <= ranges[0].last);
  }

  // Binary search
  size_t begin = 0;
  size_t end = ranges.size();

  while (true) {
    if (begin == end) {
      return false;
    }

    // NOTE (mristin):
    // Most binary-search implementation are buggy. We try to avert bugs by explicitly
    // handling the special case where there are at most 3 elements in the segment,
    // so we stop here instead of proceeding recursively.
    if (end - begin <= 3) {
      for (size_t i = begin; i < end; ++i) {
        const Range& range = ranges[i];
        if (range.first <= character && character <= range.last) {
          return true;
        }
      }
      return false;
    }

    const size_t middle = (begin + end) / 2;
    const Range& range = ranges[middle];
    if (character < range.first) {
      end = middle;
    } else if (character > range.last) {
      begin = middle;
    } else {
      return true;
    }
  }
}

/**
 * Keep track of the threads currently being executed.
 */
class ThreadList {
 public:
  explicit ThreadList(size_t program_size) {
    has_.resize(program_size, false);
    items_.reserve(program_size);
  }

  /**
   * Add a new thread for the given program counter if it is not already in the list.
   */
  void Spawn(size_t program_counter) {
    #ifdef DEBUG
    if (program_counter >= program_size_) {
      throw std::invalid_argument(
        "Unexpected spawning of a thread at the program counter "
        + std::to_string(program_counter)
        + " since the program size was indicated to be "
        + std::to_string(program_size_)
        );
    }
    #endif

    if (has_[program_counter]) {
      return;
    }

    has_[program_counter] = true;
    items_.push_back(program_counter);
  }

  bool Empty() const {
    return items_.empty();
  }

  /**
   * Pop the thread from the back, returning its program counter.
   * 
   * The order of the threads is not guaranteed.
   */
  size_t Pop() {
    #ifdef DEBUG
    if (items_.empty()) {
      throw std::logic_error(
        "You tried to pop from an empty thread list."
        );
    }
    #endif

    const size_t program_counter = items_.back();
    items_.pop_back();
    has_[program_counter] = false;
    return program_counter;
  }

  /**
   * Clear the thread list, keeping its memory capacity.
   */
  void Clear() {
    std::fill(has_.begin(), has_.end(), false);
    items_.clear();
  }

  /**
   * Return the program counters corresponding to the spawned threads.
   */
  const std::vector<size_t>& Items() const {
    return items_;
  };

 private:
  /**
   * Keep track of the program counters corresponding to the threads so that we can
   * avoid the duplicate threads.
   */
  std::vector<bool> has_;

  /**
   * Keep track of the active threads.
   */
  std::vector<size_t> items_;
};

std::string to_string(const ThreadList& thread_list) {
  if (thread_list.Empty()) {
    return "[]";
  }

  std::vector<size_t> items(thread_list.Items());
  std::sort(items.begin(), items.end());

  std::stringstream ss;
  ss << "[";
  ss << std::to_string(items[0]);

  for (size_t i = 1; i < items.size(); ++i) {
    ss << ", " << items[i];
  }
  ss << "]";

  return ss.str();
}

/**
 * Try to match the program against the text.
 * @return true if the text matches
 */
bool Match(
  const std::vector<std::unique_ptr<Instruction> >& program,
  const std::wstring& text
) {
  if (program.empty()) {
    return false;
  }

  std::cout << "---" << std::endl;
  std::cout << "Text: '" << RepresentWString(text) << "'" << std::endl;
  std::cout << "Program:" << std::endl << to_string(program) << std::endl;

  // NOTE (mristin):
  // We validate early so that we can avoid checks in the loop.
  for (size_t i = 0; i < program.size(); ++i) {
    const Instruction& instruction = *program[i];

    switch (instruction.kind()) {
      case InstructionKind::Jump: {
        const auto& instruction_jump(
          static_cast<  // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            const InstructionJump&
            >(
            instruction
          )
        );

        if (instruction_jump.target >= program.size()) {
          throw std::invalid_argument(
            "Unexpected jump beyond the end of the program. Program has "
              + std::to_string(program.size()) + " instruction(s), "
              + "but the instruction " + std::to_string(i) + " wants to jump to "
              + std::to_string(instruction_jump.target)
          );
        }
        break;
      }

      case InstructionKind::Split: {
        const auto& instruction_split(
          static_cast<  // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            const InstructionSplit&
            >(
            instruction
          )
        );

        if (instruction_split.first_target >= program.size()) {
          throw std::invalid_argument(
            "Unexpected split & jump beyond the end of the program. Program has "
              + std::to_string(program.size()) + " instruction(s), "
              + "but the instruction " + std::to_string(i) + " wants to split and "
              + "make the first jump to "
              + std::to_string(instruction_split.first_target)
          );
        }

        if (instruction_split.second_target >= program.size()) {
          throw std::invalid_argument(
            "Unexpected split & jump beyond the end of the program. Program has "
              + std::to_string(program.size()) + " instruction(s), "
              + "but the instruction " + std::to_string(i) + " wants to split and "
              + "make the second jump to "
              + std::to_string(instruction_split.second_target)
          );
        }

        break;
      }

      default:continue;
    }
  }

  // NOTE (mristin):
  // See: https://swtch.com/~rsc/regexp/regexp2.html,
  // Section "Thompson's Implementation".
  //
  // Also see: https://dl.acm.org/doi/abs/10.1145/363347.363387,
  // the original algorithm from Thompson, K. (1968).
  // "Programming Techniques: Regular expression search algorithm",
  // ACM 6(11).

  std::unique_ptr<ThreadList> clist(std::make_unique<ThreadList>(program.size()));
  std::unique_ptr<ThreadList> nlist(std::make_unique<ThreadList>(program.size()));

  clist->Spawn(0);

  for (const wchar_t character : text) {
    #ifdef DEBUG
    if (!nlist->Empty()) {
      throw std::logic_error(
        "Expected the list of next-to-be-executed threads to be empty, "
        "but it was not."
      );
    }
    #endif

    std::cout << "At character: " << RepresentWCharacter(character) << std::endl;
    std::cout << "* clist before: " << to_string(*clist) << std::endl;
    std::cout << "* nlist before: " << to_string(*nlist) << std::endl;

    while (!clist->Empty()) {
      const size_t program_counter = clist->Pop();

      std::cout << "  * Program counter: " << program_counter << std::endl;

      #ifdef DEBUG
      if (program_counter >= program.size()) {
        throw std::logic_error(
          "Unexpected program counter beyond the program. The program size was "
          + std::to_string(program.size())
          + ", while the program counter of a thread was "
          + std::to_string(program_counter)
          );
      }
      #endif

      const Instruction& instruction = *program[program_counter];

      std::cout << "  * clist before the execution: " << to_string(*clist) << std::endl;
      std::cout << "  * nlist before the execution: " << to_string(*nlist) << std::endl;
      std::cout << "  * Executing: " << to_string(instruction) << std::endl;

      switch (instruction.kind()) {
        case InstructionKind::Char: {
          const auto& instruction_char(
            static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
              const InstructionChar&
              >(instruction)
          );

          if (character != instruction_char.character) {
            // The matching failed for this thread.
            break;
          }

          nlist->Spawn(program_counter + 1);
          break;
        }

        case InstructionKind::Set: {
          const auto& instruction_set(
            static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
              const InstructionSet&
              >(instruction)
          );

          if (!CharacterInRanges(instruction_set.ranges, character)) {
            // The matching failed for this thread.
            break;
          }

          nlist->Spawn(program_counter + 1);
          break;
        }

        case InstructionKind::NotSet: {
          const auto& instruction_not_set(
            static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
              const InstructionNotSet&
              >(instruction)
          );

          if (CharacterInRanges(instruction_not_set.ranges, character)) {
            // The matching failed for this thread.
            break;
          }

          nlist->Spawn(program_counter + 1);
          break;
        }

        case InstructionKind::Any: {
          // NOTE (mristin):
          // We simply proceed to the next instruction at the next character without
          // any checks.
          nlist->Spawn(program_counter + 1);
          break;
        }

        case InstructionKind::Match:std::cout << "  * return true" << std::endl;
          return true;

        case InstructionKind::Jump: {
          const auto& instruction_jump(
            static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
              const InstructionJump&
              >(instruction)
          );

          clist->Spawn(instruction_jump.target);
          break;
        }

        case InstructionKind::Split: {
          const auto& instruction_split(
            static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
              const InstructionSplit&
              >(instruction)
          );

          clist->Spawn(instruction_split.first_target);
          clist->Spawn(instruction_split.second_target);
          break;
        }

        case InstructionKind::End: {
          // The matching failed for this thread as we have just consumed
          // a character.
          break;
        }

        default:
          throw std::logic_error(
            "Unhandled instruction kind: "
              + std::to_string(
                static_cast<std::uint8_t>(instruction.kind())
              )
          );
      }

      std::cout << "  * clist after the execution: " << to_string(*clist) << std::endl;
      std::cout << "  * nlist after the execution: " << to_string(*nlist) << std::endl;
    }

    std::cout << "* clist after all the threads: " << to_string(*clist) << std::endl;
    std::cout << "* nlist after all the threads: " << to_string(*nlist) << std::endl;

    std::swap(clist, nlist);
    nlist->Clear();

    std::cout
      << "* clist after swap: " << to_string(*clist) << std::endl;

    std::cout
      << "* nlist after swap: " << to_string(*nlist) << std::endl;
  }

  std::cout << "Clist after the end-of-input: " << to_string(*clist) << std::endl;

  // NOTE (mristin):
  // We need to process any pending jumps, splits and matches even tough there are
  // no more characters to consume.
  while (!clist->Empty()) {
    const size_t program_counter = clist->Pop();

    std::cout << "  * Program counter: " << program_counter << std::endl;

    #ifdef DEBUG
    if (program_counter >= program.size()) {
        throw std::logic_error(
          "Unexpected program counter beyond the program. The program size was "
          + std::to_string(program.size())
          + ", while the program counter of a thread was "
          + std::to_string(program_counter)
          );
      }
    #endif

    const Instruction& instruction = *program[program_counter];

    std::cout << "  * clist before the execution: " << to_string(*clist) << std::endl;
    std::cout << "  * Executing: " << to_string(instruction) << std::endl;

    switch (instruction.kind()) {
      case InstructionKind::Char: { // NOLINT(bugprone-branch-clone)
        // We reached the end-of-input so there are no characters to be matched.
        // This thread needs therefore to die.
        break;
      }

      case InstructionKind::Set: {
        // We reached the end-of-input so there are no character sets to be matched.
        // This thread needs therefore to die.
        break;
      }

      case InstructionKind::NotSet: {
        // We reached the end-of-input so there are no character sets to be matched.
        // This thread needs therefore to die.
        break;
      }

      case InstructionKind::Any: {
        // We reached the end-of-input so there are no characters to be matched.
        // This thread needs therefore to die.
        break;
      }

      case InstructionKind::Match:std::cout << "  * return true" << std::endl;
        return true;

      case InstructionKind::Jump: {
        const auto& instruction_jump(
          static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            const InstructionJump&
            >(instruction)
        );

        clist->Spawn(instruction_jump.target);
        break;
      }

      case InstructionKind::Split: {
        const auto& instruction_split(
          static_cast< // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            const InstructionSplit&
            >(instruction)
        );

        clist->Spawn(instruction_split.first_target);
        clist->Spawn(instruction_split.second_target);
        break;
      }

      case InstructionKind::End: {
        // We reached the end-of-input so we match and move to the next instruction.
        clist->Spawn(program_counter + 1);
        break;
      }

      default:
        throw std::logic_error(
          "Unhandled instruction kind: "
            + std::to_string(
              static_cast<std::uint8_t>(instruction.kind())
            )
        );
    }
  }

  std::cout << "return false" << std::endl;
  return false;
}

void AssertTrue(bool condition) {
  if (!condition) {
    throw std::logic_error("Assertion failed");
  }
}

void TestCharacterInRangesEmpty() {
  std::vector<Range> ranges;
  AssertTrue(
    !CharacterInRanges(ranges, L'M')
  );
}

void TestCharacterInRangesSinglePositive() {
  std::vector<Range> ranges = {Range(L'A', L'Z')};
  AssertTrue(
    CharacterInRanges(ranges, L'M')
  );
}

void TestCharacterInRangesSingleNegative() {
  std::vector<Range> ranges = {Range(L'A', L'Z')};
  AssertTrue(
    !CharacterInRanges(ranges, L'0')
  );
}

void TestCharacterInRangesMultiple() {
  std::vector<Range> ranges = {
    Range(L'A', L'C'),
    Range(L'D', L'F'),
    Range(L'G', L'I'),
    Range(L'J', L'L'),
    Range(L'M', L'N')
  };

  for (wchar_t character = L'A'; character < L'Z'; ++character) {
    if (character <= L'N') {
      AssertTrue(
        CharacterInRanges(ranges, character)
      );
    } else {
      AssertTrue(
        !CharacterInRanges(ranges, character)
      );
    }
  }
}

void TestProgram1() {
  // Regular expression: ^a$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestProgram2() {
  // Regular expression: ^a+b+$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  program.emplace_back(std::make_unique<InstructionSplit>(0, 2));
  program.emplace_back(std::make_unique<InstructionChar>(L'b'));
  program.emplace_back(std::make_unique<InstructionSplit>(2, 4));
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    !Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    Match(program, L"ab")
  );
  AssertTrue(
    Match(program, L"aabb")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestProgram3() {
  // Regular expression: ^a|b$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionSplit>(1, 3));
  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  program.emplace_back(std::make_unique<InstructionJump>(4));
  program.emplace_back(std::make_unique<InstructionChar>(L'b'));
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestProgram4() {
  // Regular expression: ^a?$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionSplit>(1, 2));
  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestProgram5() {
  // Regular expression: ^a*$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionSplit>(1, 3));
  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  program.emplace_back(std::make_unique<InstructionJump>(0));
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestSet() {
  // Regular expression: ^[a-b]$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(
    std::make_unique<InstructionSet>(
      std::vector<Range>{Range(L'a', L'b')}
    )
  );
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestNotSet() {
  // Regular expression: ^[^a-b]$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(
    std::make_unique<InstructionNotSet>(
      std::vector<Range>{Range(L'a', L'b')}
    )
  );
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    !Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
  AssertTrue(
    Match(program, L"c")
  );
}

void TestAny() {
  // Regular expression: ^.$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionAny>());
  program.emplace_back(std::make_unique<InstructionEnd>());
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    !Match(program, L"aa")
  );
  AssertTrue(
    Match(program, L"b")
  );

  AssertTrue(
    !Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

void TestArbitrarySuffix() {
  // Regular expression: ^a.*$
  std::vector<std::unique_ptr<Instruction> > program;

  program.emplace_back(std::make_unique<InstructionChar>(L'a'));
  // NOTE (mristin):
  // Usually, the last two instructions would be `end` followed by a `match`.
  // However, we can optimize by having a single `match` without an `end` so that
  // we return early.
  program.emplace_back(std::make_unique<InstructionMatch>());

  AssertTrue(
    !Match(program, L"")
  );
  AssertTrue(
    Match(program, L"a")
  );
  AssertTrue(
    Match(program, L"aa")
  );
  AssertTrue(
    !Match(program, L"b")
  );

  AssertTrue(
    Match(program, L"ab")
  );
  AssertTrue(
    !Match(program, L"ba")
  );
}

int main() {
  std::cout << "Hello!" << std::endl;
  try {
    TestCharacterInRangesEmpty();
    TestCharacterInRangesSinglePositive();
    TestCharacterInRangesSingleNegative();
    TestCharacterInRangesMultiple();
    TestProgram1();
    TestProgram2();
    TestProgram3();
    TestProgram4();
    TestProgram5();
    TestSet();
    TestNotSet();
    TestAny();
    TestArbitrarySuffix();
  } catch (const std::exception& exception) {
    std::cerr << "Exception caught: " << exception.what() << std::endl;
    return 1;
  }

  std::cout << "Goodbye!" << std::endl;
  return 0;
}