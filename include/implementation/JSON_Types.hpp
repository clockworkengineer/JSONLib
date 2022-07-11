#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <stdexcept>
#include <limits>
#include <array>
#include <cstring>
#include <initializer_list>
#include <variant>
#include <iostream>
// =========
// NAMESPACE
// =========
namespace JSONLib
{
    // ==========
    // JSON Error
    // ==========
    struct Error : public std::runtime_error
    {
        explicit Error(const std::string &message) : std::runtime_error("JSON Error: " + message)
        {
        }
    };
    // ===========
    // JNode Types
    // ===========
    enum class JNodeType
    {
        base = 0,
        object,
        array,
        number,
        string,
        boolean,
        null,
        hole
    };
    // ============
    // JNode Number
    // ============
    struct JNodeNumeric
    {
        JNodeNumeric() = default;
        explicit JNodeNumeric(int integer)
        {
            std::string number{std::to_string(integer)};
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        explicit JNodeNumeric(long integer)
        {
            std::string number{std::to_string(integer)};
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        explicit JNodeNumeric(long long integer)
        {
            std::string number{std::to_string(integer)};
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        explicit JNodeNumeric(float floatingPoint)
        {
            std::string number{std::to_string(floatingPoint)};
            number.erase(number.find_last_not_of('0') + 1, std::string::npos);
            if (number.back() == '.')
                number += '0';
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        explicit JNodeNumeric(double floatingPoint)
        {
            std::string number{std::to_string(floatingPoint)};
            number.erase(number.find_last_not_of('0') + 1, std::string::npos);
            if (number.back() == '.')
                number += '0';
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        explicit JNodeNumeric(long double floatingPoint)
        {
            std::string number{std::to_string(floatingPoint)};
            number.erase(number.find_last_not_of('0') + 1, std::string::npos);
            if (number.back() == '.')
                number += '0';
            std::memcpy(&value[0], number.c_str(), number.size() + 1);
        }
        JNodeNumeric(const JNodeNumeric &other) = default;
        JNodeNumeric &operator=(const JNodeNumeric &other) = default;
        JNodeNumeric(JNodeNumeric &&other) = default;
        JNodeNumeric &operator=(JNodeNumeric &&other) = default;
        ~JNodeNumeric() = default;
        // Numeric values max width in characters
        static constexpr int kLongLongWidth = std::numeric_limits<long long>::digits10 + 2;
        static constexpr int kLongDoubleWidth = std::numeric_limits<long double>::digits10 + 2;
        // Numbers stored in fix length array
        std::array<char, kLongLongWidth> value;
    };
    // ==========
    // JNode Data 
    // ==========
    struct JNodeData
    {
        explicit JNodeData(JNodeType nodeType = JNodeType::base) : m_nodeType(nodeType)
        {
        }
        [[nodiscard]] JNodeType getNodeType() const
        {
            return (m_nodeType);
        }
        JNodeData(const JNodeData &other) = delete;
        JNodeData &operator=(JNodeData &other) = delete;
        JNodeData(JNodeData &&other) = default;
        JNodeData &operator=(JNodeData &&other) = default;
        ~JNodeData() = default;

    private:
        JNodeType m_nodeType;
    };
    // =====
    // JNode
    // =====
    struct JNode
    {
        // Pointer to JNode
        using Ptr = std::unique_ptr<JNode>;
        // JNode Error
        struct Error : public std::runtime_error
        {
            explicit Error(const std::string &message) : std::runtime_error("JNode Error: " + message)
            {
            }
        };
        // Constructors/Destructors
        JNode(std::unique_ptr<JNodeData> jNodeData) : m_jNodeData(std::move(jNodeData)) {}
        JNode(std::initializer_list<std::variant<int, long, long long, float, double, long double, bool, std::string, std::nullptr_t>> list);
        JNode(const JNode &other) = delete;
        JNode &operator=(const JNode &other) = delete;
        JNode(JNode &&other) = default;
        JNode &operator=(JNode &&other) = default;
        ~JNode() = default;
        // Assignment operators
        JNode &operator=(float floatingPoint);
        JNode &operator=(double floatingPoint);
        JNode &operator=(long double floatingPoint);
        JNode &operator=(int integer);
        JNode &operator=(long integer);
        JNode &operator=(long long integer);
        JNode &operator=(const char *cString);
        JNode &operator=(const std::string &string);
        JNode &operator=(bool boolean);
        JNode &operator=(std::nullptr_t null);
        // Indexing operators
        JNode &operator[](const std::string &key);
        const JNode &operator[](const std::string &key) const;
        JNode &operator[](std::size_t index);
        const JNode &operator[](std::size_t index) const;
        // Get JNode type
        [[nodiscard]] JNodeType getNodeType() const
        {
            return (m_jNodeData->getNodeType());
        }
        // Get reference to JNodeData
        std::unique_ptr<JNodeData> &getJNodeData()
        {
            return (m_jNodeData);
        }
        const std::unique_ptr<JNodeData> &getJNodeData() const
        {
            return (m_jNodeData);
        }

    private:
        std::unique_ptr<JNodeData> m_jNodeData;
    };
    // ==================
    // JNode Object Entry
    // ==================
    struct JNodeObjectEntry
    {
        std::string key;
        JNode::Ptr value;
    };
    // ===================
    // JNode Data Variants
    // ===================
    // Object
    struct JNodeObject : JNodeData
    {
        using ObjectEntryList = std::vector<JNodeObjectEntry>;
        // Constructors/Destructors
        JNodeObject() : JNodeData(JNodeType::object) {}
        JNodeObject(ObjectEntryList &objects) : JNodeData(JNodeType::object), m_jsonObjects(std::move(objects)) {}
        JNodeObject(const JNodeObject &other) = delete;
        JNodeObject &operator=(const JNodeObject &other) = delete;
        JNodeObject(JNodeObject &&other) = default;
        JNodeObject &operator=(JNodeObject &&other) = default;
        ~JNodeObject() = default;
        // Search for a given entry given a key and object list
        static auto findKey(const std::string &key,
                            const ObjectEntryList &objects)
        {
            auto entry = std::find_if(objects.begin(), objects.end(), [&key](const JNodeObjectEntry &entry) -> bool
                                      { return (entry.key == key); });
            if (entry == objects.end())
            {
                throw JNode::Error("Invalid key used to access object.");
            }
            return (entry);
        }
        // Find a given object entry given its key
        [[nodiscard]] auto find(const std::string &key) const
        {
            return (findKey(key, m_jsonObjects));
        }
        // Return true if an object contains a given key
        [[nodiscard]] bool contains(const std::string &key) const
        {
            try
            {
                [[maybe_unused]] auto entry = findKey(key, m_jsonObjects);
            }
            catch ([[maybe_unused]] const JNode::Error &e)
            {
                return (false);
            }
            return (true);
        }
        // Return number of entries in an object
        [[nodiscard]] int size() const
        {
            return (static_cast<int>(m_jsonObjects.size()));
        }
        // Return object entry for a given key
        JNode &operator[](const std::string &key)
        {
            return (*(findKey(key, m_jsonObjects)->value));
        }
        const JNode &operator[](const std::string &key) const
        {
            return (*(findKey(key, m_jsonObjects)->value));
        }
        // Return reference to base of object entries
        ObjectEntryList &objects()
        {
            return (m_jsonObjects);
        }
        [[nodiscard]] const ObjectEntryList &objects() const
        {
            return (m_jsonObjects);
        }

    private:
        ObjectEntryList m_jsonObjects;
    };
    // Array
    struct JNodeArray : JNodeData
    {
        // Constructors/Destructors
        JNodeArray() : JNodeData(JNodeType::array) {}
        JNodeArray(std::vector<JNode::Ptr> &array) : JNodeData(JNodeType::array),
                                                         m_jsonArray(std::move(array)) {}
        JNodeArray(const JNodeArray &other) = delete;
        JNodeArray &operator=(const JNodeArray &other) = delete;
        JNodeArray(JNodeArray &&other) = default;
        JNodeArray &operator=(JNodeArray &&other) = default;
        ~JNodeArray() = default;
        // Return the size of array
        [[nodiscard]] std::size_t size() const
        {
            return (m_jsonArray.size());
        }
        // Return reference to array base
        std::vector<JNode::Ptr> &array()
        {
            return (m_jsonArray);
        }
        [[nodiscard]] const std::vector<JNode::Ptr> &array() const
        {
            return (m_jsonArray);
        }
        // Array indexing operators
        JNode &operator[](std::size_t index)
        {
            if ((index >= 0) && (index < (static_cast<int>(m_jsonArray.size()))))
            {
                return (*m_jsonArray[index]);
            }
            throw JNode::Error("Invalid index used to access array.");
        }
        const JNode &operator[](std::size_t index) const
        {
            if ((index >= 0) && (index < (static_cast<int>(m_jsonArray.size()))))
            {
                return (*m_jsonArray[index]);
            }
            throw JNode::Error("Invalid index used to access array.");
        }

    private:
        std::vector<JNode::Ptr> m_jsonArray;
    };
    // Number
    struct JNodeNumber : JNodeData
    {
        // Constructors/Destructors
        JNodeNumber() : JNodeData(JNodeType::number) {}
        explicit JNodeNumber(JNodeNumeric &number) : JNodeData(JNodeType::number), m_jsonNumber(std::move(number)) {}
        JNodeNumber(const JNodeNumber &other) = delete;
        JNodeNumber &operator=(const JNodeNumber &other) = delete;
        JNodeNumber(JNodeNumber &&other) = default;
        JNodeNumber &operator=(JNodeNumber &&other) = default;
        ~JNodeNumber() = default;
        // Is character a valid numeric character ?
        bool isValidNumeric(char ch) const
        {
            // Includes possible sign, decimal point or exponent
            return ((std::isdigit(ch) != 0) || ch == '.' || ch == '-' || ch == '+' || ch == 'E' || ch == 'e');
        }
        // Convert to long long returning true on success
        // Note: Can still return a long value for floating point
        // but false as the number is not in integer format
        [[nodiscard]] bool integer(long long &longValue) const
        {
            char *end = nullptr;
            longValue = std::strtoll(&m_jsonNumber.value[0], &end, 10);
            return (*end == '\0'); // If not all characters used then not success
        }
        // Convert to long double returning true on success
        [[nodiscard]] bool floatingpoint(long double &doubleValue) const
        {
            char *end = nullptr;
            doubleValue = std::strtod(&m_jsonNumber.value[0], &end);
            return (*end == '\0'); // If not all characters used then not success
        }
        // Check whether we nave a numeric value
        [[nodiscard]] bool isValidNumber() const
        {
            if ([[maybe_unused]] long long longValue{}; integer(longValue))
            {
                return (true);
            }
            if ([[maybe_unused]] long double doubleValue{}; floatingpoint(doubleValue))
            {
                return (true);
            }
            return (false);
        }
        // Return reference to number character array
        [[nodiscard]] JNodeNumeric &number()
        {
            return (m_jsonNumber);
        }
        [[nodiscard]] const JNodeNumeric &number() const
        {
            return (m_jsonNumber);
        }
        // Convert number to string
        [[nodiscard]] std::string toString() const
        {
            return (std::string{m_jsonNumber.value.begin(), m_jsonNumber.value.begin() + std::strlen(&m_jsonNumber.value[0])});
        }

    private:
        JNodeNumeric m_jsonNumber{};
    };
    // String
    struct JNodeString : JNodeData
    {
        // Constructors/Destructors
        JNodeString() : JNodeData(JNodeType::string) {}
        JNodeString(const std::string &string) : JNodeData(JNodeType::string),
                                                     m_jsonString(std::move(string)) {}
        JNodeString(const JNodeString &other) = delete;
        JNodeString &operator=(const JNodeString &other) = delete;
        JNodeString(JNodeString &&other) = default;
        JNodeString &operator=(JNodeString &&other) = default;
        ~JNodeString() = default;
        // Return reference to string
        std::string &string()
        {
            return (m_jsonString);
        }
        [[nodiscard]] const std::string &string() const
        {
            return (m_jsonString);
        }
        // Convert string representation to a string
        [[nodiscard]] std::string toString() const
        {
            return (m_jsonString);
        }

    private:
        std::string m_jsonString;
    };
    // Boolean
    struct JNodeBoolean : JNodeData
    {
        // Constructors/Destructors
        JNodeBoolean() : JNodeData(JNodeType::boolean) {}
        JNodeBoolean(bool boolean) : JNodeData(JNodeType::boolean), m_jsonBoolean(boolean) {}
        JNodeBoolean(const JNodeBoolean &other) = delete;
        JNodeBoolean &operator=(const JNodeBoolean &other) = delete;
        JNodeBoolean(JNodeBoolean &&other) = default;
        JNodeBoolean &operator=(JNodeBoolean &&other) = default;
        ~JNodeBoolean() = default;
        // Return boolean value
        [[nodiscard]] bool boolean() const
        {
            return (m_jsonBoolean);
        }
        // Return string representation of boolean value
        [[nodiscard]] std::string toString() const
        {
            return (m_jsonBoolean ? "true" : "false");
        }

    private:
        bool m_jsonBoolean;
    };
    // Null
    struct JNodeNull : JNodeData
    {
        // Constructors/Destructors
        JNodeNull() : JNodeData(JNodeType::null) {}
        JNodeNull(const JNodeNull &other) = delete;
        JNodeNull &operator=(const JNodeNull &other) = delete;
        JNodeNull(JNodeNull &&other) = default;
        JNodeNull &operator=(JNodeNull &&other) = default;
        ~JNodeNull() = default;
        // Return null value
        [[nodiscard]] void *null() const
        {
            return (nullptr);
        }
        // Return string representation of null value
        [[nodiscard]] std::string toString() const
        {
            return ("null");
        }
    };
    // Hole
    struct JNodeHole : JNodeData
    {
        // Constructors/Destructors
        JNodeHole() : JNodeData(JNodeType::hole) {}
        JNodeHole(const JNodeHole &other) = delete;
        JNodeHole &operator=(const JNodeHole &other) = delete;
        JNodeHole(JNodeHole &&other) = default;
        JNodeHole &operator=(JNodeHole &&other) = default;
        ~JNodeHole() = default;
        [[nodiscard]] std::string toString() const
        {
            return ("null");
        }
    };
    // ==============
    // JNode Creation
    // ==============
    inline std::unique_ptr<JNode> makeJNodeObject(JNodeObject::ObjectEntryList &objects)
    {
        return (std::make_unique<JNode>(JNode {std::make_unique<JNodeObject>(JNodeObject{objects})}));
    }
    inline std::unique_ptr<JNode> makeJNodeArray(std::vector<JNode::Ptr> &array)
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeArray>(JNodeArray{array})}));
    }
    inline std::unique_ptr<JNode> makeJNodeNumber(JNodeNumeric &number)
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeNumber>(JNodeNumber{number})}));
    }
    inline std::unique_ptr<JNode> makeJNodeString(const std::string &string)
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeString>(JNodeString{string})}));
    }
    inline std::unique_ptr<JNode> makeJNodeBoolean(bool boolean)
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeBoolean>(JNodeBoolean{boolean})}));
    }
    inline std::unique_ptr<JNode> makeJNodeNull()
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeNull>(JNodeNull())}));
    }
    inline std::unique_ptr<JNode> makeJNodeHole()
    {
        return (std::make_unique<JNode>(JNode{std::make_unique<JNodeHole>(JNodeHole())}));
    }
    // ==================
    // JNode constructors
    // ==================
    inline JNode::JNode(std::initializer_list<std::variant<int, long, long long, float, double, long double, bool, std::string, std::nullptr_t>> list)
    {
        JNodeArray array;
        for (auto &entry : list)
        {
            if (const int *pint = std::get_if<int>(&entry))
            {
                JNodeNumeric jNodeNumber{*pint};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const long *plong = std::get_if<long>(&entry))
            {
                JNodeNumeric jNodeNumber{*plong};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const long long *plonglong = std::get_if<long long>(&entry))
            {
                JNodeNumeric jNodeNumber{*plonglong};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const float *pfloat = std::get_if<float>(&entry))
            {
                JNodeNumeric jNodeNumber{*pfloat};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const double *pdouble = std::get_if<double>(&entry))
            {
                JNodeNumeric jNodeNumber{*pdouble};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const long double *plongdouble = std::get_if<long double>(&entry))
            {
                JNodeNumeric jNodeNumber{*plongdouble};
                array.array().emplace_back(makeJNodeNumber(jNodeNumber));
            }
            else if (const std::string *pstring = std::get_if<std::string>(&entry))
            {
                array.array().emplace_back(makeJNodeString(*pstring));
            }
            else if (const bool *pboolean = std::get_if<bool>(&entry))
            {
                array.array().emplace_back(makeJNodeBoolean(*pboolean));
            }
            else if (const std::nullptr_t *pnull = std::get_if<std::nullptr_t>(&entry))
            {
                array.array().emplace_back(makeJNodeNull());
            }
        }
        m_jNodeData = std::make_unique<JNodeArray>(std::move(array));
    }
    // ==============================
    // JNode base reference converter
    // ==============================
    template <typename T>
    void CheckJNodeType(const JNodeData &jNodeData)
    {
        if constexpr (std::is_same_v<T, JNodeString>)
        {
            if (jNodeData.getNodeType() != JNodeType::string)
            {
                throw JNode::Error("Node not a string.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeNumber>)
        {
            if (jNodeData.getNodeType() != JNodeType::number)
            {
                throw JNode::Error("Node not a number.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeArray>)
        {
            if (jNodeData.getNodeType() != JNodeType::array)
            {
                throw JNode::Error("Node not an array.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeObject>)
        {
            if (jNodeData.getNodeType() != JNodeType::object)
            {
                throw JNode::Error("Node not an object.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeBoolean>)
        {
            if (jNodeData.getNodeType() != JNodeType::boolean)
            {
                throw JNode::Error("Node not an boolean.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeNull>)
        {
            if (jNodeData.getNodeType() != JNodeType::null)
            {
                throw JNode::Error("Node not a null.");
            }
        }
    }
    template <typename T>
    T &JNodeRef(JNode &jNode)
    {
        CheckJNodeType<T>(*jNode.getJNodeData());
        return (static_cast<T &>(*jNode.getJNodeData()));
    }
    template <typename T>
    const T &JNodeRef(const JNode &jNode)
    {
        CheckJNodeType<T>(*jNode.getJNodeData());
        return (static_cast<const T &>(*jNode.getJNodeData()));
    }
    // =====================
    // JNode index overloads
    // =====================
    // Object
    inline JNode &JNode::operator[](const std::string &key)
    {
        if (this->getNodeType() == JNodeType::hole)
        {
            this->m_jNodeData = std::make_unique<JNodeObject>();
            JNodeRef<JNodeObject>(*this).objects().emplace_back(JNodeObjectEntry{key, makeJNodeHole()});
            return (*JNodeRef<JNodeObject>(*this).objects().back().value);
        }
        return (JNodeRef<JNodeObject>(*this)[key]);
    }
    inline const JNode &JNode::operator[](const std::string &key) const
    {
        return (JNodeRef<const JNodeObject>(*this)[key]);
    }
    // Array
    inline JNode &JNode::operator[](std::size_t index)
    {
        try
        {
            if (this->getNodeType() == JNodeType::hole)
            {
                this->m_jNodeData = std::make_unique<JNodeArray>();
            }
            return (JNodeRef<JNodeArray>(*this)[index]);
        }
        catch ([[maybe_unused]] const JNode::Error &error)
        {
            JNodeRef<JNodeArray>(*this).array().resize(index + 1);
            JNodeRef<JNodeArray>(*this).array()[index] = std::move(makeJNodeHole());
            for (auto &entry : JNodeRef<JNodeArray>(*this).array())
            {
                if (entry.get() == nullptr)
                {
                    entry = std::move(makeJNodeHole());
                }
            }
            return (JNodeRef<JNodeArray>(*this)[index]);
        }
    }
    inline const JNode &JNode::operator[](std::size_t index) const
    {
        return (JNodeRef<JNodeArray>(*this)[index]);
    }
    // ==========================
    // JNode assignment operators
    // ==========================
    inline JNode &JNode::operator=(float floatingPoint)
    {
        JNodeNumeric jNodeNumber{floatingPoint};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(double floatingPoint)
    {
        JNodeNumeric jNodeNumber{floatingPoint};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(long double floatingPoint)
    {
        JNodeNumeric jNodeNumber{floatingPoint};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(int integer)
    {
        JNodeNumeric jNodeNumber{integer};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(long integer)
    {
        JNodeNumeric jNodeNumber{integer};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(long long integer)
    {
        JNodeNumeric jNodeNumber{integer};
        m_jNodeData = std::make_unique<JNodeNumber>(std::move(JNodeNumber{jNodeNumber}));
        return (*this);
    }
    inline JNode &JNode::operator=(const char *cString)
    {
        m_jNodeData = std::make_unique<JNodeString>(std::move(JNodeString{cString}));
        return (*this);
    }
    inline JNode &JNode::operator=(const std::string &string)
    {
        m_jNodeData = std::make_unique<JNodeString>(std::move(JNodeString{string}));
        return (*this);
    }
    inline JNode &JNode::operator=(bool boolean)
    {
        m_jNodeData = std::make_unique<JNodeBoolean>(std::move(JNodeBoolean{boolean}));
        return (*this);
    }
    inline JNode &JNode::operator=([[maybe_unused]] std::nullptr_t null)
    {
        m_jNodeData = std::make_unique<JNodeNull>(std::move(JNodeNull()));
        return (*this);
    }
} // namespace JSONLib