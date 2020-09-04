#include "MyLNF.h"

MyLNF::MyLNF()
{
    roboto = Typeface::createSystemTypefaceFor (BinaryData::RobotoCondensedRegular_ttf,
                                                BinaryData::RobotoCondensedRegular_ttfSize);

    robotoBold = Typeface::createSystemTypefaceFor (BinaryData::RobotoCondensedBold_ttf,
                                                    BinaryData::RobotoCondensedBold_ttfSize);
}

Typeface::Ptr MyLNF::getTypefaceForFont (const Font& font)
{
    return font.isBold() ? robotoBold : roboto;
}

void MyLNF::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos, float rotaryStartAngle,
                              float rotaryEndAngle, juce::Slider& slider)
{
    int diameter = (width > height)? height : width;
    if (diameter < 16) return;

    juce::Point<float> centre (x + std::floor (width * 0.5f + 0.5f), y + std::floor (height * 0.5f + 0.5f));
    diameter -= (diameter % 2)? 9 : 8;
    float radius = diameter * 0.5f;
    x = int (centre.x - radius);
    y = int (centre.y - radius);

    const auto bounds = juce::Rectangle<int> (x, y, diameter, diameter).toFloat();   

    auto b = pointer->getBounds().toFloat();
    auto b2 = knob->getBounds().toFloat();
    pointer->setTransform (AffineTransform::rotation (MathConstants<float>::twoPi * ((sliderPos - 0.5f) * 300.0f / 360.0f),
        b.getCentreX(), b.getCentreY()));

    auto knobBounds = (bounds * 0.75f).withCentre (centre);
    knob->drawWithin (g, knobBounds, RectanglePlacement::stretchToFit, 1.0f);
    pointer->drawWithin (g, knobBounds, RectanglePlacement::stretchToFit, 1.0f);

    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const juce::Colour fill = slider.findColour (juce::Slider::rotarySliderFillColourId);
    constexpr float arcFactor = 0.9f;

    Path valueArc;
    valueArc.addPieSegment (bounds, rotaryStartAngle, rotaryEndAngle, arcFactor);
    g.setColour (Colour (0xff484856));
    g.fillPath (valueArc);
    valueArc.clear();

    valueArc.addPieSegment (bounds, rotaryStartAngle, toAngle, arcFactor);
    g.setColour (slider.findColour (Slider::trackColourId));
    g.fillPath (valueArc);
}

void MyLNF::drawToggleButton (Graphics& g, ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize = jmin (15.0f, (float) button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;

    drawTickBox (g, button, 4.0f, ((float) button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (Font (fontSize).boldened());

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds().withTrimmedLeft (roundToInt (tickWidth) + 10)
                                             .withTrimmedRight (2),
                      Justification::centredLeft, 10);
}

void MyLNF::positionComboBoxText (ComboBox& box, Label& label)
{
    LookAndFeel_V4::positionComboBoxText (box, label);
    label.setFont (label.getFont().boldened());
    label.setJustificationType (Justification::centred);
}
