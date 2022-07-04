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
    // ======================================
    // Numeric values max width in characters
    // ======================================
    static constexpr int kLongLongWidth = std::numeric_limits<long long>::digits10 + 2;
    static constexpr int kLongDoubleWidth = std::numeric_limits<long double>::digits10 + 2;
    // ===========
    // JNode types
    // ===========
    enum class JNodeType
    {
        base = 0,
        object,
        array,
        number,
        string,
        boolean,
        null
    };
    // ====
    // Base
    // ====
    struct JNode
    {
        using Ptr = std::unique_ptr<JNode>;
        struct Error : public std::runtime_error
        {
            explicit Error(const std::string &message) : std::runtime_error("JNode Error: " + message)
            {
            }
        };
        explicit JNode(JNodeType nodeType = JNodeType::base) : m_nodeType(nodeType)
        {
        }
        [[nodiscard]] JNodeType getNodeType() const
        {
            return (m_nodeType);
        }
        // No JNode is deleted through its base class so omit and save space
        // from virtual function table.
        // virtual ~JNode() = default;
        JNode &operator[](const std::string &key);
        const JNode &operator[](const std::string &key) const;
        JNode &operator[](int index);
        const JNode &operator[](int index) const;

    private:
        JNodeType m_nodeType;
    };
    // ==========
    // Dictionary
    // ==========
    struct JNodeObject : JNode
    {
        using Entry = std::pair<std::string, JNode::Ptr>;
        static auto findEntry(const std::string &key, const std::vector<Entry> &objects)
        {
            return (std::find_if(objects.begin(), objects.end(), [&key](const Entry &entry) -> bool
                                 { return (entry.first == key); }));
        }
        JNodeObject() : JNode(JNodeType::object)
        {
        }
        explicit JNodeObject(std::vector<JNodeObject::Entry> &objects) : JNode(JNodeType::object),
                                                                         m_objects(std::move(objects))
        {
        }
        [[nodiscard]] bool contains(const std::string &key) const
        {
            if (auto entry = findEntry(key, m_objects); entry != m_objects.end())
            {
                return (true);
            }
            return (false);
        }
        [[nodiscard]] int size() const
        {
            return (static_cast<int>(m_objects.size()));
        }
        JNode &operator[](const std::string &key)
        {
            if (auto entry = findEntry(key, m_objects); entry != m_objects.end())
            {
                return (*entry->second);
            }
            throw Error("Invalid key used to access object.");
        }
        const JNode &operator[](const std::string &key) const
        {
            if (auto entry = findEntry(key, m_objects); entry != m_objects.end())
            {
                return (*entry->second);
            }
            throw Error("Invalid key used to access object.");
        }
        std::vector<Entry> &objects()
        {
            return (m_objects);
        }
        [[nodiscard]] const std::vector<Entry> &objects() const
        {
            return (m_objects);
        }

    private:
        std::vector<JNodeObject::Entry> m_objects;
    };
    // =====
    // Array
    // =====
    struct JNodeArray : JNode
    {
        JNodeArray() : JNode(JNodeType::array)
        {
        }
        explicit JNodeArray(std::vector<JNode::Ptr> &array) : JNode(JNodeType::array),
                                                              m_array(std::move(array))
        {
        }
        [[nodiscard]] int size() const
        {
            return (static_cast<int>(m_array.size()));
        }
        std::vector<JNode::Ptr> &array()
        {
            return (m_array);
        }
        [[nodiscard]] const std::vector<JNode::Ptr> &array() const
        {
            return (m_array);
        }
        JNode &operator[](int index)
        {
            if ((index >= 0) && (index < (static_cast<int>(m_array.size()))))
            {
                return (*m_array[index]);
            }
            throw Error("Invalid index used to access array.");
        }
        const JNode &operator[](int index) const
        {
            if ((index >= 0) && (index < (static_cast<int>(m_array.size()))))
            {
                return (*m_array[index]);
            }
            throw Error("Invalid index used to access array.");
        }

    private:
        std::vector<JNode::Ptr> m_array;
    };
    // ======
    // Number
    // ======
    struct JNodeNumber : JNode
    {
        JNodeNumber() : JNode(JNodeType::number)
        {
        }
        // Convert to long long returning true on success
        // Note: Can still return a long value for floating point
        // but false as the number is not in integer format
        [[nodiscard]] bool integer(long long &longValue) const
        {
            char *end = nullptr;
            longValue = std::strtoll(&m_number[0], &end, 10);
            return (*end == '\0'); // If not all characters used then not success
        }
        // Convert to long double returning true on success
        [[nodiscard]] bool floatingpoint(long double &doubleValue) const
        {
            char *end = nullptr;
            doubleValue = std::strtod(&m_number[0], &end);
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
        [[nodiscard]] std::array<char, kLongLongWidth> &number()
        {
            return (m_number);
        }
        [[nodiscard]] const std::array<char, kLongLongWidth> &number() const
        {
            return (m_number);
        }
        [[nodiscard]] std::string toString() const
        {
            return (std::string{m_number.begin(), m_number.begin() + std::strlen(&m_number[0])});
        }
        [[nodiscard]] bool isValidNumeric(char ch)
        {
            // Includes possible sign, decimal point or exponent
            return ((std::isdigit(ch) != 0) || ch == '.' || ch == '-' || ch == '+' || ch == 'E' || ch == 'e');
        }

    private:
        std::array<char, kLongLongWidth> m_number{};
    };
    // ======
    // String
    // ======
    struct JNodeString : JNode
    {
        JNodeString() : JNode(JNodeType::string)
        {
        }
        explicit JNodeString(std::string str) : JNode(JNodeType::string), m_string(std::move(str))
        {
        }
        std::string &string()
        {
            return (m_string);
        }
        [[nodiscard]] const std::string &string() const
        {
            return (m_string);
        }
        [[nodiscard]] std::string toString() const
        {
            return (m_string);
        }

    private:
        std::string m_string;
    };
    // =======
    // Boolean
    // =======
    struct JNodeBoolean : JNode
    {
        explicit JNodeBoolean(bool boolean) : JNode(JNodeType::boolean), m_boolean(boolean)
        {
        }
        [[nodiscard]] bool boolean() const
        {
            return (m_boolean);
        }
        [[nodiscard]] std::string toString() const
        {
            return (m_boolean ? "true" : "false");
        }

    private:
        bool m_boolean;
    };
    // ====
    // Null
    // ====
    struct JNodeNull : JNode
    {
        JNodeNull() : JNode(JNodeType::null)
        {
        }
        [[nodiscard]] void *null() const
        {
            return (nullptr);
        }
        [[nodiscard]] std::string toString() const
        {
            return ("null");
        }
    };
    // ==============================
    // JNode base reference converter
    // ==============================
    template <typename T>
    void CheckJNodeType(auto &jNode)
    {
        if constexpr (std::is_same_v<T, JNodeString>)
        {
            if (jNode.getNodeType() != JNodeType::string)
            {
                throw Error("Node not a string.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeNumber>)
        {
            if (jNode.getNodeType() != JNodeType::number)
            {
                throw Error("Node not a number.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeArray>)
        {
            if (jNode.getNodeType() != JNodeType::array)
            {
                throw Error("Node not an array.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeObject>)
        {
            if (jNode.getNodeType() != JNodeType::object)
            {
                throw Error("Node not an object.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeBoolean>)
        {
            if (jNode.getNodeType() != JNodeType::boolean)
            {
                throw Error("Node not an boolean.");
            }
        }
        else if constexpr (std::is_same_v<T, JNodeNull>)
        {
            if (jNode.getNodeType() != JNodeType::null)
            {
                throw Error("Node not a null.");
            }
        }
    }
    template <typename T>
    T &JNodeRef(JNode &jNode)
    {
        CheckJNodeType<T>(jNode);
        return (static_cast<T &>(jNode));
    }
    template <typename T>
    const T &JNodeRef(const JNode &jNode)
    {
        CheckJNodeType<T>(jNode);
        return (static_cast<const T &>(jNode));
    }
    // ===============
    // Index overloads
    // ===============
    inline JNode &JNode::operator[](const std::string &key) // Object
    {
        return (JNodeRef<JNodeObject>(*this)[key]);
    }
    inline const JNode &JNode::operator[](const std::string &key) const // Object
    {
        return (JNodeRef<const JNodeObject>(*this)[key]);
    }
    inline JNode &JNode::operator[](int index) // Array
    {
        return (JNodeRef<JNodeArray>(*this)[index]);
    }
    inline const JNode &JNode::operator[](int index) const // Array
    {
        return (JNodeRef<JNodeArray>(*this)[index]);
    }
} // namespace JSONLib