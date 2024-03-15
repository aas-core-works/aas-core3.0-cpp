#include "./common_xmlization.hpp"
#include "./common.hpp"

#include <aas_core/aas_3_0/xmlization.hpp>

#pragma warning(push, 0)
#include <expat.h>
#pragma warning(pop)

#include <fstream>

namespace aas = aas_core::aas_3_0;

namespace test {
namespace common {
namespace xmlization {

const std::vector<std::string> kCausesForDeserializationFailure = {
  "TypeViolation",
  "RequiredViolation",
  "EnumViolation",
  "NullViolation",
  "UnexpectedAdditionalProperty"
};

std::shared_ptr<
  aas_core::aas_3_0::types::IClass
> MustReadInstance(
  const std::filesystem::path& path
) {
  std::ifstream ifs(path);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > instance = aas::xmlization::From(
    ifs
  );

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to read XML from ",
        path.string(),
        "; the bad bit of the file stream is set"
      )
    );
  }

  if (!instance.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to parse the instance from the XML file ",
        path.string(),
        ": ",
        aas::common::WstringToUtf8(instance.error().path.ToWstring()),
        ": ",
        aas::common::WstringToUtf8(instance.error().cause)
      )
    );
  }

  return *instance;
}

namespace canonicalizer {

struct INode {
  std::string text;

  INode(
    std::string a_text
  ) :
    text(a_text) {
    // Intentionally empty.
  }

  virtual std::unique_ptr<INode> Copy() const = 0;
  virtual ~INode() = default;
};

struct StartNode : INode {
  std::string name;
  std::string text;

  StartNode(
    std::string a_name,
    std::string a_text
  ) :
    name(std::move(a_name)),
    INode(std::move(a_text)) {
    // Intentionally empty.
  }

  std::unique_ptr<INode> Copy() const override {
    return std::make_unique<StartNode>(*this);
  }

  ~StartNode() override = default;
};

struct TextNode : INode {
  TextNode(
    std::string a_text
  ) : INode(std::move(a_text)) {
    // Intentionally empty.
  }

  std::unique_ptr<INode> Copy() const override {
    return std::make_unique<TextNode>(*this);
  }

  ~TextNode() override = default;
};

struct StopNode : INode {
  std::string name;
  std::string text;

  StopNode(
    std::string a_name,
    std::string a_text
  ) :
    name(std::move(a_name)),
    INode(std::move(a_text)) {
    // Intentionally empty.
  }

  std::unique_ptr<INode> Copy() const override {
    return std::make_unique<StopNode>(*this);
  }

  ~StopNode() override = default;
};

/**
 * Structure the data passed over to Expat XML reader.
 */
struct OurData {
  XML_Parser parser;
  std::optional<std::string> error;

  std::deque<std::unique_ptr<INode> > nodes;

  OurData(
    XML_Parser a_parser
  ) :
    parser(a_parser) {
    // Intentionally empty.
  }
};  // struct OurData

aas::common::expected<
  std::string,
  std::string
> EscapeForXmlAttribute(const std::string_view& text) {
  // NOTE (mristin):
  // See: https://stackoverflow.com/questions/19766669/which-characters-are-permitted-in-xml-attributes
  //
  // In addition, we assume that we will always surround an attribute by a double-quote.

  size_t size = 0;
  for (char character : text
    ) {
    switch (character) {
      case '<':size += sizeof("&lt;");
        break;
      case '"':size += sizeof("&quot;");
        break;
      case '\0':
        return aas::common::unexpected<std::string>(
          "Unexpected zero character in the text to be escaped "
          "for XML attribute. The XML format does not allow zero characters, "
          "not even in escaped form."
        );
      default:++size;
    }
  }

  std::string result;
  result.reserve(size);

  for (char character : text
    ) {
    switch (character) {
      case '<':result.append("&lt;");
        break;
      case '"':result.append("&quot;");
        break;
      default:result.push_back(character);
    }
  }

  return result;
}

aas::common::expected<
  std::string,
  std::string
> EscapeForXmlText(const std::string_view& text) {
  size_t size = 0;
  for (char character : text
    ) {
    switch (character) {
      case '<':size += sizeof("&lt;");
        break;
      case '>':size += sizeof("&gt;");
        break;
      case '&':size += sizeof("&amp;");
        break;
      case '\'':size += sizeof("&apos;");
        break;
      case '"':size += sizeof("&quot;");
        break;
      case '\0':
        return aas::common::unexpected(
          "Unexpected zero character in the text to be escaped "
          "for XML attribute. The XML format does not allow zero characters, "
          "not even in escaped form."
        );
      default:++size;
    }
  }

  if (text.size() == size) {
    return std::string(text);
  }

  std::string result;
  result.reserve(size);

  for (char character : text
    ) {
    switch (character) {
      case '<':result.append("&lt;");
        break;
      case '>':result.append("&gt;");
        break;
      case '&':result.append("&amp;");
        break;
      case '\'':result.append("&apos;");
        break;
      case '"':result.append("&quot;");
        break;
      default:result.push_back(character);
    }
  }

  return result;
}

void XMLCALL OnStartElement(
  void* user_data,
  const char* name,
  const char* attributes[]
) {
  auto our_data = static_cast<OurData*>(user_data);

  // NOTE (mristin):
  // Since Expat continues parsing and adding nodes even if the parsing is
  // suspended (see the documentation of `XML_StopParser`), we have to ignore
  // any further events.
  if (our_data->error.has_value()) {
    return;
  }

  std::stringstream ss;
  ss << "<" << name;

  for (size_t i = 0; attributes[i] != nullptr; i += 2
    ) {
    const char* attribute_name = attributes[i];
    const char* attribute_value = attributes[i + 1];

    aas::common::expected<
      std::string,
      std::string
    > escaped = EscapeForXmlAttribute(
      std::string_view(
        attribute_value,
        strlen(attribute_value)
      )
    );
    if (!escaped.has_value()) {
      our_data->error = escaped.error();
      XML_StopParser(our_data->parser, false);
      return;
    }

    ss
      << " "
      << attribute_name << "=\""
      << *escaped
      << "\"";
  }

  ss << ">";

  our_data->nodes.emplace_back(
    aas::common::make_unique<StartNode>(
      name,
      std::move(ss.str())
    )
  );
}

void XMLCALL OnStopElement(
  void* user_data,
  const char* name
) {
  auto* our_data = static_cast<OurData*>(user_data);

  // NOTE (mristin):
  // Since Expat continues parsing and adding nodes even if the parsing is
  // suspended (see the documentation of `XML_StopParser`), we have to ignore
  // any further events.
  if (our_data->error.has_value()) {
    return;
  }

  our_data->nodes.emplace_back(
    aas::common::make_unique<StopNode>(
      name,
      aas::common::Concat(
        "</", name, ">"
      )
    )
  );
}

void XMLCALL OnText(
  void* user_data,
  const char* val,
  int len
) {
  auto our_data = static_cast<OurData*>(user_data);

  // NOTE (mristin):
  // Since Expat continues parsing and adding nodes even if the parsing is
  // suspended (see the documentation of `XML_StopParser`), we have to ignore
  // any further events.
  if (our_data->error.has_value()) {
    return;
  }

  aas::common::expected<
    std::string,
    std::string
  > escaped = EscapeForXmlText(std::string_view(val, len));

  if (!escaped.has_value()) {
    our_data->error = std::move(escaped.error());
    XML_StopParser(our_data->parser, false);
    return;
  }

  our_data->nodes.emplace_back(
    aas::common::make_unique<TextNode>(
      std::move(*escaped)
    )
  );
}

std::deque<std::unique_ptr<INode> > MergeTextNodes(
  const std::deque<std::unique_ptr<INode> >& nodes
) {
  std::deque<std::string> parts;

  std::deque<std::unique_ptr<INode> > result;

  for (const std::unique_ptr<INode>& node : nodes
    ) {
    auto start_node = dynamic_cast<const StartNode*>(node.get());
    auto stop_node = dynamic_cast<const StopNode*>(node.get());

    if (start_node != nullptr || stop_node != nullptr) {
      if (!parts.empty()) {
        result.emplace_back(
          std::make_unique<TextNode>(
            test::common::JoinStrings(parts, "")
          )
        );
        parts.clear();
      }

      result.emplace_back(node->Copy());

      continue;
    }

    auto text_node = dynamic_cast<const TextNode*>(node.get());
    if (text_node != nullptr) {
      parts.push_back(text_node->text);
    }
  }

  if (!parts.empty()) {
    result.emplace_back(
      std::make_unique<TextNode>(
        test::common::JoinStrings(parts, "")
      )
    );
  }

  return result;
}

bool IsWhitespace(const std::string& text) {
  for (const char character : text
    ) {
    switch (character) {
      case '\n':continue;
      case '\t':continue;
      case '\r':continue;
      case ' ':continue;
      default:return false;
    }
  }
  return true;
}

/**
 * Remove the whitespace between two start/stop nodes with differing names.
 */
std::deque<std::unique_ptr<INode> > RemoveNonSemanticWhiteSpace(
  const std::deque<std::unique_ptr<INode> >& nodes
) {
  std::deque<std::unique_ptr<INode> > result;

  if (nodes.size() <= 2) {
    for (const auto& node : nodes
      ) {
      result.emplace_back(node->Copy());
    }
    return result;
  }

  INode* previous = nullptr;
  StartNode* previous_as_start = nullptr;

  auto it = nodes.begin();
  INode* current = it->get();
  StartNode* current_as_start = dynamic_cast<StartNode*>(current);
  TextNode* current_as_text = dynamic_cast<TextNode*>(current);

  ++it;
  INode* lookahead = it->get();
  StartNode* lookahead_as_start = dynamic_cast<StartNode*>(lookahead);
  StopNode* lookahead_as_stop = dynamic_cast<StopNode*>(lookahead);
  TextNode* lookahead_as_text = dynamic_cast<TextNode*>(lookahead);

  ++it;

  while (true) {
    // region Determine what to do

    // If not set, we include the current node in the result.
    bool ignore = (
      current_as_text != nullptr
        && IsWhitespace(current_as_text->text)
        && (
          previous == nullptr
            || lookahead == nullptr
            || !(
              previous_as_start != nullptr
                && lookahead_as_stop != nullptr
                && previous_as_start->name == lookahead_as_stop->name
            )
        )
    );
    // endregion

    // region Act
    if (!ignore) {
      result.emplace_back(current->Copy());
    }
    // endregion

    // region Shift
    previous = current;
    previous_as_start = current_as_start;

    current = lookahead;
    current_as_start = lookahead_as_start;
    current_as_text = lookahead_as_text;

    if (current == nullptr) {
      break;
    }

    if (it == nodes.end()) {
      lookahead = nullptr;
      lookahead_as_start = nullptr;
      lookahead_as_stop = nullptr;
      lookahead_as_text = nullptr;
    } else {
      lookahead = it->get();
      lookahead_as_start = dynamic_cast<StartNode*>(lookahead);
      lookahead_as_stop = dynamic_cast<StopNode*>(lookahead);
      lookahead_as_text = dynamic_cast<TextNode*>(lookahead);

      ++it;
    }
    // endregion
  }

  return result;
}

}  // namespace canonicalizer

std::string CanonicalizeXml(
  const std::string& xml
) {
  const char namespace_separator = ':';
  XML_Parser parser = XML_ParserCreateNS(nullptr, namespace_separator);

  std::unique_ptr<canonicalizer::OurData> our_data(
    aas::common::make_unique<canonicalizer::OurData>(
      parser
    )
  );

  XML_SetUserData(parser, our_data.get());
  XML_SetElementHandler(
    parser,
    canonicalizer::OnStartElement,
    canonicalizer::OnStopElement
  );
  XML_SetCharacterDataHandler(parser, canonicalizer::OnText);

  XML_Status status = XML_Parse(
    parser,
    xml.data(),
    static_cast<int>(xml.size()),
    true
  );

  if (status == XML_STATUS_ERROR) {
    XML_Error error_code = XML_GetErrorCode(parser);

    std::string message;

    if (error_code == XML_ERROR_ABORTED) {
      if (!our_data->error.has_value()) {
        throw std::logic_error(
          "The XML parsing was aborted, "
          "but the error in our_data was empty"
        );
      }

      message = std::move(*our_data->error);
    } else {
      const XML_LChar* error_str = XML_ErrorString(error_code);
      message = error_str;
    }

    throw std::invalid_argument(
      aas::common::Concat(
        "Failed to parse XML for canonicalization: ",
        std::to_string(XML_GetCurrentLineNumber(parser)),
        ":",
        std::to_string(XML_GetCurrentColumnNumber(parser)),
        ": ",
        message
      )
    );
  }

  std::deque<
    std::unique_ptr<canonicalizer::INode>
  > nodes(
    std::move(our_data->nodes)
  );

  nodes = canonicalizer::MergeTextNodes(nodes);

  nodes = canonicalizer::RemoveNonSemanticWhiteSpace(nodes);

  size_t size = 0;
  for (const auto& node : nodes) {
    size += node->text.size();
  }

  std::string result;
  result.reserve(size);

  for (const auto& node : nodes) {
    result.append(node->text);
  }

  return result;
}

std::shared_ptr<
  aas_core::aas_3_0::types::IClass
> MustDeserializeFile(
  const std::filesystem::path& path
) {
  std::ifstream ifs(path, std::ios::binary);

  aas::common::expected<
    std::shared_ptr<aas::types::IClass>,
    aas::xmlization::DeserializationError
  > deserialized = aas::xmlization::From(
    ifs
  );

  if (ifs.bad()) {
    throw std::runtime_error(
      aas::common::Concat(
        "The file stream is in the bad mode after "
        "reading and parsing the file as XML: ",
        path.string()
      )
    );
  }

  if (!deserialized.has_value()) {
    throw std::runtime_error(
      aas::common::Concat(
        "Failed to de-serialize from ",
        path.string(),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().path.ToWstring()
        ),
        ": ",
        aas::common::WstringToUtf8(
          deserialized.error().cause
        )
      )
    );
  }

  return std::move(deserialized.value());
}

}  // namespace xmlization
}  // namespace common
}  // namespace test
