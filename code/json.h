#ifndef JSON_UPDATED_JSON_H
#define JSON_UPDATED_JSON_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <fstream>

namespace json {

class Void {
};

enum JsonType {
    JSON_NULL = 0,
    JSON_BOOL,
    JSON_INT,
    JSON_FLOAT,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
};

class Json;

class JsonView;

class JsonParser;

extern const std::vector<std::string> JSON_TYPE_TO_STRING;

typedef std::variant<
        Void,
        bool,
        long long,
        double,
        std::string,
        std::vector<Json>,
        std::map<std::string, Json>
> JsonBasic;

class Json {
private:
    std::shared_ptr<JsonBasic> m_Value;

public:
    friend class JsonParser;

    Json();
    Json(const JsonType& tp);
    Json(const bool& bl);
    Json(const int& it);
    Json(const double& dbl);
    Json(const std::string& str);
    Json(const char* ch);
    Json(const Json& js);
    Json(Json&& js) noexcept;

    ~Json();

    Json& operator=(const bool& bl);
    Json& operator=(const int& it);
    Json& operator=(const char& ch);
    Json& operator=(const char* chPtr);
    Json& operator=(const std::string& str);
    Json& operator=(const Json& js);
    Json& operator=(Json&& js) noexcept;

    Json& operator[](unsigned int n);
    Json& operator[](const std::string& str);
    Json& operator[](const char* chPtr);

    explicit operator bool();
    explicit operator int();
    explicit operator long long();
    explicit operator float();
    explicit operator double();
    operator std::string();

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] unsigned int size() const;
    void clear();
    void copy(const Json& js);
    [[nodiscard]] JsonType get_type() const;
    void set_type(JsonType ttp);
    [[nodiscard]] std::string str() const;

    //ARRAY action
    void resize(unsigned int new_size);
    void push_back(const Json& js);
    [[nodiscard]] unsigned int capacity() const;

    //OBJECT action
    void erase(const std::string& str);
    void erase(const char* chPtr);
    void merge(const Json& js);

    class iterator {
    private:
        std::variant<std::vector<Json>::iterator,
                std::map<std::string, Json>::iterator> m_Value;
    public:
        friend class Json;

        iterator() = default;
        iterator(iterator&) = default;
        iterator(const iterator&) = default;
        iterator(iterator&&) = default;
        ~iterator() = default;

        iterator& operator++();
        iterator operator++(int);


        std::string key();
        Json& value();
    };

    iterator begin();
    iterator end();
};

class JsonParser {
private:
    std::string_view m_Str;
    unsigned long long m_Idx;
    Json m_Json;
    void read_token_char();
    void skip_white_space();

    Json parse_json();
    Json parse_null();
    Json parse_bool();
    Json parse_number();
    std::string parse_string();
    Json parse_array();
    Json parse_object();

public:
    JsonParser() = delete;
    JsonParser(const std::string& str);
    operator Json();
};

}

#endif //JSON_UPDATED_JSON_H
