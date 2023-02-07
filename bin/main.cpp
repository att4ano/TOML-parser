#include "lib/parser.h"
#include <iostream>
#include <fstream>

using namespace omfl;

void XML(Section* current_section, std::fstream& file) {
    file << '<' << current_section->GetName() << '>' << "\n";
    std::vector<Variable*> cur = current_section->GetArr();
    for (int i = 0; i < cur.size(); i++) {
        if (cur[i]->IsInt()) {
            file << '<' << cur[i]->GetName() << '>' << cur[i]->AsInt()
                 << "</" << cur[i]->GetName() << '>' << "\n";
        } else if (cur[i]->IsString()) {
            file << '<' << cur[i]->GetName() << '>' << cur[i]->AsString()
                 << "</" << cur[i]->GetName() << '>' << "\n";
        } else if (cur[i]->IsFloat()) {
            file << '<' << cur[i]->GetName() << '>' << cur[i]->AsFloat()
                 << "</" << cur[i]->GetName() << '>' << "\n";
        } else if (cur[i]->IsBool()) {
            file << '<' << cur[i]->GetName() << '>' << cur[i]->AsBool() << "</" << cur[i]->GetName() << '>' << "\n";
        }
    }
    std::vector <Section*> sections = current_section->GetSectionChild();
    for (int i = 0; i < sections.size(); i++){
        XML(sections[i], file);
    }
    file << "</" << current_section->GetName() << '>' << "\n";
}

int main() {
    std::string data = R"(
    [common]
    name = "Common config"
    description = "Some config"
    version = 1
    [servers]
    [servers.first]
    enabled = true
    ip = "127.0.0.1"
    [servers.second]
    enabled = true
    ip = "127.0.0.1"
        )";
    Parser parser = parse(data);
    std::string path = "C:\\Users\\Home\\Documents\\labs\\labwork-6-dubstephf\\123.xml";
    parser.SetPath(path);
    Section* current_section = &parser.Global();
    std::fstream file(parser.GetPath());
    XML(current_section, file);
    return 0;
}
