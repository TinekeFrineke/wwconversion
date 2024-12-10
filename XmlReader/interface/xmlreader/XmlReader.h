#pragma once

#include "Utilities/StrUtils.h"

#include "model/Day.h"
#include "model/Portie.h"
#include "model/Model.h"

#include "xmlbase/XmlBase.h"

namespace ww_1_2
{
class XmlBonuscell;
class XmlDag;
class XmlHandmatigitem;
class XmlLot;
class XmlModel;
class XmlPortie;
class XmlRecept;
class XmlVoedingsmiddel;
}

namespace weight
{

class ManualItem;
class Recept;
class Voedingsmiddel;

}

namespace ww_1_2
{

class XmlReader
{
public:
    XmlReader(weight::Model& aModel);

    weight::Result                      Read(const std::tstring& aDirectory);

    weight::Result                      ReadPersonalia(const std::tstring& aDirectory);
    weight::Result                      ReadUnits(const std::tstring& aDirectory);
    weight::Result                      ReadVoedingsmiddelDefinities(const std::tstring& aDirectory);
    weight::Result                      ReadRecepten(const std::tstring& aDirectory);
    weight::Result                      ReadGerechten(const std::tstring& aDirectory);
    weight::Result                      ReadWeeks(const std::tstring& aDirectory);
    weight::Result                      ReadBonusCells(const std::tstring& aDirectory);

private:
    XmlReader& operator=(const XmlReader&) = delete;
    XmlReader(const XmlReader&) = delete;

    weight::Result                      ReadWeek(const std::tstring& aDirectory);

    weight::Portie                  Create(const ww_1_2::XmlPortie& aPortie);
    std::unique_ptr<weight::Voedingsmiddel> Create(const ww_1_2::XmlVoedingsmiddel& aVoedingsmiddel);
    std::unique_ptr<weight::Recept> Create(const ww_1_2::XmlRecept& aRecept);
    std::unique_ptr<weight::ManualItem> Create(const ww_1_2::XmlHandmatigitem& aGerecht);
    std::unique_ptr<weight::Day> Create(const ww_1_2::XmlDag& aDag);
    weight::Bonus                   Create(const ww_1_2::XmlBonuscell& aCell);
    weight::Model& mModel;
};


} // namespace ww_1_2
