
#include "XmlItemCreateVisitor.h"

#include <Generated2019/XmlDag.h>
#include <Generated2019/XmlGerecht.h>
#include <Generated2019/XmlHandmatigitem.h>
#include <Generated2019/XmlPortie.h>
#include <Generated2019/XmlRecept.h>
#include <Generated2019/XmlReceptDef.h>
#include <Generated2019/XmlVoedingsMiddel.h>

#include "model/Gerecht.h"
#include "model/Recept.h"
#include "model/Voedingsmiddel.h"

#include "XmlWriter.h"

namespace ww2019
{


std::unique_ptr<XmlVoedingsmiddel> XmlItemCreateVisitor::Create(WW::Voedingsmiddel& aVoedingsmiddel)
{
    auto voedingsmiddel = std::make_unique<XmlVoedingsmiddel>();
    mWriter.Create(aVoedingsmiddel, *voedingsmiddel);
    return voedingsmiddel;
}


std::unique_ptr<XmlRecept> XmlItemCreateVisitor::Create(WW::Recept& aRecept)
{
    auto recept = std::make_unique<XmlRecept>();
    mWriter.Create(aRecept, *recept);
    return recept;
}


std::unique_ptr<XmlGerecht> XmlItemCreateVisitor::Create(WW::Gerecht& aGerecht)
{
    auto gerecht = std::make_unique<XmlGerecht>();
    mWriter.Create(aGerecht, *gerecht);
    return gerecht;
}


std::unique_ptr<XmlHandmatigitem> XmlItemCreateVisitor::Create(WW::ManualItem& anItem)
{
    auto item = std::make_unique<XmlHandmatigitem>();
    mWriter.Create(anItem, *item);
    return item;
}


void XmlDagItemCreateVisitor::Visit(WW::Recept& aRecept)
{
    mDag.Add(Create(aRecept));
}


void XmlDagItemCreateVisitor::Visit(WW::Gerecht& aGerecht)
{
    mDag.Add(Create(aGerecht));
}


void XmlDagItemCreateVisitor::Visit(WW::Voedingsmiddel& aVoedingsmiddel)
{
    mDag.Add(Create(aVoedingsmiddel));
}


void XmlDagItemCreateVisitor::Visit(WW::ManualItem& anItem)
{
    mDag.Add(Create(anItem));
}


void XmlReceptItemCreateVisitor::Visit(WW::Recept& aRecept)
{
    mReceptDef.Add(Create(aRecept));
}


void XmlReceptItemCreateVisitor::Visit(WW::Gerecht& aGerecht)
{
    mReceptDef.Add(Create(aGerecht));
}


void XmlReceptItemCreateVisitor::Visit(WW::Voedingsmiddel& aVoedingsmiddel)
{
    mReceptDef.Add(Create(aVoedingsmiddel));
}


void XmlReceptItemCreateVisitor::Visit(WW::ManualItem& anItem)
{
    mReceptDef.Add(Create(anItem));
}


} // namespace WW