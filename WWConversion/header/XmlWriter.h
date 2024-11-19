#pragma once

#include "Model/XmlDefinitions.h"

#include "Utilities/StrUtils.h"

namespace WW
{

class Day;
class FixedLot;
class FreeLot;
class Gerecht;
class ManualItem;
class Model;
class Portie;
class Recept;
class CalculatedLot;
class Voedingsmiddel;
class Week;

}


namespace WW_GENERATED_NAMESPACE {

class XmlDag;
class XmlFixedlot;
class XmlFreelot;
class XmlGerecht;
class XmlHandmatigitem;
class XmlModel;
class XmlPortie;
class XmlRecept;
class XmlStandardlot;
class XmlSterrelot;
class XmlVoedingsmiddel;

class XmlWriter
{
public:
    enum class Result {
        Ok, FileNotFound, ErrorInFilename,
        FileOpenError,
        ErrorInFile, ParserError, InterpretError
    };

    Result Write(const std::tstring& aDirectory);

    Result Create(const WW::Day& aDag, XmlDag& aDay);
    Result Create(const WW::Voedingsmiddel& aMiddel, XmlVoedingsmiddel& aDay);
    Result Create(const WW::Recept& aMiddel, XmlRecept& aRecept);
    Result Create(const WW::Gerecht& aMiddel, XmlGerecht& aGerecht);
    Result Create(const WW::ManualItem& anItem, XmlHandmatigitem& anXmlItem);
    Result Create(const WW::Portie& aPortie, XmlPortie& anXmlPortie);
    Result Create(const WW::CalculatedLot& aLot, XmlStandardlot& anXmlLot);
    Result Create(const WW::FreeLot& aLot, XmlFreelot& anXmlLot);
    Result Create(const WW::FixedLot& aLot, XmlStandardlot& anXmlLot);

private:
    XmlWriter& operator=(const XmlWriter&) = delete;

    Result WritePersonalia(const std::tstring& aFilename);
    Result WriteUnits(const std::tstring& aFilename);
    Result WriteVoedingsmiddelDefinities(const std::tstring& aFilename);
    Result WriteRecepten(const std::tstring& aFilename);
    Result WriteGerechten(const std::tstring& aFilename);
    Result WriteWeeks(const std::tstring& aDirectory);

    Result Write(WW::Week& aWeek, const std::tstring& aFilename);

    WW::Model& mModel;
};

}
