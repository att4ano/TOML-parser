#pragma once

#include <filesystem>
#include <istream>
#include <utility>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <stack>


namespace omfl {

    enum ELEMENT {
        VARIABLE = 1,
        SECTION,
        COMMENT,
        UNKNOWN,
        EMPTY
    };

    enum TYPE {
        INT = 1,
        STRING,
        BOOL,
        FLOAT,
        ARRAY,
        UNDEFINED
    };

    class Variable;

    class Element {
    protected:
        std::string name_;
        ELEMENT type_element;

    public:
        std::string GetName();

        Element& Get(std::string name_variable);

        virtual ~Element() = default;

        virtual Variable& operator[](int index);

        bool IsInt();

        bool IsString();

        bool IsFloat();

        bool IsBool();

        bool IsArray();

        virtual int AsInt();

        virtual int AsIntOrDefault(int default_value);

        virtual std::string AsString();

        virtual std::string AsStringOrDefault(std::string default_value);

        virtual bool AsBool();

        virtual float AsFloat();

        virtual float AsFloatOrDefault(float default_value);
    };

    class Array;

    class Variable : public Element {
    protected:
        std::string name_ = "variable";

        TYPE type_ = UNDEFINED;

        Variable() = default;

        virtual ~Variable() = default;

    public:

        std::string GetName() {
            return name_;
        }

        void SetName(std::string name) {
            name_ = name;
        }

        virtual Variable& operator[](int index);

        bool IsInt() {
            if (this->type_ == INT) {
                return true;
            } else {
                return false;
            }
        }

        bool IsString() {
            if (this->type_ == STRING) {
                return true;
            } else {
                return false;
            }
        }

        bool IsFloat() {
            if (this->type_ == FLOAT) {
                return true;
            } else {
                return false;
            }
        }

        bool IsBool() {
            if (this->type_ == BOOL) {
                return true;
            } else {
                return false;
            }
        }

        bool IsArray() {
            if (this->type_ == ARRAY) {
                return true;
            } else {
                return false;
            }
        }

        int AsInt() override;

        int AsIntOrDefault(int default_value);

        std::string AsString();

        std::string AsStringOrDefault(std::string default_value);

        bool AsBool();

        float AsFloat();

        float AsFloatOrDefault(float default_value);
    };

    class IntVar : public Variable {
    protected:
        int value_;
    public:
        explicit IntVar(int value, std::string name) {
            name_ = std::move(name);
            value_ = value;
            type_ = INT;
            type_element = VARIABLE;
        }

        explicit IntVar(int value) {
            value_ = value;
            type_ = INT;
            type_element = VARIABLE;
        }

        IntVar& operator=(IntVar& other) {
            name_ = other.name_;
            value_ = other.value_;
            type_ = INT;
            type_element = VARIABLE;
            return *this;
        }

        [[nodiscard]] int GetValue() const {
            return value_;
        }
    };

    class StringVar : public Variable {
    protected:
        std::string value_;
    public:
        explicit StringVar(std::string value) {
            value_ = std::move(value);
            type_ = STRING;
            type_element = VARIABLE;
        }

        explicit StringVar(std::string value, std::string name) {
            name_ = std::move(name);
            value_ = std::move(value);
            type_ = STRING;
            type_element = VARIABLE;
        }

        StringVar& operator=(StringVar& other) {
            name_ = other.name_;
            value_ = other.value_;
            type_ = STRING;
            type_element = VARIABLE;
            return *this;
        }

        [[nodiscard]] std::string GetValue() const {
            return value_;
        }
    };

    class BoolVar : public Variable {
    protected:
        bool value_;
    public:
        explicit BoolVar(bool value) {
            value_ = value;
            type_ = BOOL;
            type_element = VARIABLE;
        }

        explicit BoolVar(bool value, std::string name) {
            name_ = std::move(name);
            value_ = value;
            type_ = BOOL;
            type_element = VARIABLE;
        }

        BoolVar& operator=(BoolVar& other) {
            name_ = other.name_;
            value_ = other.value_;
            type_ = BOOL;
            type_element = VARIABLE;
            return *this;
        }

        [[nodiscard]] bool GetValue() const {
            return value_;
        }
    };

    class FloatVar : public Variable {
    protected:
        float value_;
    public:
        explicit FloatVar(float value) {
            value_ = value;
            type_ = FLOAT;
            type_element = VARIABLE;
        }

        explicit FloatVar(float value, std::string name) {
            name_ = std::move(name);
            value_ = value;
            type_ = FLOAT;
            type_element = VARIABLE;
        }

        FloatVar& operator=(FloatVar& other) {
            name_ = other.name_;
            value_ = other.value_;
            type_ = FLOAT;
            type_element = VARIABLE;
            return *this;
        }

        [[nodiscard]] float GetValue() const {
            return value_;
        }
    };

    class Array : public Variable {
    protected:
        std::vector<Variable*> var_array;
    public:
        Array() {
            type_ = ARRAY;
            type_element = VARIABLE;
        }

        explicit Array(std::vector<Variable*>& array) {
            var_array = array;
            type_ = ARRAY;
            type_element = VARIABLE;
        }

        explicit Array(std::string name) {
            name_ = std::move(name);
            type_ = ARRAY;
            type_element = VARIABLE;
        }

        Array& operator=(Array const& other) {
            name_ = other.name_;
            var_array = other.var_array;
            type_ = ARRAY;
            type_element = VARIABLE;
            return *this;
        }

        Variable& operator[](int index) {
            if (index >= var_array.size()) {
                BoolVar* new_var = new BoolVar(false);
                return *new_var;
            } else {
                return *var_array[index];
            }
        }
    };


    class Section : public Element {
        std::string name_ = "global";
        std::vector<Variable*> var_list;
        Section* parent_section = nullptr;
        std::vector<Section*> child_section;
    public:
        Section() = default;

        std::string GetName() {
            return name_;
        }

        explicit Section(std::string name_section) {
            name_ = name_section;
            type_element = SECTION;
        }

        explicit Section(std::string name_section, Section* parent) {
            name_ = name_section;
            this->parent_section = parent;
            type_element = SECTION;
        }

        Element& Get(std::string name_variable);

        std::vector<Section*>& GetSectionChild() {
            return child_section;
        }

        Section& operator=(const Section& other) {
            name_ = other.name_;
            var_list = other.var_list;
            parent_section = other.parent_section;
            child_section = other.child_section;
            type_element = SECTION;
            return *this;
        }

        Section& SetChild(Section* child) {
            child_section.push_back(child);
            return *child_section.back();
        }

        std::vector<Variable*>& GetArr() {
            return var_list;
        }

        void AddNewIntVar(std::string& name, int value);

        void AddNewStringVar(std::string& name, std::string value);

        void AddNewBoolVar(std::string& name, bool value);

        void AddNewFloatVar(std::string& name, float value);

        void AddNewArray(std::string name, Array& array);
    };


    class Parser {
        std::string name;
        Section global_section = Section();
        std::vector<Section*> section_list = {&global_section};
        bool is_valid = true;
        std::string path_;
    public:
        Parser() {
            name = "my new parser";
        }

        [[nodiscard]] std::vector<Section*> GetSectionList() const {
            return this->section_list;
        }

        [[nodiscard]] bool valid() const {
            return is_valid;
        }

        Section& Global() {
            return global_section;
        }

        Section& AddNewSection(std::string& name, std::string parent_name);

        [[nodiscard]] Element& Get(std::string name_variable) const;

        void SetValid() {
            this->is_valid = false;
        }

        void SetPath(const std::string& path){
            path_ = path;
        }

        std::string GetPath(){
            return path_;
        }
    };

    void TakeToStr(std::string& line);

    void DeleteWhiteSpaces(std::string& line);

    std::pair<std::string, std::string> ParseVar(std::string current_var);

    ELEMENT CheckElement(std::string current_line);

    Parser parse(const std::filesystem::path& path);

    Parser parse(const std::string& str);

    bool CheckVarName(std::string var_name);

    bool CheckVarValue(std::string var_value);

    TYPE TypeVar(std::string line_value);

    Array* ParseArray(std::string array, Array* final_array);

    bool CheckSection(std::string section);
}// namespace