#ifndef JSON__JSON_H_
#define JSON__JSON_H_

#include <string>
#include <vector>
#include <map>

namespace ns_json
{
class Json;

enum JsonType {
    JSON_NULL = 0,
    JSON_BOOL,
    JSON_INT,
    JSON_FLOAT,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
};

extern const std::vector<std::string> JSON_TYPE_TO_STRING;
extern const std::map<std::string, JsonType> STRING_TO_JSON_TYPE;

namespace privacy
{
    struct JsonBasic {
        unsigned int quoteTimes = 0;
        JsonType type;

        union JsonBasic_Union {
            bool bl{};
            int it;
            double dbl;
            std::string str;
            std::vector<Json> arr;
            std::map<std::string, Json> obj;
            JsonBasic_Union();
            ~JsonBasic_Union();
        } value;

        void conversion_type(JsonType ttp);
        JsonBasic();
        JsonBasic(JsonType tp);
        JsonBasic(const bool &bl);
        JsonBasic(const int &it);
        JsonBasic(const double &dbl);
        JsonBasic(const char &ch);
        JsonBasic(const char *&chPtr);
        JsonBasic(const std::string &str);
        ~JsonBasic();

        void clear();
    };
}

class Json {
private:
    privacy::JsonBasic *m_Value;
    void only_delete();
    void copy_highest_layer(const privacy::JsonBasic &jsb);
    void self_separate();
    void copy_highest_layer(const Json &js);

    friend class JsonParser;

public:
    Json();
    Json(const JsonType &tp);
    Json(const bool &bl);
    Json(const int &it);
    Json(const double &dbl);
    Json(const std::string &str);
    Json(const char *&ch);
    Json(const Json &js);
    ~Json();

    [[nodiscard]] JsonType type() const;
    [[nodiscard]] std::string type_str() const;
    [[nodiscard]] std::string str() const;
    void clear();
    void copy(const Json &js);
    void swap(Json &js) noexcept;
    void conversion_type(JsonType ttp);
    unsigned int quote_times();

    explicit operator bool();
    explicit operator int();
    explicit operator double();
    explicit operator std::string();

    Json &operator[](const int &n);
    Json &operator[](const std::string &str);
    Json &operator[](const char *chPtr);

    Json operator[](const int &n) const;
    Json operator[](const std::string &str) const;
    Json operator[](const char *chPtr) const;

    Json &operator=(const bool &bl);
    Json &operator=(const int &it);
    Json &operator=(const char &ch);
    Json &operator=(const char *chPtr);
    Json &operator=(const std::string &str);
    Json &operator=(const Json &js);

    //JSON_ARRAY action
    void push_back(const Json &js);
    Json &at(const int &n);

    //JSON_OBJECT action
    Json &at(const std::string &str);
    Json &at(const char *chPtr);
};


class JsonParser {
private:
    std::string_view m_Str;
    unsigned long long m_Idx;
    Json m_Json;
    void read_token_char();
    char peek_token_char();
    bool is_white_space();
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
    JsonParser(const std::string &str);
    operator Json();
};
}

#endif //JSON__JSON_H_
