#pragma once

class XmlBase
{
public:
    enum class Result {
        Ok, FileNotFound, ErrorInFilename,
        FileOpenError,
        ErrorInFile, ParserError, InterpretError
    };
};
