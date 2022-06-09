#pragma once
//
// C++ STL
//
#include <fstream>
#include <string>
//
// Source interface
//
#include "ISource.hpp"
// =========
// NAMESPACE
// =========
namespace JSONLib
{
    // ==============================
    // Source classes for JSON parser
    // ==============================
    //
    // Buffered character source
    //
    class BufferSource : public ISource
    {
    public:
        explicit BufferSource(const std::string &sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
             m_parseBuffer = m_UTF8.from_bytes(sourceBuffer);
        }
        [[nodiscard]] Char current() const override
        {
            if (more())
            {
                return (m_parseBuffer[m_bufferPosition]);
            }
            return (static_cast<Char>(EOF));
        }
        void next() override
        {
            if (!more())
            {
                throw std::runtime_error("Tried to read past and of buffer.");
            }
            m_bufferPosition++;
        }
        [[nodiscard]] bool more() const override
        {
            return (m_bufferPosition < m_parseBuffer.size());
        }
        void reset() override
        {
            m_bufferPosition = 0;
        }
    private:
        std::size_t m_bufferPosition = 0;
        String m_parseBuffer;
    };
    //
    // File character source
    //
    class FileSource : public ISource
    {
    public:
        explicit FileSource(const std::string &sourceFileName)
        {
            m_source.open(sourceFileName.c_str(), std::ios_base::binary);
            if (!m_source.is_open())
            {
                throw std::runtime_error("JSON file input stream failed to open or does not exist.");
            }
        }
        Char current() const override
        {
            return (static_cast<Char>(m_source.peek()));
        }
        void next() override
        {
            if (!more())
            {
                throw std::runtime_error("Tried to read past end of file.");
            }
            char c = 0;
            m_source.get(c);
        }
        bool more() const override
        {
            return (m_source.peek() != EOF);
        }
        void reset() override
        {
            m_source.clear();
            m_source.seekg(0, std::ios_base::beg);
        }
    private:
        mutable std::ifstream m_source;
    };
} // namespace JSONLib