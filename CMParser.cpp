
#include <iostream>
#include <fstream>
#include <sstream>
#include "document.hpp"
#include "parser.hpp"

std::string slurp(const char* path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        std::cout << " File " << path << " is not accessible \n";
        return "";
    }
    
    auto size = f.tellg();
    std::string buf(size, '\0');
    f.seekg(0);
    f.read(buf.data(), size);
    return buf;
}

void usage(char* argv[]) {

        std::cout << "Usage: " << argv[0] << " <path_to_file> \n";
}

void write_bfs_texts(const CmmlDocument& doc, std::ostream& out)
{
    bool first_output = true;

    for (size_t depth = 0; depth < doc.level_first_text.size(); ++depth) {
        for (NodeIndex node_index = doc.level_first_text[depth];
             node_index != npos;
             node_index = doc.nodes[node_index].next_text_at_same_depth) {

            const Node& node = doc.nodes[node_index];

            const TextContent& text_content =
                std::get<TextContent>(node.content);

            const Span text = text_content.text;

            if (!first_output) {
                out.put('\n');
            }

            out.write(
                doc.source.data() + text.start,
                static_cast<std::streamsize>(text.length)
            );

            first_output = false;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage(argv);
        return 1;
    }

    std::string text = slurp(argv[1]);
    if(text.size() == 0) {
        std::cout << "Nothing to parse. \n";
        return 1;
    }

    CmmlParser parser(std::move(text));
    CmmlDocument doc;

    try {
        doc = parser.parse();
    }
    catch(ParseError& err) {
        std::cout << "Error line " << err.line << " column " << err.column << "\n"; // << ": " << err.message;
        return 1;
    }
    
    write_bfs_texts(doc, std::cout);
	std::cout << "\n";
    
    return 0;
}