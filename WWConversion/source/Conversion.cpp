// Conversion.cpp : Defines the entry point for the console application.
//

#include "model/Model.h"
#include "xmlreader/XmlReader.h"
#include "xmlwriter/XmlWriter.h"

#include <xercesc/util/PlatformUtils.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 3)
        return -1;

    std::wstring logfile = L"logfile.txt";
    if (argc == 4)
        logfile = argv[3];

    weight::Model model;

    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

    ww_1_2::XmlReader reader(logfile, model);
    reader.Read(argv[1]);

    ww2024::XmlWriter writer(model);
    writer.Write(argv[2]);

    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

    return 0;
}

