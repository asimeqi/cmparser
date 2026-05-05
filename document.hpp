
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <variant>
#include <ostream>


using NodeIndex = uint32_t;
constexpr NodeIndex npos = UINT32_MAX;

struct Span {
    size_t start = 0;
    size_t length = 0;
};

struct TextContent {
    Span text;
};

struct ChildContent {
    NodeIndex first_child = npos;
    NodeIndex last_child = npos;
    uint32_t child_count = 0;
};

using Content = std::variant<std::monostate, TextContent, ChildContent>;

struct Node {
    Span label;
    Content content = std::monostate{};

    NodeIndex next_sibling = npos;
    NodeIndex next_text_at_same_depth = npos;

    uint32_t depth = 0;
};

class CmmlDocument {
public:
    std::string source;
    std::vector<Node> nodes;

    NodeIndex root = npos;

    std::vector<NodeIndex> level_first_text;
    std::vector<NodeIndex> level_last_text;

    std::string_view view(Span span) const {
        return std::string_view(source.data() + span.start, span.length);
    }

    void write_text(Span span, std::ostream& out) const {
        out << view(span);
    }
};

