
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
#include "model/Gerecht.h"
#include "model/ManualItem.h"
#include "model/Personalia.h"
#include "model/Recept.h"
#include "model/ReceptDefinitie.h"
#include "model/VoedingsMiddel.h"
#include "model/VoedingsmiddelDefinitie.h"
#include "model/Week.h"
#include "model/WWDefinitions.h"
//#include "XmlVMCreateVisitor.h"

namespace ww_1_2
{


XmlReader::XmlReader(WW::Model& aModel)
    : mModel(aModel)
{
}


WW::Result XmlReader::Read(const std::tstring& aDirectory)
{
    WW::Result result = ReadPersonalia(aDirectory + _T("\\personalia.xml"));
    if (result == WW::Result::Ok)
        result = ReadVoedingsmiddelDefinities(aDirectory + _T("\\voedingsmiddeldefinities.xml"));
    if (result == WW::Result::Ok)
        result = ReadUnits(aDirectory + _T("\\units.xml"));
    if (result == WW::Result::Ok)
        result = ReadRecepten(aDirectory + _T("\\recepten.xml"));
    if (result == WW::Result::Ok)
        result = ReadGerechten(aDirectory + _T("\\restaurantgerechten.xml"));
    if (result == WW::Result::Ok)
        result = ReadWeeks(aDirectory);
    if (result == WW::Result::Ok)
        result = ReadBonusCells(aDirectory);

    return result;
}


WW::Result XmlReader::ReadPersonalia(const std::tstring& aDirectory)
{
    XmlPersonaliaReader reader;
    XmlPersonalia* xmlpersonalia;
    switch (reader.Read(aDirectory, xmlpersonalia))
    {
        case XmlPersonaliaReader::RESULT_Ok:
            if (xmlpersonalia == nullptr)
                return WW::Result::Ok;
            break;
        case XmlPersonaliaReader::RESULT_FileNotFound:
            return WW::Result::FileNotFound;
        case XmlPersonaliaReader::RESULT_ErrorInFilename:
            return WW::Result::ErrorInFilename;
        case XmlPersonaliaReader::RESULT_FileOpenError:
            return WW::Result::FileOpenError;
        case XmlPersonaliaReader::RESULT_ErrorInFile:
            return WW::Result::ErrorInFile;
        case XmlPersonaliaReader::RESULT_ParserError:
            return WW::Result::ParserError;
        case XmlPersonaliaReader::RESULT_InterpretError:
        case XmlPersonaliaReader::RESULT_WriteError:
        default:
            return WW::Result::InterpretError;
            break;
    }

    auto personalia = std::make_unique<WW::Personalia>(xmlpersonalia->Getgebruikersnaam());
    personalia->SetName(xmlpersonalia->Getnaam());
    Utils::Date date(Utils::Date::Today());
    if (Utils::ToDate(xmlpersonalia->Getgeboren(), date))
        personalia->SetDateOfBirth(date);
    personalia->SetGeslacht(xmlpersonalia->Getgeslacht() == XmlPersonalia::geslacht_Mannelijk
                            ? WW::Personalia::GESLACHT::Mannelijk : WW::Personalia::GESLACHT::Vrouwelijk);
    switch (xmlpersonalia->Getwerk())
    {
        case XmlPersonalia::werk_Zittend:
            personalia->SetTypeOfWork(WW::Personalia::WERKTYPE::Zittend);
            break;
        case XmlPersonalia::werk_Staand:
            personalia->SetTypeOfWork(WW::Personalia::WERKTYPE::Staand);
            break;
        case XmlPersonalia::werk_Lopend:
            personalia->SetTypeOfWork(WW::Personalia::WERKTYPE::Lopend);
            break;
        case XmlPersonalia::werk_Zwaar:
            personalia->SetTypeOfWork(WW::Personalia::WERKTYPE::Zwaar);
            break;
    }

    personalia->SetFPPuntenTotaal(_ttoi(xmlpersonalia->Getpunten().c_str()));
    personalia->SetPPPuntenTotaal(_ttoi(xmlpersonalia->Getpppunten().c_str()));
    personalia->SetPPWeekPuntenTotaal(_ttoi(xmlpersonalia->Getppweekpunten().c_str()));
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
            personalia->SetStrategy(WW::STRATEGY_TYPE::KCal);
            break;
        case XmlPersonalia::strategie_CarboHydrates:
            personalia->SetStrategy(WW::STRATEGY_TYPE::CarboHydrates);
            break;
        default:
            return WW::Result::InterpretError;
    }

    mModel.Add(std::move(personalia));

    delete xmlpersonalia;

    return WW::Result::Ok;

}

WW::Result XmlReader::ReadUnits(const std::tstring& aDirectory)
{
    XmlUnitsReader reader;
    XmlUnits* xmlunits;
    switch (reader.Read(aDirectory, xmlunits))
    {
        case XmlUnitsReader::RESULT_Ok:
            if (xmlunits == nullptr)
                return WW::Result::Ok;
            break;
        case XmlUnitsReader::RESULT_FileNotFound:
            return WW::Result::FileNotFound;
        case XmlUnitsReader::RESULT_ErrorInFilename:
            return WW::Result::ErrorInFilename;
        case XmlUnitsReader::RESULT_FileOpenError:
            return WW::Result::FileOpenError;
        case XmlUnitsReader::RESULT_ErrorInFile:
            return WW::Result::ErrorInFile;
        case XmlUnitsReader::RESULT_ParserError:
            return WW::Result::ParserError;
        case XmlUnitsReader::RESULT_InterpretError:
        case XmlUnitsReader::RESULT_WriteError:
        default:
            return WW::Result::InterpretError;
            break;
    }

    const std::vector<XmlUnit*>& units(xmlunits->GetUnitList());
    for (size_t i = 0; i < units.size(); ++i)
        mModel.Add(WW::Unit(mModel, units[i]->Getnaam()));

    delete xmlunits;

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadVoedingsmiddelDefinities(const std::tstring& aDirectory)
{
    XmlVoedingsmiddeldefsReader reader;
    XmlVoedingsmiddeldefs* xmlvoedingsmiddeldefs;
    switch (reader.Read(aDirectory, xmlvoedingsmiddeldefs))
    {
        case XmlVoedingsmiddeldefsReader::RESULT_Ok:
            if (xmlvoedingsmiddeldefs == nullptr)
                return WW::Result::Ok;
            break;
        case XmlVoedingsmiddeldefsReader::RESULT_FileNotFound:
            return WW::Result::FileNotFound;
        case XmlVoedingsmiddeldefsReader::RESULT_ErrorInFilename:
            return WW::Result::ErrorInFilename;
        case XmlVoedingsmiddeldefsReader::RESULT_FileOpenError:
            return WW::Result::FileOpenError;
        case XmlVoedingsmiddeldefsReader::RESULT_ErrorInFile:
            return WW::Result::ErrorInFile;
        case XmlVoedingsmiddeldefsReader::RESULT_ParserError:
            return WW::Result::ParserError;
        case XmlVoedingsmiddeldefsReader::RESULT_InterpretError:
        case XmlVoedingsmiddeldefsReader::RESULT_WriteError:
        default:
            return WW::Result::InterpretError;
            break;
    }

    const std::vector<XmlVoedingsmiddeldef*>& vmlist = xmlvoedingsmiddeldefs->GetVoedingsmiddeldefList();
    for (size_t i = 0; i < vmlist.size(); ++i)
    {
        // TF_TODO: Improve!!!
        WW::VMDefBase* base = nullptr;

        if (vmlist[i]->GetVoedingswaarde() != nullptr)
        {
            WW::CalculatedVMDef* cdefinitie = new WW::CalculatedVMDef(mModel.GetCalculator());
            cdefinitie->SetKCalPer100Units(Str::ToDouble(vmlist[i]->GetVoedingswaarde()->Getkcalper100().c_str()));
            cdefinitie->SetVetPer100Units(Str::ToDouble(vmlist[i]->GetVoedingswaarde()->Getvetper100().c_str()));
            cdefinitie->SetKoolhydratenPer100Units(Str::ToDouble(vmlist[i]->GetVoedingswaarde()->Getkoolhydratenper100().c_str()));
            cdefinitie->SetEiwitPer100Units(Str::ToDouble(vmlist[i]->GetVoedingswaarde()->Geteiwitper100().c_str()));
            cdefinitie->SetVezelsPer100Units(Str::ToDouble(vmlist[i]->GetVoedingswaarde()->Getvezelsper100().c_str()));
            base = cdefinitie;
        }
        else if (vmlist[i]->GetPuntenper100() != nullptr)
        {
            WW::FixedVMDef* fdefinitie = new WW::FixedVMDef;
            fdefinitie->SetPointsPer100Units(Str::ToDouble(vmlist[i]->GetPuntenper100()->Getpunten()));
            base = fdefinitie;
        }
        else
        {
            assert(false);
            return WW::Result::InterpretError;
        }

        WW::VMDefinitie* definitie = new WW::VMDefinitie(mModel.GetCalculator(),
                                                         vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getnaam(),
                                                         WW::Unit(mModel, vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getunit()),
                                                         base);


        if (!vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getcategorie().empty())
            definitie->SetCategory(WW::CategorieNaam(mModel, vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getcategorie()));
        if (!vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getmerk().empty())
            definitie->SetMerk(WW::MerkNaam(mModel, vmlist[i]->GetVoedingsmiddelbasis().GetVoedingsmiddelheader().Getmerk()));
        definitie->SetFavourite(vmlist[i]->Getfavoriet() == XmlVoedingsmiddeldef::favoriet_true);

        const std::vector<XmlPortie*>& portielist = vmlist[i]->GetPortieList();
        for (size_t p = 0; p < portielist.size(); ++p)
        {
            auto portie = std::make_unique<WW::Portie>(WW::PortieNaam(mModel, portielist[p]->Getnaam()));
            portie->SetUnits(Str::ToDouble(portielist[p]->Geteenheden().c_str()));
            mModel.Add(portie->GetName());
            definitie->AddPortie(std::move(portie));
        }

        auto uni = std::unique_ptr<WW::VMDefinitie>(definitie);
        mModel.Add(std::move(uni));
    }

    delete xmlvoedingsmiddeldefs;

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadRecepten(const std::tstring& aDirectory)
{
    XmlReceptdefsReader reader;
    XmlReceptdefs* xmlreceptdefs;
    switch (reader.Read(aDirectory, xmlreceptdefs))
    {
        case XmlReceptdefsReader::RESULT_Ok:
            if (xmlreceptdefs == nullptr)
                return WW::Result::Ok;
            break;
        case XmlReceptdefsReader::RESULT_FileNotFound:
            return WW::Result::FileNotFound;
        case XmlReceptdefsReader::RESULT_ErrorInFilename:
            return WW::Result::ErrorInFilename;
        case XmlReceptdefsReader::RESULT_FileOpenError:
            return WW::Result::FileOpenError;
        case XmlReceptdefsReader::RESULT_ErrorInFile:
            return WW::Result::ErrorInFile;
        case XmlReceptdefsReader::RESULT_ParserError:
            return WW::Result::ParserError;
        case XmlReceptdefsReader::RESULT_InterpretError:
        case XmlReceptdefsReader::RESULT_WriteError:
        default:
            return WW::Result::InterpretError;
            break;
    }

    const std::vector<XmlReceptdef*> receptlist = xmlreceptdefs->GetReceptdefList();
    for (size_t i = 0; i < receptlist.size(); ++i)
    {
        auto receptdef = std::make_unique<WW::ReceptDefinitie>(receptlist[i]->Getnaam());
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

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadWeeks(const std::tstring& aDirectory)
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

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadWeek(const std::tstring& aDirectory)
{
    XmlWeekReader reader;
    XmlWeek* xmlweek;
    switch (reader.Read(aDirectory, xmlweek))
    {
        case XmlWeekReader::RESULT_Ok:
            if (xmlweek == nullptr)
                return WW::Result::Ok;
            break;
        case XmlWeekReader::RESULT_FileNotFound:
            return WW::Result::FileNotFound;
        case XmlWeekReader::RESULT_ErrorInFilename:
            return WW::Result::ErrorInFilename;
        case XmlWeekReader::RESULT_FileOpenError:
            return WW::Result::FileOpenError;
        case XmlWeekReader::RESULT_ErrorInFile:
            return WW::Result::ErrorInFile;
        case XmlWeekReader::RESULT_ParserError:
            return WW::Result::ParserError;
        case XmlWeekReader::RESULT_InterpretError:
        case XmlWeekReader::RESULT_WriteError:
        default:
            return WW::Result::InterpretError;
            break;
    }

    auto week = std::make_unique<WW::Week>(Utils::ToDate(xmlweek->Getbegindatum()),
                                           Utils::ToDate(xmlweek->Geteinddatum()));
    week->SetPoints(Str::ToDouble(xmlweek->Getpunten()));
    week->SetSaveablePoints(Str::ToDouble(xmlweek->Getweekpunten()));
    week->SetStartWeight(Str::ToDouble(xmlweek->Getstartweight()));

    switch (xmlweek->Getstrategie())
    {
        case XmlWeek::strategie_FlexiPoints:
        case XmlWeek::strategie_KCal:
            week->SetStrategy(WW::STRATEGY_TYPE::KCal, mModel);
            break;
        case XmlWeek::strategie_CarboHydrates:
            week->SetStrategy(WW::STRATEGY_TYPE::CarboHydrates, mModel);
            break;
    }

    const std::vector<XmlDag*> dagen(xmlweek->GetDagList());
    for (size_t i = 0; i < dagen.size(); ++i)
    {
        auto dag = Create(*dagen[i]);
        if (!week->Add(std::move(dag)))
            return WW::Result::InterpretError;
    }

    if (!mModel.Add(std::move(week)))
        return WW::Result::InterpretError;

    delete xmlweek;

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadGerechten(const std::tstring& aDirectory)
{
    (void)aDirectory;

    return WW::Result::Ok;
}


WW::Result XmlReader::ReadBonusCells(const std::tstring& aDirectory)
{
    XmlBonuslistReader reader;
    XmlBonuslist* xmlbonuslist;

    switch (reader.Read(aDirectory + _T("\\bonuspoints.xml"), xmlbonuslist))
    {
        case XmlWeekReader::RESULT_Ok:
            if (xmlbonuslist == nullptr)
                return WW::Result::Ok;
            break;
        case XmlWeekReader::RESULT_FileNotFound:
            delete xmlbonuslist;
            return WW::Result::FileNotFound;
        case XmlWeekReader::RESULT_ErrorInFilename:
            delete xmlbonuslist;
            return WW::Result::ErrorInFilename;
        case XmlWeekReader::RESULT_FileOpenError:
            delete xmlbonuslist;
            return WW::Result::FileOpenError;
        case XmlWeekReader::RESULT_ErrorInFile:
            delete xmlbonuslist;
            return WW::Result::ErrorInFile;
        case XmlWeekReader::RESULT_ParserError:
            delete xmlbonuslist;
            return WW::Result::ParserError;
        case XmlWeekReader::RESULT_InterpretError:
        case XmlWeekReader::RESULT_WriteError:
        default:
            delete xmlbonuslist;
            return WW::Result::InterpretError;
            break;
    }

    for (size_t i = 0; i < xmlbonuslist->GetBonuscellList().size(); ++i)
    {
        XmlBonuscell* cell = xmlbonuslist->GetBonuscellList()[i];
        WW::BonusPointsMap::MOVEMENT_INTENSITY intensity = WW::BonusPointsMap::MI_High;
        switch (cell->Getintensiteit())
        {
            case XmlBonuscell::intensiteit_hoog:
                intensity = WW::BonusPointsMap::MI_High;
                break;
            case XmlBonuscell::intensiteit_middel:
                intensity = WW::BonusPointsMap::MI_Medium;
                break;
            case XmlBonuscell::intensiteit_laag:
                intensity = WW::BonusPointsMap::MI_Low;
                break;
            default:
                delete xmlbonuslist;
                return WW::Result::InterpretError;
        }

        mModel.GetBonusPointsMap()[intensity][cell->Getgewicht()][(cell->Getminuten())] = cell->Getpunten();
    }

    //mModel.GetBonusPointsMap().Debug();
    delete xmlbonuslist;
    return WW::Result::Ok;
}


WW::Portie XmlReader::Create(const XmlPortie& aPortie)
{
    WW::Portie portie(WW::PortieNaam(mModel, aPortie.Getnaam()));
    portie.SetUnits(Str::ToDouble(aPortie.Geteenheden()));

    return portie;
}


std::unique_ptr<WW::Voedingsmiddel> XmlReader::Create(const XmlVoedingsmiddel& aVoedingsmiddel)
{
    WW::Lot* lot = nullptr;
    if (aVoedingsmiddel.GetStandardlot() != nullptr)
    {
        const XmlStandardlot* xmllot = aVoedingsmiddel.GetStandardlot();
        if (xmllot->GetVoedingswaarde() != nullptr)
        {
            WW::CalculatedLot* plot = new WW::CalculatedLot(mModel.GetCalculator(), Create(xmllot->GetPortie()));
            plot->SetNumberOfPortions(Str::ToDouble(xmllot->Gethoeveelheid()));
            plot->SetKCalPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getkcalper100()));
            plot->SetVetPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getvetper100()));
            plot->SetKoolhydratenPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getkoolhydratenper100().c_str()));
            plot->SetEiwitPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Geteiwitper100().c_str()));
            plot->SetVezelsPer100Units(Str::ToDouble(xmllot->GetVoedingswaarde()->Getvezelsper100().c_str()));
            lot = plot;
        }
        else if (xmllot->GetPuntenper100() != nullptr)
        {
            WW::FixedLot* plot = new WW::FixedLot(Create(xmllot->GetPortie()));
            plot->SetNumberOfPortions(Str::ToDouble(xmllot->Gethoeveelheid()));
            plot->SetPointsPer100Units(Str::ToDouble(xmllot->GetPuntenper100()->Getpunten()));
            lot = plot;
        }
    }
    else
    {
        WW::VMDefinitie* def = mModel.FindVoedingsmiddelDefinitie(aVoedingsmiddel.Getnaam());
        if (def == nullptr)
        {
            ::MessageBox(0, (_T("Unable to interpret voedingsmiddel ") + aVoedingsmiddel.Getnaam()).c_str(),
                         _T("ERROR"), MB_OK);
        }
        else
        {
            if (!def->GetPortieList().empty())
            {
                WW::Portie& portie = *def->GetPortieList()[0];

                if (def->IsCalculated())
                {
                    WW::CalculatedVMDef* cvmdef = def->GetCalculatedVMDef();
                    assert(def != nullptr);
                    WW::CalculatedLot* clot = new WW::CalculatedLot(mModel.GetCalculator(), portie);
                    clot->SetNumberOfPortions(1);
                    clot->SetParameters(cvmdef->GetParameters());
                    lot = clot;
                }
                else if (def->IsFixed())
                {
                    WW::FixedVMDef* fvmdef = def->GetFixedVMDef();
                    assert(def != nullptr);
                    WW::FixedLot* flot = new WW::FixedLot(portie);
                    flot->SetNumberOfPortions(1);
                    flot->SetPointsPer100Units(fvmdef->GetPointsPer100Units());
                    lot = flot;
                }
            }
            else
            {
                assert(false);
            }
        }
    }

    auto voedingsmiddel = std::make_unique<WW::Voedingsmiddel>(aVoedingsmiddel.Getnaam(), lot,
                                                               WW::Unit(mModel, aVoedingsmiddel.Getunit()));
    voedingsmiddel->SetCategory(WW::CategorieNaam(mModel, aVoedingsmiddel.Getcategorie()));
    return std::move(voedingsmiddel);
}


std::unique_ptr<WW::Recept> XmlReader::Create(const XmlRecept& aRecept)
{
    auto recept = std::make_unique<WW::Recept>(aRecept.Getnaam());
    recept->SetPointsPerPortion(Str::ToDouble(aRecept.Getpunten()));
    recept->SetNumberOfPortions(Str::ToDouble(aRecept.Gethoeveelheid()));
    return std::move(recept);
}


std::unique_ptr<WW::Gerecht> XmlReader::Create(const XmlGerecht& aGerecht)
{
    auto gerecht = std::make_unique<WW::Gerecht>(aGerecht.Getnaam(), Str::ToDouble(aGerecht.Getpunten()));
    gerecht->SetNumberOfPortions(Str::ToDouble(aGerecht.Gethoeveelheid()));
    return std::move(gerecht);
}


std::unique_ptr<WW::ManualItem> XmlReader::Create(const XmlHandmatigitem& anItem)
{
    auto item = std::make_unique<WW::ManualItem>(anItem.Getnaam(), Str::ToDouble(anItem.Getpunten()));
    item->Set(Str::ToDouble(anItem.Getpunten()), Str::ToDouble(anItem.Gethoeveelheid()));
    return item;
}


std::unique_ptr<WW::Day> XmlReader::Create(const XmlDag& aDag)
{
    auto day = std::make_unique<WW::Day>(Utils::ToDate(aDag.Getdatum()));
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

    const std::vector<XmlGerecht*>& gerechten(aDag.GetGerechtList());
    for (size_t i = 0; i < gerechten.size(); ++i)
        day->Add(Create(*gerechten[i]));

    const std::vector<XmlHandmatigitem*>& handmatig(aDag.GetHandmatigitemList());
    for (size_t i = 0; i < handmatig.size(); ++i)
        day->Add(Create(*handmatig[i]));

    const std::vector<XmlBonuscell*>& bonuslist(aDag.GetBonuscellList());
    for (size_t i = 0; i < bonuslist.size(); ++i)
        day->Add(Create(*bonuslist[i]));

    return std::move(day);
}


WW::Bonus XmlReader::Create(const XmlBonuscell& aCell)
{
    WW::Bonus::INTENSITY intensity =
        aCell.Getintensiteit() == XmlBonuscell::intensiteit_hoog ? WW::Bonus::INTENSITY::High :
        aCell.Getintensiteit() == XmlBonuscell::intensiteit_middel ? WW::Bonus::INTENSITY::Medium :
        WW::Bonus::INTENSITY::Low;
    return WW::Bonus(intensity, aCell.Getminuten(), aCell.Getpunten());
}


} // namespace ww_1_2