
#include "XmlReader.h"

#include <assert.h>
#include <stdlib.h>
#include <tchar.h>

#include "Utilities/FileUtils.h"

#include <Generated/XmlBonusCell.h>
#include <Generated/XmlBonusCellReader.h>
#include <Generated/XmlBonusList.h>
#include <Generated/XmlBonusListReader.h>
#include <Generated/XmlDag.h>
#include <Generated/XmlFreelot.h>
#include <Generated/XmlGerecht.h>
#include <Generated/XmlGerechtDef.h>
#include <Generated/XmlGerechtDefs.h>
#include <Generated/XmlGerechtDefsReader.h>
#include <Generated/XmlHandmatigitem.h>
#include <Generated/XmlPersonalia.h>
#include <Generated/XmlPersonaliaReader.h>
#include <Generated/XmlPortie.h>
#include <Generated/XmlPortieomschrijving.h>
#include <Generated/XmlPuntenPer100.h>
#include <Generated/XmlRecept.h>
#include <Generated/XmlReceptdef.h>
#include <Generated/XmlReceptdefs.h>
#include <Generated/XmlReceptdefsReader.h>
#include <Generated/XmlStandardlot.h>
#include <Generated/XmlUnit.h>
#include <Generated/XmlUnits.h>
#include <Generated/XmlUnitsReader.h>
#include <Generated/XmlVoedingsmiddel.h>
#include <Generated/XmlVoedingsmiddelDef.h>
#include <Generated/XmlVoedingsmiddelDefs.h>
#include <Generated/XmlVoedingsmiddelDefsReader.h>
#include <Generated/XmlVoedingswaarde.h>
#include <Generated/XmlWeek.h>
#include <Generated/XmlWeekReader.h>

#include "model/BonusPointsMap.h"
#include "model/ManualItem.h"
#include "model/NutritionalValue.h"
#include "model/Personalia.h"
#include "model/Recept.h"
#include "model/ReceptDefinitie.h"
#include "model/VoedingsMiddel.h"
#include "model/VoedingsmiddelDefinitie.h"
#include "model/Week.h"
#include "model/WWDefinitions.h"

namespace ww_1_2
{


XmlReader::XmlReader(weight::Model& aModel)
    : mModel(aModel)
{
}


weight::Result XmlReader::Read(const std::tstring& aDirectory)
{
    weight::Result result = ReadPersonalia(aDirectory + _T("\\personalia.xml"));
    if (result == weight::Result::Ok)
        result = ReadVoedingsmiddelDefinities(aDirectory + _T("\\voedingsmiddeldefinities.xml"));
    if (result == weight::Result::Ok)
        result = ReadUnits(aDirectory + _T("\\units.xml"));
    if (result == weight::Result::Ok)
        result = ReadRecepten(aDirectory + _T("\\recepten.xml"));
    if (result == weight::Result::Ok)
        result = ReadGerechten(aDirectory + _T("\\restaurantgerechten.xml"));
    if (result == weight::Result::Ok)
        result = ReadWeeks(aDirectory);
    if (result == weight::Result::Ok)
        result = ReadBonusCells(aDirectory);

    return result;
}


weight::Result XmlReader::ReadPersonalia(const std::tstring& aDirectory)
{
    XmlPersonaliaReader reader;
    XmlPersonalia* xmlpersonalia;
    switch (reader.Read(aDirectory, xmlpersonalia))
    {
        case XmlPersonaliaReader::RESULT_Ok:
            if (xmlpersonalia == nullptr)
                return weight::Result::Ok;
            break;
        case XmlPersonaliaReader::RESULT_FileNotFound:
            return weight::Result::FileNotFound;
        case XmlPersonaliaReader::RESULT_ErrorInFilename:
            return weight::Result::ErrorInFilename;
        case XmlPersonaliaReader::RESULT_FileOpenError:
            return weight::Result::FileOpenError;
        case XmlPersonaliaReader::RESULT_ErrorInFile:
            return weight::Result::ErrorInFile;
        case XmlPersonaliaReader::RESULT_ParserError:
            return weight::Result::ParserError;
        case XmlPersonaliaReader::RESULT_InterpretError:
        case XmlPersonaliaReader::RESULT_WriteError:
        default:
            return weight::Result::InterpretError;
            break;
    }

    auto personalia = std::make_unique<weight::Personalia>(xmlpersonalia->Getgebruikersnaam());
    personalia->SetName(xmlpersonalia->Getnaam());
    Utils::Date date(Utils::Date::Today());
    if (Utils::ToDate(xmlpersonalia->Getgeboren(), date))
        personalia->SetDateOfBirth(date);
    personalia->SetGeslacht(xmlpersonalia->Getgeslacht() == XmlPersonalia::geslacht_Mannelijk
                            ? weight::Personalia::GESLACHT::Mannelijk : weight::Personalia::GESLACHT::Vrouwelijk);
    switch (xmlpersonalia->Getwerk())
    {
        case XmlPersonalia::werk_Zittend:
            personalia->SetTypeOfWork(weight::Personalia::WERKTYPE::Zittend);
            break;
        case XmlPersonalia::werk_Staand:
            personalia->SetTypeOfWork(weight::Personalia::WERKTYPE::Staand);
            break;
        case XmlPersonalia::werk_Lopend:
            personalia->SetTypeOfWork(weight::Personalia::WERKTYPE::Lopend);
            break;
        case XmlPersonalia::werk_Zwaar:
            personalia->SetTypeOfWork(weight::Personalia::WERKTYPE::Zwaar);
            break;
    }

    personalia->SetKCPuntenTotaal(_ttoi(xmlpersonalia->Getkcpunten().c_str()));
    personalia->SetKCWeekPuntenTotaal(_ttoi(xmlpersonalia->Getkcweekpunten().c_str()));
    personalia->SetCHPuntenTotaal(Str::ToDouble(xmlpersonalia->Getchpunten().c_str()));
    personalia->SetCHWeekPuntenTotaal(Str::ToDouble(xmlpersonalia->Getchweekpunten().c_str()));
    personalia->SetStreefGewicht(Str::ToDouble(xmlpersonalia->Getstreefgewicht().c_str()));
    personalia->SetStartGewicht(Str::ToDouble(xmlpersonalia->Getstartgewicht().c_str()));
    personalia->SetHuidigGewicht(Str::ToDouble(xmlpersonalia->Gethuidiggewicht().c_str()));
    personalia->SetLengte(Str::ToInt(xmlpersonalia->Getlengte().c_str()));

    switch (xmlpersonalia->Getstrategie())
    {
        case XmlPersonalia::strategie_KCal:
            personalia->SetStrategy(weight::STRATEGY_TYPE::KCal);
            break;
        case XmlPersonalia::strategie_CarboHydrates:
            personalia->SetStrategy(weight::STRATEGY_TYPE::CarboHydrates);
            break;
        default:
            return weight::Result::InterpretError;
    }

    mModel.Add(std::move(personalia));

    delete xmlpersonalia;

    return weight::Result::Ok;

}

weight::Result XmlReader::ReadUnits(const std::tstring& aDirectory)
{
    XmlUnitsReader reader;
    XmlUnits* xmlunits;
    switch (reader.Read(aDirectory, xmlunits))
    {
        case XmlUnitsReader::RESULT_Ok:
            if (xmlunits == nullptr)
                return weight::Result::Ok;
            break;
        case XmlUnitsReader::RESULT_FileNotFound:
            return weight::Result::FileNotFound;
        case XmlUnitsReader::RESULT_ErrorInFilename:
            return weight::Result::ErrorInFilename;
        case XmlUnitsReader::RESULT_FileOpenError:
            return weight::Result::FileOpenError;
        case XmlUnitsReader::RESULT_ErrorInFile:
            return weight::Result::ErrorInFile;
        case XmlUnitsReader::RESULT_ParserError:
            return weight::Result::ParserError;
        case XmlUnitsReader::RESULT_InterpretError:
        case XmlUnitsReader::RESULT_WriteError:
        default:
            return weight::Result::InterpretError;
            break;
    }

    const std::vector<XmlUnit*>& units(xmlunits->GetUnitList());
    for (size_t i = 0; i < units.size(); ++i)
        mModel.AddUnit(units[i]->Getnaam());

    delete xmlunits;

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadVoedingsmiddelDefinities(const std::tstring& aDirectory)
{
    XmlVoedingsmiddeldefsReader reader;
    XmlVoedingsmiddeldefs* xmlvoedingsmiddeldefs;
    switch (reader.Read(aDirectory, xmlvoedingsmiddeldefs))
    {
        case XmlVoedingsmiddeldefsReader::RESULT_Ok:
            if (xmlvoedingsmiddeldefs == nullptr)
                return weight::Result::Ok;
            break;
        case XmlVoedingsmiddeldefsReader::RESULT_FileNotFound:
            return weight::Result::FileNotFound;
        case XmlVoedingsmiddeldefsReader::RESULT_ErrorInFilename:
            return weight::Result::ErrorInFilename;
        case XmlVoedingsmiddeldefsReader::RESULT_FileOpenError:
            return weight::Result::FileOpenError;
        case XmlVoedingsmiddeldefsReader::RESULT_ErrorInFile:
            return weight::Result::ErrorInFile;
        case XmlVoedingsmiddeldefsReader::RESULT_ParserError:
            return weight::Result::ParserError;
        case XmlVoedingsmiddeldefsReader::RESULT_InterpretError:
        case XmlVoedingsmiddeldefsReader::RESULT_WriteError:
        default:
            return weight::Result::InterpretError;
            break;
    }

    const std::vector<XmlVoedingsmiddeldef*>& vmlist = xmlvoedingsmiddeldefs->GetVoedingsmiddeldefList();
    for (auto vm : vmlist)
    {
        if (vm->GetVoedingswaarde() == nullptr)
            // This was a formerly fixed points food definition
            continue;

        auto nutritionalValue = std::make_unique<weight::NutritionalValue>(mModel.GetCalculator());
        nutritionalValue->SetKCalPer100Units(Str::ToDouble(vm->GetVoedingswaarde()->Getkcalper100().c_str()));
        nutritionalValue->SetFatPer100Units(Str::ToDouble(vm->GetVoedingswaarde()->Getvetper100().c_str()));
        nutritionalValue->SetCarbohydratesPer100Units(Str::ToDouble(vm->GetVoedingswaarde()->Getkoolhydratenper100().c_str()));
        nutritionalValue->SetProteiPer100Units(Str::ToDouble(vm->GetVoedingswaarde()->Geteiwitper100().c_str()));
        nutritionalValue->SetFibersPer100Units(Str::ToDouble(vm->GetVoedingswaarde()->Getvezelsper100().c_str()));

        // TF_TODO: Improve!!!
        weight::VMDefinitie* definitie = new weight::VMDefinitie(mModel.GetCalculator(),
                                                                 vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getnaam(),
                                                                 vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getunit(),
                                                                 std::move(nutritionalValue));

        if (!vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getcategorie().empty())
            definitie->SetCategory(vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getcategorie());
        if (!vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getmerk().empty())
            definitie->SetMerk(vm->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getmerk());
        definitie->SetFavourite(vm->Getfavoriet() == XmlVoedingsmiddeldef::favoriet_true);

        const std::vector<XmlPortie*>& portielist = vm->GetPortieList();
        for (auto xmlportie : portielist)
        {
            auto portie = std::make_unique<weight::Portie>(xmlportie->Getnaam());
            portie->SetUnits(Str::ToDouble(xmlportie->Geteenheden().c_str()));
            definitie->AddPortie(std::move(portie));
        }

        auto uni = std::unique_ptr<weight::VMDefinitie>(definitie);
        mModel.Add(std::move(uni));
    }

    delete xmlvoedingsmiddeldefs;

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadRecepten(const std::tstring& aDirectory)
{
    XmlReceptdefsReader reader;
    XmlReceptdefs* xmlreceptdefs;
    switch (reader.Read(aDirectory, xmlreceptdefs))
    {
        case XmlReceptdefsReader::RESULT_Ok:
            if (xmlreceptdefs == nullptr)
                return weight::Result::Ok;
            break;
        case XmlReceptdefsReader::RESULT_FileNotFound:
            return weight::Result::FileNotFound;
        case XmlReceptdefsReader::RESULT_ErrorInFilename:
            return weight::Result::ErrorInFilename;
        case XmlReceptdefsReader::RESULT_FileOpenError:
            return weight::Result::FileOpenError;
        case XmlReceptdefsReader::RESULT_ErrorInFile:
            return weight::Result::ErrorInFile;
        case XmlReceptdefsReader::RESULT_ParserError:
            return weight::Result::ParserError;
        case XmlReceptdefsReader::RESULT_InterpretError:
        case XmlReceptdefsReader::RESULT_WriteError:
        default:
            return weight::Result::InterpretError;
            break;
    }

    const std::vector<XmlReceptdef*> receptlist = xmlreceptdefs->GetReceptdefList();
    for (size_t i = 0; i < receptlist.size(); ++i)
    {
        auto receptdef = std::make_unique<weight::ReceptDefinitie>(receptlist[i]->Getnaam());
        receptdef->SetPortions(Str::ToInt(receptlist[i]->Getporties()));
        const std::vector<XmlVoedingsmiddel*>& voedingsmiddellist(receptlist[i]->GetVoedingsmiddelList());
        for (size_t j = 0; j < voedingsmiddellist.size(); ++j)
        {
            auto voedingsmiddel = Create(*voedingsmiddellist[j]);
            if (voedingsmiddel != nullptr)
                receptdef->Add(std::move(voedingsmiddel));
        }

        mModel.Add(std::move(receptdef));
    }

    delete xmlreceptdefs;

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadWeeks(const std::tstring& aDirectory)
{
    std::tstring filename(aDirectory + _T("\\week*.xml"));
    //TCHAR filemask[_MAX_PATH];
    //_stprintf(filemask, _T("%sweek????????.xml"), aDirectory.c_str());
    WIN32_FIND_DATA finddata;
    HANDLE hFind = FindFirstFile(filename.c_str(), &finddata);
    bool bContinue = true;
    while (bContinue) {
        ReadWeek(aDirectory + _T("\\") + finddata.cFileName);
        bContinue = FindNextFile(hFind, &finddata) != FALSE;
    }

    FindClose(hFind);

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadWeek(const std::tstring& aDirectory)
{
    XmlWeekReader reader;
    XmlWeek* xmlweek;
    switch (reader.Read(aDirectory, xmlweek))
    {
        case XmlWeekReader::RESULT_Ok:
            if (xmlweek == nullptr)
                return weight::Result::Ok;
            break;
        case XmlWeekReader::RESULT_FileNotFound:
            return weight::Result::FileNotFound;
        case XmlWeekReader::RESULT_ErrorInFilename:
            return weight::Result::ErrorInFilename;
        case XmlWeekReader::RESULT_FileOpenError:
            return weight::Result::FileOpenError;
        case XmlWeekReader::RESULT_ErrorInFile:
            return weight::Result::ErrorInFile;
        case XmlWeekReader::RESULT_ParserError:
            return weight::Result::ParserError;
        case XmlWeekReader::RESULT_InterpretError:
        case XmlWeekReader::RESULT_WriteError:
        default:
            return weight::Result::InterpretError;
            break;
    }

    auto week = std::make_unique<weight::Week>(Utils::ToDate(xmlweek->Getbegindatum()),
                                               Utils::ToDate(xmlweek->Geteinddatum()));
    week->SetPoints(Str::ToDouble(xmlweek->Getpunten()));
    week->SetSaveablePoints(Str::ToDouble(xmlweek->Getweekpunten()));
    week->SetStartWeight(Str::ToDouble(xmlweek->Getstartweight()));

    switch (xmlweek->Getstrategie())
    {
        case XmlWeek::strategie_FlexiPoints:
        case XmlWeek::strategie_KCal:
            week->SetStrategy(weight::STRATEGY_TYPE::KCal, mModel);
            break;
        case XmlWeek::strategie_CarboHydrates:
            week->SetStrategy(weight::STRATEGY_TYPE::CarboHydrates, mModel);
            break;
    }

    const std::vector<XmlDag*> dagen(xmlweek->GetDagList());
    for (size_t i = 0; i < dagen.size(); ++i)
    {
        auto dag = Create(*dagen[i]);
        if (!week->Add(std::move(dag)))
            return weight::Result::InterpretError;
    }

    if (!mModel.Add(std::move(week)))
        return weight::Result::InterpretError;

    delete xmlweek;

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadGerechten(const std::tstring& aDirectory)
{
    (void)aDirectory;

    return weight::Result::Ok;
}


weight::Result XmlReader::ReadBonusCells(const std::tstring& aDirectory)
{
    XmlBonuslistReader reader;
    XmlBonuslist* xmlbonuslist;

    switch (reader.Read(aDirectory + _T("\\bonuspoints.xml"), xmlbonuslist))
    {
        case XmlWeekReader::RESULT_Ok:
            if (xmlbonuslist == nullptr)
                return weight::Result::Ok;
            break;
        case XmlWeekReader::RESULT_FileNotFound:
            delete xmlbonuslist;
            return weight::Result::FileNotFound;
        case XmlWeekReader::RESULT_ErrorInFilename:
            delete xmlbonuslist;
            return weight::Result::ErrorInFilename;
        case XmlWeekReader::RESULT_FileOpenError:
            delete xmlbonuslist;
            return weight::Result::FileOpenError;
        case XmlWeekReader::RESULT_ErrorInFile:
            delete xmlbonuslist;
            return weight::Result::ErrorInFile;
        case XmlWeekReader::RESULT_ParserError:
            delete xmlbonuslist;
            return weight::Result::ParserError;
        case XmlWeekReader::RESULT_InterpretError:
        case XmlWeekReader::RESULT_WriteError:
        default:
            delete xmlbonuslist;
            return weight::Result::InterpretError;
            break;
    }

    for (size_t i = 0; i < xmlbonuslist->GetBonuscellList().size(); ++i)
    {
        XmlBonuscell* cell = xmlbonuslist->GetBonuscellList()[i];
        weight::BonusPointsMap::MOVEMENT_INTENSITY intensity = weight::BonusPointsMap::MI_High;
        switch (cell->Getintensiteit())
        {
            case XmlBonuscell::intensiteit_hoog:
                intensity = weight::BonusPointsMap::MI_High;
                break;
            case XmlBonuscell::intensiteit_middel:
                intensity = weight::BonusPointsMap::MI_Medium;
                break;
            case XmlBonuscell::intensiteit_laag:
                intensity = weight::BonusPointsMap::MI_Low;
                break;
            default:
                delete xmlbonuslist;
                return weight::Result::InterpretError;
        }

        mModel.GetBonusPointsMap()[intensity][cell->Getgewicht()][(cell->Getminuten())] = cell->Getpunten();
    }

    //mModel.GetBonusPointsMap().Debug();
    delete xmlbonuslist;
    return weight::Result::Ok;
}


weight::Portie XmlReader::Create(const XmlPortie& aPortie)
{
    weight::Portie portie(aPortie.Getnaam());
    portie.SetUnits(Str::ToDouble(aPortie.Geteenheden()));

    return portie;
}


std::unique_ptr<weight::Voedingsmiddel> XmlReader::Create(const XmlVoedingsmiddel& aVoedingsmiddel)
{
    std::unique_ptr<weight::PortionedLot> lot;

    if (aVoedingsmiddel.GetStandardlot() != nullptr)
    {
        const XmlStandardlot* xmllot = aVoedingsmiddel.GetStandardlot();
        const auto voedingswaarde = xmllot->GetVoedingswaarde();
        if (voedingswaarde == nullptr)
            // Obsolete food type, don't convert
            return {};

        weight::Portie portie(xmllot->GetPortie().Getnaam());
        portie.SetUnits(Str::ToDouble(xmllot->GetPortie().Geteenheden()));

        lot = std::make_unique<weight::PortionedLot>(mModel.GetCalculator(), portie);
        lot->SetNumberOfPortions(Str::ToDouble(xmllot->Gethoeveelheid()));
        lot->SetKCalPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getkcalper100()));
        lot->SetVetPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getvetper100()));
        lot->SetKoolhydratenPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getkoolhydratenper100().c_str()));
        lot->SetEiwitPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Geteiwitper100().c_str()));
        lot->SetVezelsPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getvezelsper100().c_str()));
    }
    else
    {
        weight::VMDefinitie* def = mModel.FindVoedingsmiddelDefinitie(aVoedingsmiddel.Getnaam());
        if (def == nullptr)
        {
            ::MessageBox(0, (_T("Unable to interpret voedingsmiddel ") + aVoedingsmiddel.Getnaam()).c_str(),
                         _T("ERROR"), MB_OK);
            return {};
        }
        else
        {
            if (!def->GetPortieList().empty())
            {
                weight::Portie& portie = *def->GetPortieList()[0];
                lot = std::make_unique< weight::PortionedLot>(mModel.GetCalculator(), portie);
                lot->SetNumberOfPortions(1);
                lot->SetParameters(def->GetNutritionalValue().GetParameters());
            }
            else
            {
                assert(false);
            }
        }
    }

    auto voedingsmiddel = std::make_unique<weight::Voedingsmiddel>(aVoedingsmiddel.Getnaam(), std::move(lot),
                                                                   aVoedingsmiddel.Getunit());
    voedingsmiddel->SetCategory(aVoedingsmiddel.Getcategorie());
    return std::move(voedingsmiddel);
}


std::unique_ptr<weight::Recept> XmlReader::Create(const XmlRecept& aRecept)
{
    auto recept = std::make_unique<weight::Recept>(aRecept.Getnaam());
    recept->SetPointsPerPortion(Str::ToDouble(aRecept.Getpunten()));
    recept->SetNumberOfPortions(Str::ToDouble(aRecept.Gethoeveelheid()));
    return std::move(recept);
}


std::unique_ptr<weight::ManualItem> XmlReader::Create(const XmlHandmatigitem& anItem)
{
    auto item = std::make_unique<weight::ManualItem>(anItem.Getnaam(), Str::ToDouble(anItem.Getpunten()));
    item->Set(Str::ToDouble(anItem.Getpunten()), Str::ToDouble(anItem.Gethoeveelheid()));
    return item;
}


std::unique_ptr<weight::Day> XmlReader::Create(const XmlDag& aDag)
{
    auto day = std::make_unique<weight::Day>(Utils::ToDate(aDag.Getdatum()));
    if (!aDag.Getgewicht().empty())
        day->SetWeight(Str::ToDouble(aDag.Getgewicht().c_str()));

    if (!aDag.Getbonuspunten().empty())
        day->SetFreeBonusPoints(Str::ToDouble(aDag.Getbonuspunten().c_str()));

    const std::vector<XmlVoedingsmiddel*>& vmiddelen(aDag.GetVoedingsmiddelList());
    for (size_t i = 0; i < vmiddelen.size(); ++i)
        day->Add(Create(*vmiddelen[i]));

    const std::vector<XmlRecept*>& recepten(aDag.GetReceptList());
    for (size_t i = 0; i < recepten.size(); ++i)
        day->Add(Create(*recepten[i]));

    const std::vector<XmlHandmatigitem*>& handmatig(aDag.GetHandmatigitemList());
    for (size_t i = 0; i < handmatig.size(); ++i)
        day->Add(Create(*handmatig[i]));

    const std::vector<XmlBonuscell*>& bonuslist(aDag.GetBonuscellList());
    for (size_t i = 0; i < bonuslist.size(); ++i)
        day->Add(Create(*bonuslist[i]));

    return std::move(day);
}


weight::Bonus XmlReader::Create(const XmlBonuscell& aCell)
{
    weight::Bonus::INTENSITY intensity =
        aCell.Getintensiteit() == XmlBonuscell::intensiteit_hoog ? weight::Bonus::INTENSITY::High :
        aCell.Getintensiteit() == XmlBonuscell::intensiteit_middel ? weight::Bonus::INTENSITY::Medium :
        weight::Bonus::INTENSITY::Low;
    return weight::Bonus(intensity, aCell.Getminuten(), aCell.Getpunten());
}


} // namespace ww_1_2