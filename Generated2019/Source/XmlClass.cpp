#include "XmlClass.h"

#include <iostream>
#include <tchar.h>
#include <xercesc/framework/localfileinputsource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/saxparseexception.hpp>

#include <Utilities/strutils.h>


namespace ww2019
{

void XmlClass::ParseErrorHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
    std::cout << "Warning at file :" << e.getSystemId() << ", line " << e.getLineNumber()
        << " char " << e.getColumnNumber() << ": " << e.getMessage() << '\n';
}


void XmlClass::ParseErrorHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
    std::cout << "Error at file :" << e.getSystemId() << ", line " << e.getLineNumber()
        << " char " << e.getColumnNumber() << ": " << e.getMessage() << '\n';

    throw e;
}


void XmlClass::ParseErrorHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
    std::cout << "Fatal error at file :" << e.getSystemId() << ", line " << e.getLineNumber()
        << " char " << e.getColumnNumber() << ": " << e.getMessage() << '\n';

    throw e;
}


void XmlClass::ParseErrorHandler::resetErrors()
{
}

XmlClass::Result XmlClass::ParseDocument(const std::wstring &                   aFilename,
                                          XERCES_CPP_NAMESPACE::XercesDOMParser &aParser)
{
    if (_taccess(aFilename.c_str(), 0) == -1)
    {
        switch (errno)
        {
            case ENOENT:
                return Result::FileNotFound;
            default:
                return Result::FileOpenError;
        }
    }

    XERCES_CPP_NAMESPACE::LocalFileInputSource source(NULL, aFilename.c_str());
    try
    {
        aParser.parse(source);
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& e)
    {
        std::cout << "Exception occurred: " << e.getMessage() << "; File being parsed is " << Str::ToTString(aFilename).c_str() << '\n';
        return Result::ParserError;
    }
 	catch (...)
 	{
        std::cout << "Unexpected Exception occurred: " << Str::ToTString(aFilename).c_str() << '\n';
        return Result::ParserError;
    }

    return Result::Ok;
}
}

