#include "parser.hpp"
#include <cassert>


bool CmmlParser::eof()
{
    return pos_ >= doc_.source.size();
}

char CmmlParser::current_char()
{
    return doc_.source[pos_];
}

void CmmlParser::advance()
{
    assert(pos_ < doc_.source.size()); //assumes pos_ is already checked from the caller.
    char c = doc_.source[pos_++];
    if (c == '\r') {
        if (pos_ < doc_.source.size() && doc_.source[pos_] == '\n') ++pos_;  // swallow \n of \r\n
        ++line_; col_ = 1;
    } else if (c == '\n') {
        ++line_; col_ = 1;
    } else {
        ++col_;
    }
}

void CmmlParser::fail(size_t line, size_t column, std::string message)
{
    throw ParseError{line, column, std::move(message)};
}

bool CmmlParser::span_equals(Span span, std::string_view expected)
{
    return span.length == expected.size()
        && doc_.source.compare(span.start, span.length, expected) == 0;
}

bool CmmlParser::spans_equal(Span a, Span b)
{
    if (a.length != b.length) {
        return false;
    }

    return std::char_traits<char>::compare(
        doc_.source.data() + a.start,
        doc_.source.data() + b.start,
        a.length
    ) == 0;
}

std::string CmmlParser::span_to_string(Span span)
{
    return doc_.source.substr(span.start, span.length);
}

CmmlDocument CmmlParser::parse()
{
    while (!eof()) {
        if (current_char() == '<') {
            if (pos_ + 1 < doc_.source.size() && doc_.source[pos_ + 1] == '/') {
                parse_close_tag();
            } else {
                parse_open_tag();
            }
        } else {
            parse_text();
        }
        
    }
    if (!open_stack_.empty()) {
        NodeIndex unclosed = open_stack_.back();

        fail(line_, col_,
            std::string("end of file reached while <")
            + span_to_string(doc_.nodes[unclosed].label)
            + "> is still open");
    }

    if (doc_.root == npos) {
        fail(line_, col_, "document is missing required root <doc> tag");
    }

    return std::move(doc_);

}

void CmmlParser::parse_open_tag()
{
    const size_t tag_start_pos = pos_;
    const size_t tag_start_line = line_;
    const size_t tag_start_column = col_;

    if (eof() || current_char() != '<') {
        fail(line_, col_, "expected opening tag");
    }

    // A second root-level tag after </doc> is invalid.
    if (doc_.root != npos && open_stack_.empty()) {
        fail(tag_start_line, tag_start_column,
             "tag appears after the closing </doc> tag");
    }

    // If the current parent already contains text, it cannot contain a child tag.
    if (!open_stack_.empty()) {
        NodeIndex parent_index = open_stack_.back();

        if (std::holds_alternative<TextContent>(
                doc_.nodes[parent_index].content)) {
            fail(tag_start_line, tag_start_column,
                 "child tag appears inside a scope that already contains plain text");
        }
    }

    // Consume '<'.
    advance();

    if (eof()) {
        fail(line_, col_, "end of file reached after '<'");
    }

    // opening tags only please
    if (current_char() == '/') {
        fail(line_, col_,
             "expected opening tag, found closing tag");
    }

    const size_t label_start_pos = pos_;
    const size_t label_start_line = line_;
    const size_t label_start_column = col_;

    if (current_char() == '>') {
        fail(label_start_line, label_start_column,
             "tag label may not be empty");
    }

    while (!eof() && current_char() != '>') {
        unsigned char c = static_cast<unsigned char>(current_char());

        if (current_char() == '<') {
            fail(line_, col_,
                 "reserved symbol '<' is not allowed inside a tag label");
        }

        if (current_char() == '/') {
            fail(line_, col_,
                 "reserved symbol '/' is not allowed inside a tag label");
        }

        if (std::isspace(c)) {
            fail(line_, col_,
                 "whitespace is not allowed inside a tag declaration");
        }

        advance();
    }

    if (eof()) {
        fail(line_, col_,
             "end of file reached before closing '>' of opening tag");
    }

    const size_t label_length = pos_ - label_start_pos;

    if (label_length == 0) {
        fail(label_start_line, label_start_column,
             "tag label may not be empty");
    }

    Span label{label_start_pos, label_length};

    // The first real tag must be <doc>.
    if (doc_.root == npos) {
        if (!span_equals(label, "doc")) {
            fail(label_start_line, label_start_column,
                 "root tag must be <doc>");
        }
    }


    // eat '>'.
    advance();

    // NodeIndex new_index = static_cast<NodeIndex>(doc_.nodes.size());

    // Node new_node;
    // new_node.label = label;
    // new_node.next_sibling = npos;
    // new_node.next_text_at_same_depth = npos;
    // new_node.depth = static_cast<uint32_t>(open_stack_.size());

    // doc_.nodes.push_back(new_node);
    NodeIndex new_index = add_node(label, tag_start_line, tag_start_column );

    open_stack_.push_back(new_index);

}

void CmmlParser::parse_close_tag()
{
    const size_t tag_start_line = line_;
    const size_t tag_start_column = col_;

    if (eof() || current_char() != '<') {
        fail(line_, col_, "expected closing tag");
    }

    // Consume '<'.
    advance();

    if (eof() || current_char() != '/') {
        fail(line_, col_, "expected '/' after '<' in closing tag");
    }

    // Consume '/'.
    advance();

    if (open_stack_.empty()) {
        fail(tag_start_line, tag_start_column,
             "closing tag appears outside any open tag scope");
    }

    if (eof()) {
        fail(line_, col_,
             "end of file reached after '</'");
    }

    const size_t label_start_pos = pos_;
    const size_t label_start_line = line_;
    const size_t label_start_column = col_;

    if (current_char() == '>') {
        fail(label_start_line, label_start_column,
             "closing tag label may not be empty");
    }

    while (!eof() && current_char() != '>') {
        unsigned char c = static_cast<unsigned char>(current_char());

        if (current_char() == '<') {
            fail(line_, col_,
                 "reserved symbol '<' is not allowed inside a tag label");
        }

        if (current_char() == '/') {
            fail(line_, col_,
                 "reserved symbol '/' is not allowed inside a tag label");
        }

        if (std::isspace(c)) {
            fail(line_, col_,
                 "whitespace is not allowed inside a tag declaration");
        }

        advance();
    }

    if (eof()) {
        fail(line_, col_,
             "end of file reached before closing '>' of closing tag");
    }

    Span closing_label{
        label_start_pos,
        pos_ - label_start_pos
    };

    NodeIndex open_node_index = open_stack_.back();
    Node& open_node = doc_.nodes[open_node_index];

    if (!spans_equal(open_node.label, closing_label)) {
        fail(label_start_line, label_start_column,
             std::string("closing tag </")
             + span_to_string(closing_label)
             + "> does not match the currently open <"
             + span_to_string(open_node.label)
             + "> tag; expected </"
             + span_to_string(open_node.label)
             + ">");
    }

    // CMML says a scope may contain either additional tags or plain text.
    // Does that mean empty scopes are not allowed? I think so.
    if (std::holds_alternative<std::monostate>(open_node.content)) {
        fail(tag_start_line, tag_start_column,
             std::string("tag <")
             + span_to_string(open_node.label)
             + "> has an empty scope");
    }

    // Consume '>'.
    advance();

    open_stack_.pop_back();
}

void CmmlParser::parse_text()
{
        constexpr size_t no_position = std::string::npos;

    size_t first_non_ws_pos = no_position;
    size_t first_non_ws_line = 0;
    size_t first_non_ws_column = 0;

    size_t last_non_ws_end = 0;

    while (!eof() && current_char() != '<') {
        char c = current_char();

        if (c == '>') {
            fail(line_, col_,
                 "reserved symbol '>' appears in plain text");
        }

        if (c == '/') {
            fail(line_, col_,
                 "reserved symbol '/' appears in plain text");
        }

        if (!std::isspace(static_cast<unsigned char>(c))) {
            if (first_non_ws_pos == no_position) {
                first_non_ws_pos = pos_;
                first_non_ws_line = line_;
                first_non_ws_column = col_;
            }

            last_non_ws_end = pos_ + 1;
        }

        advance();
    }

    // Whitespace-only text outside tag declarations is allowed and ignored.
    if (first_non_ws_pos == no_position) {
        return;
    }

    if (open_stack_.empty()) {
        if (doc_.root == npos) {
            fail(first_non_ws_line, first_non_ws_column,
                 "non-whitespace text appears before the root <doc> tag");
        } else {
            fail(first_non_ws_line, first_non_ws_column,
                 "non-whitespace text appears after the closing </doc> tag");
        }
    }

    NodeIndex current_node_index = open_stack_.back();
    Node& current_node = doc_.nodes[current_node_index];

    if (std::holds_alternative<ChildContent>(current_node.content)) {
        fail(first_non_ws_line, first_non_ws_column,
             std::string("plain text starts inside <")
             + span_to_string(current_node.label)
             + "> after that scope already contains child tags");
    }

    if (std::holds_alternative<TextContent>(current_node.content)) {
        fail(first_non_ws_line, first_non_ws_column,
             std::string("scope <")
             + span_to_string(current_node.label)
             + "> already contains plain text");
    }

    Span text_span{
        first_non_ws_pos,
        last_non_ws_end - first_non_ws_pos
    };

    current_node.content = TextContent{text_span};

    add_text_node_at_depth(
        current_node_index,
        current_node.depth
    );
}

NodeIndex CmmlParser::add_node(Span label, size_t tag_start_line, size_t tag_start_column)
{

    NodeIndex new_index = static_cast<NodeIndex>(doc_.nodes.size());

    Node new_node;
    new_node.label = label;
    new_node.content = std::monostate{};
    new_node.next_sibling = npos;
    new_node.next_text_at_same_depth = npos;
    new_node.depth = static_cast<uint32_t>(open_stack_.size());

    doc_.nodes.push_back(new_node);

    if (open_stack_.empty()) {
        if (doc_.root != npos) {
            fail(tag_start_line, tag_start_column,
                 "tag appears after the closing </doc> tag");
        }

        doc_.root = new_index;
        return new_index;
    }

    NodeIndex parent_index = open_stack_.back();
    Node& parent = doc_.nodes[parent_index];

    if (std::holds_alternative<TextContent>(parent.content)) {
        fail(tag_start_line, tag_start_column,
             "child tag appears inside a scope that already contains plain text");
    }

    if (std::holds_alternative<std::monostate>(parent.content)) {
        parent.content = ChildContent{
            new_index,
            new_index,
            1
        };
    } else {
        ChildContent& children = std::get<ChildContent>(parent.content);

        doc_.nodes[children.last_child].next_sibling = new_index;
        children.last_child = new_index;
        ++children.child_count;
    }

    return new_index;
}

void CmmlParser::add_text_node_at_depth(NodeIndex node_index, uint32_t depth)
{
    while (depth >= doc_.level_first_text.size()) {
        doc_.level_first_text.push_back(npos);
        doc_.level_last_text.push_back(npos);
    }

    doc_.nodes[node_index].next_text_at_same_depth = npos;

    NodeIndex& first = doc_.level_first_text[depth];
    NodeIndex& last = doc_.level_last_text[depth];

    if (first == npos) {
        first = node_index;
        last = node_index;
    } else {
        doc_.nodes[last].next_text_at_same_depth = node_index;
        last = node_index;
    }
}


