#pragma once

#include <pch.h>

/** Custom LookAndFeel for ComboBoxes */
class ComboBoxLNF : public chowdsp::ChowLNF
{
public:
    ComboBoxLNF() = default;
    ~ComboBoxLNF() override = default;

    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area, const bool isSeparator, const bool isActive, const bool isHighlighted, const bool /*isTicked*/, const bool hasSubMenu, const String& text, const String& shortcutKeyText, const Drawable* icon, const Colour* const textColourToUse) override
    {
        LookAndFeel_V4::drawPopupMenuItem (g, area, isSeparator, isActive, isHighlighted, false /*isTicked*/, hasSubMenu, text, shortcutKeyText, icon, textColourToUse);
    }

    Component* getParentComponentForMenuOptions (const PopupMenu::Options& options) override
    {
#if JUCE_IOS
        if (PluginHostType::getPluginLoadedAs() == AudioProcessor::wrapperType_AudioUnitv3)
        {
            if (options.getParentComponent() == nullptr && options.getTargetComponent() != nullptr)
                return options.getTargetComponent()->getTopLevelComponent();
        }
#endif
        return LookAndFeel_V2::getParentComponentForMenuOptions (options);
    }

    void drawPopupMenuBackground (Graphics& g, int width, int height) override
    {
        g.fillAll (findColour (PopupMenu::backgroundColourId));
        ignoreUnused (width, height);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComboBoxLNF)
};

class ButtonLNF : public chowdsp::ChowLNF
{
public:
    ButtonLNF() {}

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        constexpr auto cornerSize = 3.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                              .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (Colour (0xFF2B3335));
        g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
    }

    Font getTextButtonFont (TextButton&, int buttonHeight) override
    {
        return Font (jmin (17.0f, (float) buttonHeight * 0.8f)).boldened();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonLNF)
};
