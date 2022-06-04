//
// Class: JSON_Translator
//
// Description: Translate to/from JSON string escapes within source
// strings. This is the default translator but is possible to write
// a custom one and pass it to the JSON class constructor to be used.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "JSON.hpp"
#include "JSON_Converter.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace JSONLib
{
// ===========================
// PRIVATE TYPES AND CONSTANTS
// ===========================
// ==========================
// PUBLIC TYPES AND CONSTANTS
// ==========================
// ========================
// PRIVATE STATIC VARIABLES
// ========================
// =======================
// PUBLIC STATIC VARIABLES
// =======================
// ===============
// PRIVATE METHODS
// ===============
// NEED TO SEPARATE OUT LAYER.
// ==============
// PUBLIC METHODS
// ==============
#if defined(_WIN64)
#include "Windows.h"
#else
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
#endif
    /// <summary>
    /// Convert utf8 <-> utf16 strings.
    /// </summary>
    std::u16string JSON_Converter::utf8_to_utf16(const std::string &utf8)
    {
#if defined(_WIN64)
        int count = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.length()), NULL, 0);
        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.length()), &wstr[0], count);
        return (std::u16string{wstr.begin(), wstr.end()});
#else
        return (m_UTF16.from_bytes(utf8));
#endif
    }
    std::string JSON_Converter::utf16_to_utf8(const std::u16string &utf16)
    {
#if defined(_WIN64)
        std::wstring wstr{utf16.begin(), utf16.end()};
        int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), NULL, 0, NULL, NULL);
        std::string str(count, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
        return str;
#else
        return (m_UTF16.to_bytes(utf16));
#endif
    }
} // namespace JSONLib
