#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include "document.hpp"


struct ParseError {
    size_t line;
    size_t column;
    std::string message;
};

class CmmlParser {
public:
    explicit CmmlParser(std::string input)
    {
        doc_.source = std::move(input);
    }

    CmmlDocument parse();

private:
    CmmlDocument doc_;

    size_t pos_ = 0;
    size_t line_ = 1;
    size_t col_ = 1;

    size_t depth_ = 0;

    std::vector<NodeIndex> open_stack_;

    void parse_open_tag();
    void parse_close_tag();
    void parse_text();
    NodeIndex add_node(Span label, size_t tag_start_line, size_t tag_start_column);
    void add_text_node_at_depth(NodeIndex node_index, uint32_t depth);

    // helper functions:
    bool eof(); 
    char current_char(); 
    void advance();
    void fail(size_t line, size_t column, std::string message);
    bool span_equals(Span span, std::string_view expected);
    bool spans_equal(Span a, Span b);
    std::string span_to_string(Span span);
};