#include "TipJar.h"

namespace
{
const std::vector<std::pair<String, String>> productInfos = {
    { "Small Tip ($2)", "chowcentaur_small_tip_123" },
    { "Medium Tip ($5)", "chowcentaur_medium_tip_456" },
    { "Large Tip ($10)", "chowcentaur_large_tip_789" },
    { "Huge Tip ($25)", "chowcentaur_huge_tip_808" },
};
}

TipJar::TipJar()
{
    if (! InAppPurchases::getInstance()->isInAppPurchasesSupported())
    {
        // this should never happen, since we only enable IAPs on iOS!
        jassertfalse;
        return;
    }

    setText ("Tip Jar");
    setColour (backgroundColourId, Colours::transparentBlack);
    setColour (outlineColourId, Colour (0xFF2B3335));
    setColour (textColourId, Colour (0xFF2B3335));
    setColour (arrowColourId, Colour (0xFF2B3335));
    setColour (PopupMenu::backgroundColourId, Colour (0xFF484856));
    setJustificationType (Justification::centred);

    StringArray purchaseIDs;
    for (auto& info : productInfos)
        purchaseIDs.add (info.second);

    setDisconnectedMenu();

    InAppPurchases::getInstance()->addListener (this);
    InAppPurchases::getInstance()->getProductsInformation (purchaseIDs);
}

TipJar::~TipJar()
{
    InAppPurchases::getInstance()->removeListener (this);
}

void TipJar::setDisconnectedMenu()
{
    auto rootMenu = getRootMenu();
    rootMenu->addItem ("Not connected to server!", false, false, {});
}

void TipJar::productsInfoReturned (const Array<InAppPurchases::Product>& products)
{
    if (products.isEmpty())
        return;

    auto rootMenu = getRootMenu();
    rootMenu->clear();
    for (auto& info : productInfos)
    {
        for (int i = 0; i < products.size(); ++i)
        {
            if (info.second == products[i].identifier)
            {
                rootMenu->addItem (info.first, [=] { doTipPurchase (info.second); });
                break;
            }
        }
    }
}

void TipJar::doTipPurchase (const String& id)
{
    InAppPurchases::getInstance()->purchaseProduct (id);
    setText ("Tip Jar");
}
