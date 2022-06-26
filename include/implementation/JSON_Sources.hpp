#pragma once
// =======
// C++ STL
// =======
#include <fstream>
#include <string>
// ================
// Source interface
// ================
#include "ISource.hpp"
// ==============
// JSON converter
// ==============
#include "JSON_Converter.hpp"
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
            JSON_Converter converter;
            if (sourceBuffer.empty())
            {
                throw Error("Empty source buffer passed to be parsed.");
            }
            m_parseBuffer = sourceBuffer;
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
                throw Error("Tried to read past and of buffer.");
            }
            m_bufferPosition++;
        }
        [[nodiscard]] bool more() const override
        {
            return (m_bufferPosition < m_parseBuffer.size());
        }
        void backup(long length) override
        {
            m_bufferPosition -= length;
            if (m_bufferPosition < 0)
            {
                m_bufferPosition = 0;
            }
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
                throw Error("File input stream failed to open or does not exist.");
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
                throw Error("Tried to read past end of file.");
            }
            char c = 0;
            m_source.get(c);
        }
        bool more() const override
        {
            return (m_source.peek() != EOF);
        }
        void backup(long length) override
        {
            if ((static_cast<long>(m_source.tellg()) - length >= 0) || (current() == (ISource::Char)EOF))
            {
                m_source.clear();
                m_source.seekg(-length, std::ios_base::cur);
            }
            else
            {
                m_source.seekg(0, std::ios_base::beg);
            }
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