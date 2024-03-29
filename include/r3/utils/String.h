#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Array.h>

#include <gjs/builtin/script_string.h>

namespace r3 {
    class String;
};

namespace std {
    template<>
    struct hash<r3::String> {
        size_t operator()(const r3::String& str) const;
    };
};

namespace r3 {
    template <typename T>
    class FixedAllocator;
    template <typename T, typename PageAllocatorTp>
    class PagedAllocator;
    template <typename T, typename AllocatorTp>
    class PerThreadSingletonAllocator;

    class String {
        public:
            using Allocator = PerThreadSingletonAllocator<char, PagedAllocator<char, FixedAllocator<char>>>;

            String();
            String(const char* cstr);
            String(const String& str);
            String(const std::string& str);
            String(const gjs::script_string& str);
            String(char* str, u32 len);
            String(void* str, u32 len) : String((char*)str, len) { }
            ~String();

            u32 size() const;
            char& operator[](u32 idx);
            char operator[](u32 idx) const;

            inline const char* c_str() const { return m_data; }
            inline operator std::string() const { return std::string(m_data, m_len); }
            inline operator void*() const { return m_data; }

            String& operator =(const char* rhs);
            String& operator =(const std::string& rhs);
            String& operator =(const String& rhs);
            String& operator +=(const String& rhs);
            String operator +(const String& rhs);
            String& operator +=(char ch);
            String operator +(char ch);
            bool operator==(const char* rhs);
            bool operator==(const String& rhs);

            void replaceAll(const String& str, const String& with);
            void replaceAll(const char* str, const String& with);

            String trim() const;
            String toUpperCase() const;
            String toLowerCase() const;

            /*
             * Splits a string into components separated by a set of delimiters
             * 
             * @note:
             * If `notBetween` is not null, then it should be an even number of characters.
             * If it is not null, and not an even number of characters, the function will
             * return an empty array.
             * 
             * Example:
             *    - `str.split({ ",", " " }, "''\"\"");` - Split the string at commas and spaces,
             *      but not between single or double quotes
             *    - `str.split("\n", "{}()");` - Split the string at newlines but not between braces
             *      or parenthesis
             * 
             * @param delimiters: Initializer list of delimiters to split on
             * @param notBetween: If not null, a string containing a number of character pairs
             *        that define the beginning and end chars of string sections which should
             *        not be split
             * @param maxCount: Maximum number of times to split
             * 
             * @returns Array of substrings which were separated by any of the specified delimiters
             */
            Array<String> split(std::initializer_list<const char*> delimiters, const char* notBetween = nullptr, u32 maxCount = 0) const;

            /*
             * Splits a string into components separated by a set of delimiters
             * 
             * @note:
             * If `notBetween` is not null, then it should be an even number of characters.
             * If it is not null, and not an even number of characters, the function will
             * return an empty array.
             * 
             * Example:
             *    - `str.split({ ",", " " }, "''\"\"");` - Split the string at commas and spaces,
             *      but not between single or double quotes
             *    - `str.split("\n", "{}()");` - Split the string at newlines but not between braces
             *      or parenthesis
             * 
             * @param delimiter: string to split on
             * @param notBetween: If not null, a string containing a number of character pairs
             *        that define the beginning and end chars of string sections which should
             *        not be split
             * @param maxCount: Maximum number of times to split
             * 
             * @returns Array of substrings which were separated by any of the specified delimiters
             */
            Array<String> split(const char* delimiter, const char* notBetween = nullptr, u32 maxCount = 0) const;
            
            Array<u32> indicesOf(const String& str) const;
            Array<u32> indicesOf(const char* str) const;
            Array<u32> indicesOf(char c) const;
            i64 firstIndexOf(const String& str) const;
            i64 firstIndexOf(const char* str) const;
            i64 firstIndexOf(char c) const;
            i64 lastIndexOf(const String& str) const;
            i64 lastIndexOf(const char* str) const;
            i64 lastIndexOf(char c) const;

            static String Format(const char* fmt, ...);

        protected:
            friend struct std::hash<String>;
            
            void resize(u32 cap);
            char* m_data;
            u32 m_len;
            u32 m_capacity;
    };
};