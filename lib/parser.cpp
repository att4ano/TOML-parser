#include "parser.h"

#include <utility>

using namespace omfl;

bool Element::IsInt() {
    return dynamic_cast<Variable*> (this)->IsInt();
}

bool Element::IsString() {
    return dynamic_cast<Variable*>(this)->IsString();
}

bool Element::IsFloat() {
    return dynamic_cast<Variable*> (this)->IsFloat();
}

bool Element::IsBool() {
    return dynamic_cast<Variable*> (this)->IsBool();
}

bool Element::IsArray() {
    return dynamic_cast<Variable*> (this)->IsArray();
}

int Element::AsInt() {
    return dynamic_cast<Variable*> (this)->AsInt();
}

int Element::AsIntOrDefault(int default_value) {
    return dynamic_cast<Variable*> (this)->AsIntOrDefault(default_value);
}

std::string Element::AsString() {
    return dynamic_cast<Variable*> (this)->AsString();
}

std::string Element::AsStringOrDefault(std::string default_value) {
    return dynamic_cast<Variable*> (this)->AsStringOrDefault(std::move(default_value));
}

bool Element::AsBool() {
    return dynamic_cast<Variable*> (this)->AsBool();
}

float Element::AsFloat() {
    return dynamic_cast<Variable*> (this)->AsFloat();
}

float Element::AsFloatOrDefault(float default_value) {
    return dynamic_cast<Variable*> (this)->AsFloatOrDefault(default_value);
}

int Variable::AsInt() {
    if (this->type_ == INT) {
        return dynamic_cast<IntVar*> (this)->GetValue();
    } else {
        throw std::invalid_argument("Invalid argument");
    }
}

int Variable::AsIntOrDefault(int default_value) {
    if (this->type_ == INT) {
        return dynamic_cast<IntVar*> (this)->GetValue();
    } else {
        return default_value;
    }
}

std::string Variable::AsString() {
    if (this->type_ == STRING) {
        return dynamic_cast<StringVar*> (this)->GetValue();
    } else {
        throw std::invalid_argument("Invalid argument");
    }
}

std::string Variable::AsStringOrDefault(std::string default_value) {
    if (this->type_ == STRING) {
        return dynamic_cast<StringVar*> (this)->GetValue();
    } else {
        return default_value;
    }
}

bool Variable::AsBool() {
    if (this->type_ == BOOL) {
        return dynamic_cast<BoolVar*> (this)->GetValue();
    } else {
        throw std::invalid_argument("Invalid argument");
    }
}

float Variable::AsFloat() {
    if (this->type_ == FLOAT) {
        return dynamic_cast<FloatVar*> (this)->GetValue();
    } else {
        throw std::invalid_argument("Invalid argument");
    }
}

float Variable::AsFloatOrDefault(float default_value) {
    if (this->type_ == FLOAT) {
        return dynamic_cast<FloatVar*> (this)->GetValue();
    } else {
        return default_value;
    }
}

Variable& Element::operator[](int index) {
    if (this->type_element == VARIABLE) {
        return dynamic_cast<Variable*> (this)->operator[](index);
    } else {
        throw std::invalid_argument("invalid argument");
    }
}

Variable& Variable::operator[](int index) {
    if (this->type_ == ARRAY) {
        return dynamic_cast<Array*> (this)->operator[](index);
    } else {
        throw std::invalid_argument("invalid argument");
    }
}

void Section::AddNewIntVar(std::string& name, int value) {
    IntVar* int_var = new IntVar(value, std::move(name));
    var_list.push_back(int_var);
}

void Section::AddNewStringVar(std::string& name, std::string value) {
    StringVar* string_var = new StringVar(std::move(value), std::move(name));
    var_list.push_back(string_var);
}

void Section::AddNewBoolVar(std::string& name, bool value) {
    BoolVar* bool_var = new BoolVar(value, std::move(name));
    var_list.push_back(bool_var);
}

void Section::AddNewFloatVar(std::string& name, float value) {
    FloatVar* float_var = new FloatVar(value, std::move(name));
    var_list.push_back(float_var);
}

void Section::AddNewArray(std::string name, Array& array) {
    Array* new_array = &array;
    new_array->SetName(std::move(name));
    var_list.push_back(new_array);
}

Section& Parser::AddNewSection(std::string& name, std::string parent_name) {
    Section* parent = new Section();
    bool find_parent = false;
    for (int i = 0; i < section_list.size(); i++) {
        if (section_list[i]->GetName() == parent_name) {
            parent = section_list[i];
            find_parent = true;
            break;
        }
    }
    if (find_parent) {
        Section* new_section = new Section(name, parent);
        parent->GetSectionChild().push_back(new_section);
        section_list.push_back(new_section);
    } else {
        Section* new_section = new Section(name, section_list[0]);
        global_section.GetSectionChild().push_back(new_section);
        section_list.push_back(new_section);
    }
    return *section_list.back();
}

std::string Element::GetName() {
    if (this->type_element == SECTION) {
        Section* tmp = dynamic_cast<Section*> (this);
        return tmp->GetName();
    } else {
        Variable* tmp = dynamic_cast<Variable*> (this);
        return tmp->GetName();
    }
}

Element& Element::Get(std::string name_variable) {
    Section* tmp = dynamic_cast<Section*> (this);
    return tmp->Get(name_variable);
}

Element& Section::Get(std::string name_variable) {
    Section* current_section = this;
    if (name_variable.find('.') == std::string::npos) {
        for (int i = 0; i < current_section->child_section.size(); i++) {
            if (current_section->child_section[i]->GetName() == name_variable) {
                return *current_section->child_section[i];
            }
        }
        for (int i = 0; i < current_section->var_list.size(); i++) {
            if (current_section->var_list[i]->GetName() == name_variable) {
                return *current_section->var_list[i];
            }
        }
    } else {
        std::istringstream name_stream(name_variable);
        std::string element;
        std::vector<std::string> element_list;
        while (std::getline(name_stream, element, '.')) {
            element_list.push_back(element);
        }
        for (int i = 0; i < current_section->child_section.size(); i++) {
            if (current_section->child_section[i]->GetName() == element_list.back()) {
                return *current_section->child_section[i];
            }
        }
        for (int i = 0; i < current_section->var_list.size(); i++) {
            if (current_section->var_list[i]->GetName() == element_list.back()) {
                return *current_section->var_list[i];
            }
        }
    }
}

Element& Parser::Get(std::string name_variable) const {
    if (name_variable.find('.') == std::string::npos) {
        for (int i = 0; i < this->section_list.size(); i++) {
            if (this->section_list[i]->GetName() == name_variable) {
                return *this->section_list[i];
            } else {
                for (int j = 0; j < this->section_list[i]->GetArr().size(); j++) {
                    if (this->section_list[i]->GetArr()[j]->GetName() == name_variable) {
                        return *this->section_list[i]->GetArr()[j];
                    }
                }
            }
        }
    } else {
        std::istringstream name_stream(name_variable);
        std::string element;
        std::vector<std::string> element_list;
        while (std::getline(name_stream, element, '.')) {
            element_list.push_back(element);
        }
        for (int i = 0; i < this->section_list.size(); i++) {
            if (this->section_list[i]->GetName() == element_list.back()) {
                return *this->section_list[i];
            } else {
                for (int j = 0; j < this->section_list[i]->GetArr().size(); j++) {
                    if (this->section_list[i]->GetArr()[j]->GetName() == element_list.back()) {
                        return *this->section_list[i]->GetArr()[j];
                    }
                }
            }
        }
    }
}

bool IsNum(std::string num) {
    if (num[0] == '+' || num[0] == '-') {
        if (num.size() == 1) {
            return false;
        } else {
            int i = 1;
            while (num[i] != '\0') {
                if ((num[i] == '.' && i == 1) || (!std::isdigit(num[i]) && num[i] != '.') ||
                    (num[i] == '.' && i == num.size() - 1)) {
                    return false;
                }
                i++;
            }
        }
    } else {
        int i = 0;
        while (num[i] != '\0') {
            if ((num[i] == '.' && i == 0) || (!std::isdigit(num[i]) && num[i] != '.') ||
                (num[i] == '.' && i == num.size() - 1)) {
                return false;
            }
            i++;
        }
    }
    return true;
}

bool IsString(std::string line_value) {
    int i = 1;
    while (i != line_value.size() - 1) {
        if (line_value[i] == '\"') {
            return false;
        }
        i++;
    }
    return true;
}

TYPE omfl::TypeVar(std::string line_value) {
    if (line_value.back() == '\"' && line_value.front() == '\"') {
        return STRING;
    } else if (line_value == "true" || line_value == "false") {
        return BOOL;
    } else if (line_value.find('.') != std::string::npos &&
               std::count(line_value.begin(), line_value.end(), '.') == 1 &&
               IsNum(line_value) && line_value.size() != 0) {
        return FLOAT;
    } else if (line_value.front() == '[' && line_value.back() == ']') {
        return ARRAY;
    } else if (IsNum(line_value)) {
        return INT;
    } else {
        return UNDEFINED;
    }
}

void omfl::TakeToStr(std::string& line) {
    if (line.find('#') != std::string::npos) {
        line.erase(line.find('#'));
    }
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t') {
        i++;
    }
    line.erase(0, i);
    i = line.size() - 1;
    while (line[i] == ' ' || line[i] == '\t') {
        i--;
    }
    line.erase(i + 1);
    int index = line.find('=');
    i = index;
    while (line[i - 1] == ' ' || line[i] == '\t') {
        i--;
    }
    line.erase(i, index - i);
    index = line.find('=');
    i = index + 1;
    while (line[i] == ' ' || line[i] == '\t') {
        i++;
    }
    line.erase(index + 1, i - index - 1);
}

void omfl::DeleteWhiteSpaces(std::string& line) {
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t') {
        i++;
    }
    line.erase(0, i);
    i = line.size() - 1;
    while (line[i] == ' ' || line[i] == '\t') {
        i--;
    }
    line.erase(i + 1);
}

ELEMENT omfl::CheckElement(std::string current_line) {
    if (current_line.find('=') != std::string::npos) {
        return VARIABLE;
    } else if (current_line[0] == '[' && current_line[current_line.size() - 1] == ']') {
        return SECTION;
    } else if (current_line.find('#') != std::string::npos) {
        return COMMENT;
    } else if (!current_line.empty()) {
        return UNKNOWN;
    } else {
        return EMPTY;
    }
}

bool omfl::CheckVarName(std::string line_name) {
    if (line_name.size() == 0) {
        return false;
    } else {
        for (int i = 0; i < line_name.size(); i++) {
            if (std::isdigit(line_name[i]) || std::isalpha(line_name[i]) || line_name[i] == '-' ||
                line_name[i] == '_') {
                continue;
            } else {
                return false;
            }
        }
        return true;
    }
}

std::pair<std::string, std::string> omfl::ParseVar(std::string current_var) {
    int i = 0;
    std::string name_var;
    std::string value;
    while (current_var[i] != '=') {
        name_var += current_var[i];
        i++;
    }
    i++;
    while (current_var[i] != '\0') {
        value += current_var[i];
        i++;
    }
    return std::make_pair(name_var, value);
}

bool omfl::CheckSection(std::string section) {
    if (section.size() == 0 || section.find('.') == 0 || section.find('.') == section.size() - 1) {
        return false;
    }
    std::istringstream section_stream(section);
    std::string section_value;
    while (std::getline(section_stream, section_value, '.')) {
        if (section_value.empty() || section.find(']') != std::string::npos || section.find('[') != std::string::npos) {
            return false;
        }
    }
    return true;
}

bool omfl::CheckVarValue(std::string line_value) {
    if (line_value.size() == 0) {
        return false;
    } else {
        if (TypeVar(line_value) == UNDEFINED) {
            return false;
        } else if (TypeVar(line_value) == INT) {
            if (line_value.front() != '+' && line_value.front() != '-') {
                int i = 0;
                while (line_value[i] != '\0') {
                    if (!std::isdigit(line_value[i])) {
                        return false;
                    }
                    i++;
                }
                return true;
            } else {
                int i = 1;
                while (line_value[i] != '\0') {
                    if (!std::isdigit(line_value[i])) {
                        return false;
                    }
                    i++;
                }
                return true;
            }
        } else if (TypeVar(line_value) == STRING) {
            if (IsString(line_value)) {
                return true;
            } else {
                return false;
            }
        } else if (TypeVar(line_value) == BOOL) {
            return true;
        } else if (TypeVar(line_value) == FLOAT) {
            if (line_value.front() == '+' || line_value.front() == '-') {
                if (line_value[1] == '.' || line_value.back() == '.') {
                    return false;
                } else {
                    return true;
                }
            }
        } else if (TypeVar(line_value) == ARRAY) {
            std::stack<char> stack;
            size_t quotes = 0;
            size_t bracket = 0;
            for (int i = 1; i < line_value.size() - 1; i++) {
                if (line_value[i] == '[' && quotes % 2 == 0) {
                    stack.push(line_value[i]);
                } else if (line_value[i] == '\"' && bracket == 0) {
                    quotes = quotes + 1;
                } else if (line_value[i] == ']' && quotes % 2 == 0) {
                    if (stack.top() == '[') {
                        stack.pop();
                    }
                } else if (line_value[i] == ';' && quotes % 2 == 0) {
                    return false;
                }
            }
            if (stack.empty()) {
                return true;
            } else {
                return false;
            }
        }
    }
    return true;
}

Array* omfl::ParseArray(std::string array, Array* final_array) {
    size_t quotes = 0;
    size_t brackets = 0;
    size_t start = 0;
    std::string variable;
    std::vector<Variable*> variables;
    for (int i = 1; i < array.size() - 1; i++) {
        if (array[i] == '[' && quotes % 2 == 0) {
            brackets = brackets + 1;
        } else if (array[i] == '\"' && brackets == 0) {
            quotes = quotes + 1;
        } else if (array[i] == ']' && quotes % 2 == 0) {
            brackets = brackets - 1;
        } else if ((array[i] == ',' && quotes % 2 == 0)) {
            if (brackets == 0) {
                variable = array.substr(start + 1, i - start - 1);
                start = i + 1;
                if (TypeVar(variable) == INT && variable.size() != 0) {
                    IntVar* int_var = new IntVar(std::stoi(variable));
                    variables.push_back(int_var);
                } else if (TypeVar(variable) == STRING) {
                    StringVar* string_var = new StringVar(variable.substr(1, variable.size() - 2));
                    variables.push_back(string_var);
                } else if (TypeVar(variable) == BOOL) {
                    if (variable == "true") {
                        BoolVar* bool_var = new BoolVar(true);
                        variables.push_back(bool_var);
                    } else {
                        BoolVar* bool_var = new BoolVar(false);
                        variables.push_back(bool_var);
                    }
                } else if (TypeVar(variable) == FLOAT) {
                    FloatVar* float_var = new FloatVar(std::stof(variable));
                    variables.push_back(float_var);
                } else if (TypeVar(variable) == ARRAY) {
                    Array* array1 = new Array();
                    Array* new_array = ParseArray(variable, array1);
                    variables.push_back(new_array);
                }
            }
        }
    }
    variable = array.substr(start + 1, array.size() - start - 2);
    if (TypeVar(variable) == INT && variable.size() != 0) {
        IntVar* int_var = new IntVar(std::stoi(variable));
        variables.push_back(int_var);
    } else if (TypeVar(variable) == STRING) {
        StringVar* string_var = new StringVar(variable.substr(1, variable.size() - 2));
        variables.push_back(string_var);
    } else if (TypeVar(variable) == BOOL) {
        if (variable == "true") {
            BoolVar* bool_var = new BoolVar(true);
            variables.push_back(bool_var);
        } else {
            BoolVar* bool_var = new BoolVar(false);
            variables.push_back(bool_var);
        }
    } else if (TypeVar(variable) == FLOAT) {
        FloatVar* float_var = new FloatVar(std::stof(variable));
        variables.push_back(float_var);
    } else if (TypeVar(variable) == ARRAY) {
        Array* array1 = new Array();
        Array* new_array = ParseArray(variable, array1);
        variables.push_back(new_array);
    }
    final_array = new Array(variables);
    return final_array;
}

Parser omfl::parse(const std::string& str) {
    Parser* parser = new Parser();
    Section* current_section = &parser->Global();
    std::istringstream file_stream(str);
    std::string line_value;
    while (std::getline(file_stream, line_value, '\n')) {
        DeleteWhiteSpaces(line_value);
        if (CheckElement(line_value) == EMPTY || CheckElement(line_value) == COMMENT) {
            continue;
        } else if (CheckElement(line_value) == UNKNOWN) {
            parser->SetValid();
            return *parser;
        } else if (CheckElement(line_value) == VARIABLE) {
            TakeToStr(line_value);
            std::pair<std::string, std::string> current_var = ParseVar(line_value);
            if (!CheckVarName(current_var.first) || !CheckVarValue(current_var.second)) {
                parser->SetValid();
                return *parser;
            } else {
                if (TypeVar(current_var.second) == INT) {
                    for (int i = 0; i < current_section->GetArr().size(); i++) {
                        if (current_section->GetArr()[i]->GetName() == current_var.first) {
                            parser->SetValid();
                            return *parser;
                        }
                    }
                    current_section->AddNewIntVar(current_var.first, std::stoi(current_var.second));
                } else if (TypeVar(current_var.second) == STRING) {
                    current_section->AddNewStringVar(current_var.first,
                                                     current_var.second.substr(1, current_var.second.size() - 2));
                } else if (TypeVar(current_var.second) == BOOL) {
                    if (current_var.second == "true") {
                        current_section->AddNewBoolVar(current_var.first, true);
                    } else {
                        current_section->AddNewBoolVar(current_var.first, false);
                    }
                } else if (TypeVar(current_var.second) == FLOAT) {
                    current_section->AddNewFloatVar(current_var.first, std::stof(current_var.second));
                } else if (TypeVar(current_var.second) == ARRAY) {
                    Array* array = new Array();
                    current_section->AddNewArray(current_var.first, *ParseArray(current_var.second, array));
                }
            }
        } else if (CheckElement(line_value) == SECTION) {
            line_value = line_value.substr(1, line_value.size() - 2);
            if (CheckSection(line_value)) {
                std::istringstream section_stream(line_value);
                std::string section_value;
                std::vector<std::string> sections;
                while (std::getline(section_stream, section_value, '.')) {
                    sections.push_back(section_value);
                }
                Section* this_section = &parser->Global();
                for (int i = 0; i < sections.size(); i++) {
                    bool flag = false;
                    for (int j = 0; j < parser->GetSectionList().size(); j++) {
                        if (parser->GetSectionList()[j]->GetName() == sections[i]) {
                            this_section = parser->GetSectionList()[j];
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        this_section = &parser->AddNewSection(sections[i], this_section->GetName());
                    }
                }
                current_section = this_section;
            } else {
                parser->SetValid();
                return *parser;
            }
        }
    }
    return *parser;
}



