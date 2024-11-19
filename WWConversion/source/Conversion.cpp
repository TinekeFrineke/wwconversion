// Conversion.cpp : Defines the entry point for the console application.
//

#include "model/XmlReader.h"
#include "XmlWriter.h"

#include <xercesc/util/PlatformUtils.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 3)
        return -1;

    WW::Model model;

    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

    ww_1_2::XmlReader reader(model);
    reader.Read(argv[1]);

    ww2019::XmlWriter writer(model);
    writer.Write(argv[2]);

    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

    return 0;
}

