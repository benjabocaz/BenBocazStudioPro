#include "PluginEditor.h"

#if HAS_CUSTOM_ASSETS
 #include "BinaryData.h"
#endif

//==============================================================================
// RusticStudioLookAndFeel
//==============================================================================
RusticStudioLookAndFeel::RusticStudioLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1a1511));
    setColour (juce::ComboBox::textColourId, juce::Colour (0xfff2e6d0));
    setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff6b563c));
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1b1612));
    setColour (juce::PopupMenu::textColourId, juce::Colour (0xfff2e6d0));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xffd69d46));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colour (0xff14100d));
    setColour (juce::TextButton::textColourOffId, juce::Colour (0xfff2e6d0));
    setColour (juce::TextButton::textColourOnId, juce::Colour (0xff14100d));
}

void RusticStudioLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                                juce::Slider&)
{
    auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                          static_cast<float> (width), static_cast<float> (height)).reduced (7.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float cx = bounds.getCentreX();
    const float cy = bounds.getCentreY();
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::Path bgArc;
    bgArc.addCentredArc (cx, cy, radius + 3.5f, radius + 3.5f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (juce::Colour (0xff342b22));
    g.strokePath (bgArc, juce::PathStrokeType (2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valArc;
    valArc.addCentredArc (cx, cy, radius + 3.5f, radius + 3.5f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (juce::Colour (0xffe5a94c));
    g.strokePath (valArc, juce::PathStrokeType (2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour (juce::Colours::black.withAlpha (0.65f));
    g.fillEllipse (cx - radius + 1.5f, cy - radius + 3.5f, radius * 2.0f, radius * 2.0f);

    juce::ColourGradient body (juce::Colour (0xff443a31), cx - radius * 0.5f, cy - radius * 0.5f,
                               juce::Colour (0xff15110e), cx + radius * 0.6f, cy + radius * 0.6f, true);
    g.setGradientFill (body);
    g.fillEllipse (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    juce::ColourGradient cap (juce::Colour (0xff5c4e3f), cx - radius * 0.3f, cy - radius * 0.4f,
                              juce::Colour (0xff26201a), cx + radius * 0.4f, cy + radius * 0.4f, true);
    g.setGradientFill (cap);
    g.fillEllipse (cx - radius * 0.72f, cy - radius * 0.72f, radius * 1.44f, radius * 1.44f);

    g.setColour (juce::Colour (0xffb08b55).withAlpha (0.55f));
    g.drawEllipse (cx - radius * 0.72f, cy - radius * 0.72f, radius * 1.44f, radius * 1.44f, 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.6f, -radius * 0.88f, 3.2f, radius * 0.52f, 1.6f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (cx, cy));
    g.setColour (juce::Colour (0xfffaf1df));
    g.fillPath (pointer);
}

void RusticStudioLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                            int, int, int, int, juce::ComboBox&)
{
    auto area = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height)).reduced (0.5f);
    g.setColour (juce::Colour (0xff18130f));
    g.fillRoundedRectangle (area, 4.0f);
    g.setColour (juce::Colour (0xff6b573e));
    g.drawRoundedRectangle (area, 4.0f, 1.0f);

    juce::Path arrow;
    const float ax = static_cast<float> (width) - 14.0f;
    const float ay = static_cast<float> (height) * 0.5f - 2.0f;
    arrow.addTriangle (ax, ay, ax + 8.0f, ay, ax + 4.0f, ay + 5.0f);
    g.setColour (juce::Colour (0xffe5a94c));
    g.fillPath (arrow);
}

void RusticStudioLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour&, bool isHighlighted, bool isDown)
{
    auto area = button.getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (isDown ? juce::Colour (0xffd69d46) : (isHighlighted ? juce::Colour (0xff3c3024) : juce::Colour (0xff231c16)));
    g.fillRoundedRectangle (area, 4.0f);
    g.setColour (juce::Colour (0xffc99546));
    g.drawRoundedRectangle (area, 4.0f, 1.1f);
}

void RusticStudioLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                                bool isHighlighted, bool)
{
    auto area = button.getLocalBounds().toFloat().reduced (0.5f);
    const bool isOn = button.getToggleState();

    g.setColour (isOn ? juce::Colour (0xff3b2b18) : (isHighlighted ? juce::Colour (0xff29221c) : juce::Colour (0xff171310)));
    g.fillRoundedRectangle (area, 4.0f);
    g.setColour (isOn ? juce::Colour (0xffe8ab4c) : juce::Colour (0xff544535));
    g.drawRoundedRectangle (area, 4.0f, isOn ? 1.4f : 1.0f);

    const float ledR = 4.2f;
    const float ledX = area.getX() + 10.0f;
    const float ledY = area.getCentreY();

    if (isOn)
    {
        g.setColour (juce::Colour (0xffffb84d).withAlpha (0.35f));
        g.fillEllipse (ledX - ledR * 1.8f, ledY - ledR * 1.8f, ledR * 3.6f, ledR * 3.6f);
        g.setColour (juce::Colour (0xffffc866));
    }
    else
    {
        g.setColour (juce::Colour (0xff3d332a));
    }
    g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);

    g.setColour (isOn ? juce::Colour (0xfffaf0dc) : juce::Colour (0xff9c8e7d));
    g.setFont (juce::Font (juce::FontOptions (9.8f).withStyle ("Bold")));
    g.drawText (button.getButtonText(), area.withTrimmedLeft (18.0f).reduced (2.0f, 0.0f), juce::Justification::centredLeft);
}

//==============================================================================
// AnalogVuMeter
//==============================================================================
AnalogVuMeter::AnalogVuMeter (BenBocazStudioProProcessor& proc) : processor (proc) { startTimerHz (30); }

void AnalogVuMeter::timerCallback()
{
    float target = 0.0f;
    if (showGR)
        target = 1.0f - (std::clamp (processor.getCompGrDb(), 0.0f, 20.0f) / 20.0f);
    else
        target = juce::jmap (std::clamp (processor.getOutputVuDb(), -36.0f, 3.0f), -36.0f, 3.0f, 0.0f, 1.0f);

    smoothedNeedle = smoothedNeedle * 0.72f + target * 0.28f;
    repaint();
}

void AnalogVuMeter::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (juce::Colour (0xff120e0b));
    g.fillRoundedRectangle (r, 5.0f);
    g.setColour (juce::Colour (0xff786043));
    g.drawRoundedRectangle (r, 5.0f, 1.5f);

    auto glass = r.reduced (3.5f);
    juce::ColourGradient lamp (juce::Colour (0xfff8db96), glass.getCentreX(), glass.getBottom(),
                               juce::Colour (0xffb88130), glass.getCentreX(), glass.getY(), true);
    g.setGradientFill (lamp);
    g.fillRoundedRectangle (glass, 3.0f);

    const float pivotX = glass.getCentreX();
    const float pivotY = glass.getBottom() + 14.0f;
    const float rad = glass.getHeight() + 5.0f;

    juce::Path scaleArc;
    scaleArc.addCentredArc (pivotX, pivotY, rad * 0.86f, rad * 0.86f, 0.0f, -0.75f, 0.75f, true);
    g.setColour (juce::Colour (0xff2b1e0d));
    g.strokePath (scaleArc, juce::PathStrokeType (1.4f));

    juce::Path redArc;
    redArc.addCentredArc (pivotX, pivotY, rad * 0.86f, rad * 0.86f, 0.0f, 0.35f, 0.75f, true);
    g.setColour (juce::Colour (0xffb52318));
    g.strokePath (redArc, juce::PathStrokeType (2.4f));

    g.setColour (juce::Colour (0xff2b1e0d));
    g.setFont (juce::Font (juce::FontOptions (9.5f).withStyle ("Bold")));
    g.drawText (showGR ? "GR  dB" : "VU  OUT", glass.reduced (6.0f, 3.0f), juce::Justification::bottomLeft);
    g.drawText ("-20    -10    -5    0   +3", glass.reduced (8.0f, 4.0f), juce::Justification::centredTop);

    const float needleAngle = juce::jmap (smoothedNeedle, 0.0f, 1.0f, -0.75f, 0.75f);
    juce::Path needle;
    needle.startNewSubPath (pivotX, pivotY);
    needle.lineTo (pivotX + std::sin (needleAngle) * (rad * 0.92f),
                   pivotY - std::cos (needleAngle) * (rad * 0.92f));
    g.setColour (juce::Colour (0xff1a1209));
    g.strokePath (needle, juce::PathStrokeType (1.8f));
}

//==============================================================================
// Studio4MicStagePad
//==============================================================================
Studio4MicStagePad::Studio4MicStagePad (juce::AudioProcessorValueTreeState& apvtsRef) : apvts (apvtsRef)
{
    startTimerHz (24);
}

juce::Point<float> Studio4MicStagePad::getStandScreenPos (int idx, juce::Rectangle<float> area) const
{
    const juce::String pfx = "mic" + juce::String (idx + 1);
    const float mx = apvts.getRawParameterValue (pfx + "X")->load();
    const float my = apvts.getRawParameterValue (pfx + "Y")->load();

    const float sx = juce::jmap (mx, -1.0f, 1.0f, area.getX() + 42.0f, area.getRight() - 42.0f);
    const float sy = juce::jmap (my,  0.0f, 1.0f, area.getY() + 68.0f, area.getBottom() - 26.0f);
    return { sx, sy };
}

void Studio4MicStagePad::paint (juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced (2.0f);

    juce::Path clip;
    clip.addRoundedRectangle (area, 6.0f);
    g.saveState();
    g.reduceClipRegion (clip);

    if (roomImage.isValid())
    {
        g.drawImage (roomImage, area, juce::RectanglePlacement::fillDestination);
        g.setColour (juce::Colour (0xff140e09).withAlpha (0.42f));
        g.fillRect (area);
    }
    else
    {
        juce::ColourGradient wood (juce::Colour (0xff3b2618), area.getTopLeft(),
                                   juce::Colour (0xff1f140c), area.getBottomRight(), false);
        g.setGradientFill (wood);
        g.fillRect (area);
        auto rug = area.reduced (55.0f, 38.0f);
        g.setColour (juce::Colour (0xff6e2d1e).withAlpha (0.65f));
        g.fillRoundedRectangle (rug, 4.0f);
        g.setColour (juce::Colour (0xffd69d46).withAlpha (0.45f));
        g.drawRoundedRectangle (rug.reduced (5.0f), 3.0f, 1.5f);
    }
    g.restoreState();

    const int roomIdx   = static_cast<int> (apvts.getRawParameterValue ("room")->load());
    const int wallsLive = static_cast<int> (apvts.getRawParameterValue ("walls")->load());
    const int sourceIdx = static_cast<int> (apvts.getRawParameterValue ("source")->load());
    const bool phaseOk  = apvts.getRawParameterValue ("phaseAlign")->load() >= 0.5f;

    g.setColour (wallsLive ? juce::Colour (0xffd9a04a) : juce::Colour (0xff5c4f41));
    g.drawRoundedRectangle (area, 6.0f, wallsLive ? 2.0f : 1.4f);

    auto topBar = area.reduced (8.0f).removeFromTop (16.0f);
    g.setColour (juce::Colour (0xff140f0b).withAlpha (0.72f));
    g.fillRoundedRectangle (topBar.expanded (4.0f, 2.0f), 3.0f);

    g.setColour (juce::Colour (0xfff4d08c));
    g.setFont (juce::Font (juce::FontOptions (10.0f).withStyle ("Bold")));
    juce::String roomTitle = (roomIdx == 0 ? "SHANGRI-LA LIVE ROOM" : "CANYON WOOD ROOM");
    roomTitle += (wallsLive ? "  •  CEDAR LIVE" : "  •  DAMPED");
    g.drawText (roomTitle, topBar, juce::Justification::centredLeft);

    g.setColour (phaseOk ? juce::Colour (0xff8ce08e) : juce::Colour (0xffe8a94c));
    g.drawText (phaseOk ? "PHASE LOCKED" : "FREE ACOUSTIC DELAY", topBar, juce::Justification::centredRight);

    const float srcX = area.getCentreX();
    const float srcY = area.getY() + 42.0f;

    g.setColour (juce::Colour (0xff120d09).withAlpha (0.82f));
    if (sourceIdx == 3)
    {
        g.fillRoundedRectangle (srcX - 42.0f, srcY - 11.0f, 84.0f, 22.0f, 4.0f);
        g.setColour (juce::Colour (0xfff2e6d0));
        g.drawRoundedRectangle (srcX - 42.0f, srcY - 11.0f, 84.0f, 22.0f, 4.0f, 1.3f);
        g.setFont (juce::Font (juce::FontOptions (9.2f).withStyle ("Bold")));
        g.drawText ("MONITORS L/R", static_cast<int> (srcX - 42), static_cast<int> (srcY - 11), 84, 22, juce::Justification::centred);
    }
    else
    {
        g.fillRoundedRectangle (srcX - 34.0f, srcY - 11.0f, 68.0f, 22.0f, 4.0f);
        g.setColour (juce::Colour (0xfff2e6d0));
        g.drawRoundedRectangle (srcX - 34.0f, srcY - 11.0f, 68.0f, 22.0f, 4.0f, 1.3f);
        juce::String lbl = (sourceIdx == 0 ? "VOCALS" : (sourceIdx == 1 ? "DRUMS" : "ENSEMBLE"));
        g.setFont (juce::Font (juce::FontOptions (9.2f).withStyle ("Bold")));
        g.drawText (lbl, static_cast<int> (srcX - 34), static_cast<int> (srcY - 11), 68, 22, juce::Justification::centred);
    }

    static const std::array<juce::String, 4> names { "MAIN", "MID", "AIR", "DECCA" };
    static const std::array<juce::Colour, 4> cols {
        juce::Colour (0xfff2b24c),
        juce::Colour (0xffe07a52),
        juce::Colour (0xff8fc7e8),
        juce::Colour (0xfff5ebd6)
    };

    const float roomD = (roomIdx == 0) ? 12.0f : 8.5f;

    for (int i = 0; i < 4; ++i)
    {
        const auto pos = getStandScreenPos (i, area);
        const float gain = apvts.getRawParameterValue ("mic" + juce::String (i + 1) + "Gain")->load();
        const float my   = apvts.getRawParameterValue ("mic" + juce::String (i + 1) + "Y")->load();
        const float distM = juce::jmap (my, 0.0f, 1.0f, 0.8f, roomD - 1.5f);
        const float alpha = gain > 0.01f ? 1.0f : 0.35f;

        g.setColour (cols[static_cast<size_t> (i)].withAlpha (0.28f * alpha));
        g.drawLine (srcX, srcY + 11.0f, pos.x, pos.y, i == selectedStand ? 1.8f : 1.0f);

        if (i == 3)
        {
            g.setColour (cols[3].withAlpha (0.8f * alpha));
            g.drawLine (pos.x - 16.0f, pos.y + 6.0f, pos.x + 16.0f, pos.y + 6.0f, 1.6f);
            g.drawLine (pos.x, pos.y - 6.0f, pos.x, pos.y + 6.0f, 1.6f);
            g.fillEllipse (pos.x - 19.0f, pos.y + 3.0f, 6.0f, 6.0f);
            g.fillEllipse (pos.x + 13.0f, pos.y + 3.0f, 6.0f, 6.0f);
        }

        if (i == selectedStand)
        {
            g.setColour (cols[static_cast<size_t> (i)].withAlpha (0.28f));
            g.fillEllipse (pos.x - 15.0f, pos.y - 15.0f, 30.0f, 30.0f);
        }

        g.setColour (juce::Colour (0xff120d0a).withAlpha (0.85f));
        g.fillEllipse (pos.x - 10.0f, pos.y - 10.0f, 20.0f, 20.0f);
        g.setColour (cols[static_cast<size_t> (i)].withAlpha (alpha));
        g.drawEllipse (pos.x - 9.5f, pos.y - 9.5f, 19.0f, 19.0f, 2.0f);

        g.setFont (juce::Font (juce::FontOptions (8.8f).withStyle ("Bold")));
        g.drawText (juce::String (i + 1), static_cast<int> (pos.x - 9), static_cast<int> (pos.y - 9), 18, 18, juce::Justification::centred);

        const juce::String tag = names[static_cast<size_t> (i)] + " " + juce::String (distM, 1) + "m";
        g.setColour (juce::Colour (0xff120d0a).withAlpha (0.78f));
        g.fillRoundedRectangle (pos.x - 32.0f, pos.y + 11.0f, 64.0f, 13.0f, 3.0f);
        g.setColour (cols[static_cast<size_t> (i)].withAlpha (alpha));
        g.setFont (juce::Font (juce::FontOptions (8.4f).withStyle ("Bold")));
        g.drawText (tag, static_cast<int> (pos.x - 32), static_cast<int> (pos.y + 11), 64, 13, juce::Justification::centred);
    }

    g.setColour (juce::Colour (0xfff2e6d0).withAlpha (0.7f));
    g.setFont (juce::Font (juce::FontOptions (9.0f)));
    g.drawText ("DRAG ANY OF THE 4 MIC STANDS (1:MAIN  2:MID  3:AIR  4:DECCA)",
                area.reduced (6.0f).removeFromBottom (13.0f), juce::Justification::centred);
}

void Studio4MicStagePad::mouseDown (const juce::MouseEvent& e)
{
    auto area = getLocalBounds().toFloat().reduced (2.0f);
    float bestDist = 1e9f;
    int bestIdx = selectedStand;

    for (int i = 0; i < 4; ++i)
    {
        const float d = getStandScreenPos (i, area).getDistanceFrom (e.position);
        if (d < bestDist) { bestDist = d; bestIdx = i; }
    }

    selectedStand = bestIdx;
    updateStandFromMouse (selectedStand, e);
}

void Studio4MicStagePad::mouseDrag (const juce::MouseEvent& e)
{
    updateStandFromMouse (selectedStand, e);
}

void Studio4MicStagePad::updateStandFromMouse (int idx, const juce::MouseEvent& e)
{
    auto area = getLocalBounds().toFloat().reduced (2.0f);
    const float normX = juce::jlimit (0.0f, 1.0f, juce::jmap (static_cast<float> (e.x), area.getX() + 42.0f, area.getRight() - 42.0f, 0.0f, 1.0f));
    const float normY = juce::jlimit (0.0f, 1.0f, juce::jmap (static_cast<float> (e.y), area.getY() + 68.0f, area.getBottom() - 26.0f, 0.0f, 1.0f));

    const juce::String pfx = "mic" + juce::String (idx + 1);
    if (auto* pX = apvts.getParameter (pfx + "X")) pX->setValueNotifyingHost (normX);
    if (auto* pY = apvts.getParameter (pfx + "Y")) pY->setValueNotifyingHost (normY);
}

//==============================================================================
// BenBocazStudioProEditor
//==============================================================================
BenBocazStudioProEditor::BenBocazStudioProEditor (BenBocazStudioProProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), vuMeter (p), roomPad (p.apvts)
{
    setLookAndFeel (&rusticLAF);
    setSize (1160, 720);

    loadEmbeddedImages();

    addAndMakeVisible (vuMeter);
    addAndMakeVisible (roomPad);

    addAndMakeVisible (phaseAlignBtn);
    addAndMakeVisible (phaseInvBtn);
    addAndMakeVisible (vocalModBtn);
    addAndMakeVisible (chamberOnBtn);
    addAndMakeVisible (eqOnBtn);
    addAndMakeVisible (compOnBtn);

    vuModeBtn.onClick = [this]
    {
        const bool next = ! vuMeter.isShowingGainReduction();
        vuMeter.setShowGainReduction (next);
        vuModeBtn.setButtonText (next ? "VU: GR" : "VU: OUT");
    };
    addAndMakeVisible (vuModeBtn);

    loadIRBtn.onClick = [this] { openIRFileChooser(); };
    addAndMakeVisible (loadIRBtn);

    irNameLbl.setText (audioProcessor.getLoadedIRName(), juce::dontSendNotification);
    irNameLbl.setJustificationType (juce::Justification::centredLeft);
    irNameLbl.setColour (juce::Label::textColourId, juce::Colour (0xffc9b99e));
    irNameLbl.setFont (juce::Font (juce::FontOptions (10.0f)));
    addAndMakeVisible (irNameLbl);

    setupCombo (modeBox,     { "Reverb", "Re-Mic" }, modeLbl, "MODE");
    setupCombo (chainBox,    { "EQ -> Comp -> Chamber", "Comp -> EQ -> Chamber", "Chamber -> Comp -> EQ" }, chainLbl, "SIGNAL ROUTING");
    setupCombo (vocalSrcBox, { "Shure SM27", "Shure SM57", "Shure SM7B", "Generic LDC" }, vocalSrcLbl, "YOUR INPUT MIC");
    setupCombo (vocalTgtBox, { "Sony C-800G", "Neumann TLM 103", "Aston Origin" }, vocalTgtLbl, "TARGET VOCAL MIC");
    setupCombo (sourceBox,   { "Vocals", "Drums", "Ensemble", "Monitors (Swedien)" }, sourceLbl, "ROOM SOURCE");
    setupCombo (monitorBox,  { "Altec Main", "NS-10 Nearfield", "JBL PA Wedge" }, monitorLbl, "MONITOR SPEAKER");
    setupCombo (roomBox,     { "Studio A (Shangri-La)", "Studio B (Canyon)" }, roomLbl, "STUDIO ROOM");
    setupCombo (wallsBox,    { "Tight / Damped", "Live / Cedar Wood" }, wallsLbl, "ROOM WALLS");
    setupCombo (chamberBox,  { "Chamber 1 (Bright)", "Chamber 3 (Classic)", "Chamber 5 (Deep)", "Chamber 8 (Dense)", "Custom WAV IR" }, chamberLbl, "ECHO CHAMBER");
    setupCombo (eqTypeBox,   { "Neve 1073 (British)", "Pultec EQP-1A (Tube)", "API 550A (Punch)", "SSL 4000E (Clean)" }, eqTypeLbl, "EQ MODEL");
    setupCombo (compBox,     { "1176LN (FET)", "LA-2A (Opto)", "Mastering (Tube)" }, compLbl, "COMP MODEL");

    setupKnob (driveSlider,      driveLbl,      "TUBE / TAPE DRIVE");
    setupKnob (mic1GainSlider,   mic1GainLbl,   "1: MAIN LVL");
    setupKnob (mic2GainSlider,   mic2GainLbl,   "2: MID LVL");
    setupKnob (mic3GainSlider,   mic3GainLbl,   "3: AIR LVL");
    setupKnob (mic4GainSlider,   mic4GainLbl,   "4: DECCA LVL");
    setupKnob (chMixSlider,      chMixLbl,      "SEND");
    setupKnob (chDecaySlider,    chDecayLbl,    "DECAY");
    setupKnob (chPreSlider,      chPreLbl,      "PRE-DELAY");
    setupKnob (chWidthSlider,    chWidthLbl,    "WIDTH");
    setupKnob (eqHpfSlider,      eqHpfLbl,      "HPF");
    setupKnob (eqLowSlider,      eqLowLbl,      "LOW");
    setupKnob (eqMidSlider,      eqMidLbl,      "MID");
    setupKnob (eqHighSlider,     eqHighLbl,     "HIGH");
    setupKnob (compThreshSlider, compThreshLbl, "THRESHOLD");
    setupKnob (outGainSlider,    outGainLbl,    "OUTPUT");

    auto& v = audioProcessor.apvts;
    phaseAlignAtt = std::make_unique<ButtonAtt> (v, "phaseAlign", phaseAlignBtn);
    phaseInvAtt   = std::make_unique<ButtonAtt> (v, "phaseInvert", phaseInvBtn);
    vocalModAtt   = std::make_unique<ButtonAtt> (v, "vocalModOn", vocalModBtn);
    chamberOnAtt  = std::make_unique<ButtonAtt> (v, "chamberOn", chamberOnBtn);
    eqOnAtt       = std::make_unique<ButtonAtt> (v, "eqOn", eqOnBtn);
    compOnAtt     = std::make_unique<ButtonAtt> (v, "compOn", compOnBtn);

    modeAtt     = std::make_unique<ComboAtt> (v, "mode", modeBox);
    chainAtt    = std::make_unique<ComboAtt> (v, "chainOrder", chainBox);
    vocalSrcAtt = std::make_unique<ComboAtt> (v, "vocalSrcMic", vocalSrcBox);
    vocalTgtAtt = std::make_unique<ComboAtt> (v, "vocalTgtMic", vocalTgtBox);
    sourceAtt   = std::make_unique<ComboAtt> (v, "source", sourceBox);
    monitorAtt  = std::make_unique<ComboAtt> (v, "monitorType", monitorBox);
    roomAtt     = std::make_unique<ComboAtt> (v, "room", roomBox);
    wallsAtt    = std::make_unique<ComboAtt> (v, "walls", wallsBox);
    chamberAtt  = std::make_unique<ComboAtt> (v, "chamberType", chamberBox);
    eqTypeAtt   = std::make_unique<ComboAtt> (v, "eqType", eqTypeBox);
    compAtt     = std::make_unique<ComboAtt> (v, "compType", compBox);

    driveAtt      = std::make_unique<SliderAtt> (v, "consoleDrive", driveSlider);
    mic1GainAtt   = std::make_unique<SliderAtt> (v, "mic1Gain", mic1GainSlider);
    mic2GainAtt   = std::make_unique<SliderAtt> (v, "mic2Gain", mic2GainSlider);
    mic3GainAtt   = std::make_unique<SliderAtt> (v, "mic3Gain", mic3GainSlider);
    mic4GainAtt   = std::make_unique<SliderAtt> (v, "mic4Gain", mic4GainSlider);
    chMixAtt      = std::make_unique<SliderAtt> (v, "chamberMix", chMixSlider);
    chDecayAtt    = std::make_unique<SliderAtt> (v, "chamberDecay", chDecaySlider);
    chPreAtt      = std::make_unique<SliderAtt> (v, "chamberPreDelay", chPreSlider);
    chWidthAtt    = std::make_unique<SliderAtt> (v, "chamberWidth", chWidthSlider);
    eqHpfAtt      = std::make_unique<SliderAtt> (v, "eqHpf", eqHpfSlider);
    eqLowAtt      = std::make_unique<SliderAtt> (v, "eqLow", eqLowSlider);
    eqMidAtt      = std::make_unique<SliderAtt> (v, "eqMid", eqMidSlider);
    eqHighAtt     = std::make_unique<SliderAtt> (v, "eqHigh", eqHighSlider);
    compThreshAtt = std::make_unique<SliderAtt> (v, "compThresh", compThreshSlider);
    outGainAtt    = std::make_unique<SliderAtt> (v, "outputGain", outGainSlider);
}

BenBocazStudioProEditor::~BenBocazStudioProEditor() { setLookAndFeel (nullptr); }

void BenBocazStudioProEditor::loadEmbeddedImages()
{
#if HAS_CUSTOM_ASSETS
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
    {
        juce::String resName (BinaryData::namedResourceList[i]);
        int dataSize = 0;
        if (const char* data = BinaryData::getNamedResource (BinaryData::namedResourceList[i], dataSize))
        {
            if (resName.containsIgnoreCase ("room"))
                roomPad.setRoomImage (juce::ImageCache::getFromMemory (data, dataSize));
            else if (resName.containsIgnoreCase ("chassis") || resName.containsIgnoreCase ("wood"))
                chassisImage = juce::ImageCache::getFromMemory (data, dataSize);
        }
    }
#endif
}

void BenBocazStudioProEditor::openIRFileChooser()
{
    fileChooser = std::make_unique<juce::FileChooser> ("Select Studio or Chamber IR (.wav)", juce::File{}, "*.wav;*.aiff;*.aif");
    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync (flags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            audioProcessor.loadCustomIR (file);
            irNameLbl.setText (audioProcessor.getLoadedIRName(), juce::dontSendNotification);
            chamberBox.setSelectedItemIndex (4, juce::sendNotification);
        }
    });
}

void BenBocazStudioProEditor::setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 15);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe0d1b8));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffe6ab50));
    label.setFont (juce::Font (juce::FontOptions (9.8f).withStyle ("Bold")));
    addAndMakeVisible (label);
}

void BenBocazStudioProEditor::setupCombo (juce::ComboBox& box, const juce::StringArray& items, juce::Label& label, const juce::String& text)
{
    box.addItemList (items, 1);
    addAndMakeVisible (box);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredLeft);
    label.setColour (juce::Label::textColourId, juce::Colour (0xffc9b89e));
    label.setFont (juce::Font (juce::FontOptions (9.2f).withStyle ("Bold")));
    addAndMakeVisible (label);
}

void BenBocazStudioProEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    if (chassisImage.isValid())
    {
        g.drawImage (chassisImage, bounds.toFloat(), juce::RectanglePlacement::fillDestination);
        g.setColour (juce::Colour (0xff140f0b).withAlpha (0.55f));
        g.fillRect (bounds);
    }
    else
    {
        juce::ColourGradient bg (juce::Colour (0xff2b221a), 0.0f, 0.0f,
                                 juce::Colour (0xff140f0c), 0.0f, static_cast<float> (getHeight()), false);
        g.setGradientFill (bg);
        g.fillRect (bounds);
    }

    auto drawWoodRail = [&] (juce::Rectangle<int> r)
    {
        juce::ColourGradient wood (juce::Colour (0xff5c361c), static_cast<float> (r.getX()), 0.0f,
                                   juce::Colour (0xff261308), static_cast<float> (r.getRight()), 0.0f, false);
        g.setGradientFill (wood);
        g.fillRect (r);
        g.setColour (juce::Colours::black.withAlpha (0.3f));
        for (int x = r.getX() + 3; x < r.getRight(); x += 5)
            g.drawVerticalLine (x, 0.0f, static_cast<float> (getHeight()));
    };
    drawWoodRail (bounds.removeFromLeft (18));
    drawWoodRail (bounds.removeFromRight (18));

    auto header = bounds.removeFromTop (76);
    g.setColour (juce::Colour (0xff120e0b).withAlpha (0.90f));
    g.fillRect (header);
    g.setColour (juce::Colour (0xffe5a94c).withAlpha (0.5f));
    g.drawHorizontalLine (header.getBottom() - 1, static_cast<float> (header.getX()), static_cast<float> (header.getRight()));

    g.setColour (juce::Colour (0xfff6ead4));
    g.setFont (juce::Font (juce::FontOptions (22.0f).withStyle ("Bold")));
    g.drawText ("BEN BOCAZ STUDIO PRO", header.reduced (14, 10).removeFromTop (28), juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xffe5a94c));
    g.setFont (juce::Font (juce::FontOptions (10.0f).withStyle ("Bold")));
    g.drawText ("RUSTIC ANALOG ROOMS  •  4-STAND RE-MIC  •  VOCAL MIC MOD",
                header.reduced (14, 0).withTrimmedTop (42), juce::Justification::centredLeft);

    auto drawPanel = [&] (juce::Rectangle<int> r, const juce::String& title)
    {
        g.setColour (juce::Colour (0xff1a1511).withAlpha (0.86f));
        g.fillRoundedRectangle (r.toFloat(), 6.0f);
        g.setColour (juce::Colour (0xff614e39).withAlpha (0.8f));
        g.drawRoundedRectangle (r.toFloat(), 6.0f, 1.15f);

        g.setColour (juce::Colour (0xff8c7250));
        g.fillEllipse (static_cast<float> (r.getX() + 6), static_cast<float> (r.getY() + 6), 5.5f, 5.5f);
        g.fillEllipse (static_cast<float> (r.getRight() - 12), static_cast<float> (r.getY() + 6), 5.5f, 5.5f);

        g.setColour (juce::Colour (0xffe6ab50));
        g.setFont (juce::Font (juce::FontOptions (10.5f).withStyle ("Bold")));
        g.drawText (title, r.reduced (18, 5).removeFromTop (16), juce::Justification::centredLeft);
    };

    drawPanel ({ 24,   86, 270, 366 }, "VOCAL MIC MOD & SOURCE");
    drawPanel ({ 306,  86, 520, 366 }, "SHANGRI-LA 2D STAGE — 4 MIC STANDS");
    drawPanel ({ 838,  86, 298, 366 }, "ECHO CHAMBERS & IR");
    drawPanel ({ 24,  464, 560, 236 }, "ANALOG CONSOLE EQ");
    drawPanel ({ 596, 464, 382, 236 }, "OUTBOARD COMPRESSOR");
    drawPanel ({ 990, 464, 146, 236 }, "MASTER");

    g.setColour (juce::Colour (0xff614e39).withAlpha (0.55f));
    g.drawHorizontalLine (190, 36.0f, 282.0f);
}

void BenBocazStudioProEditor::resized()
{
    modeLbl.setBounds       (376, 12, 95, 14);  modeBox.setBounds       (376, 28, 102, 26);
    chainLbl.setBounds      (486, 12, 165, 14); chainBox.setBounds      (486, 28, 174, 26);
    phaseAlignBtn.setBounds (670, 12, 112, 24);
    phaseInvBtn.setBounds   (670, 40, 112, 24);
    vuMeter.setBounds       (794, 8,  250, 60);
    vuModeBtn.setBounds     (1054, 22, 82, 28);

    vocalModBtn.setBounds (36,  112, 246, 24);
    vocalSrcLbl.setBounds (36,  142, 118, 13); vocalSrcBox.setBounds (36,  156, 118, 23);
    vocalTgtLbl.setBounds (164, 142, 118, 13); vocalTgtBox.setBounds (164, 156, 118, 23);

    sourceLbl.setBounds   (36,  198, 118, 13); sourceBox.setBounds   (36,  212, 118, 23);
    monitorLbl.setBounds  (164, 198, 118, 13); monitorBox.setBounds  (164, 212, 118, 23);
    roomLbl.setBounds     (36,  244, 118, 13); roomBox.setBounds     (36,  258, 118, 23);
    wallsLbl.setBounds    (164, 244, 118, 13); wallsBox.setBounds    (164, 258, 118, 23);

    driveLbl.setBounds    (92,  302, 134, 14);
    driveSlider.setBounds (113, 320, 92, 102);

    roomPad.setBounds (318, 110, 496, 236);

    mic1GainLbl.setBounds (328, 354, 106, 13); mic1GainSlider.setBounds (338, 368, 86, 76);
    mic2GainLbl.setBounds (450, 354, 106, 13); mic2GainSlider.setBounds (460, 368, 86, 76);
    mic3GainLbl.setBounds (572, 354, 106, 13); mic3GainSlider.setBounds (582, 368, 86, 76);
    mic4GainLbl.setBounds (694, 354, 106, 13); mic4GainSlider.setBounds (704, 368, 86, 76);

    chamberOnBtn.setBounds (1052, 90, 72, 20);
    chamberLbl.setBounds   (852, 116, 160, 13); chamberBox.setBounds (852, 130, 270, 24);
    loadIRBtn.setBounds    (852, 162, 116, 24); irNameLbl.setBounds  (974, 162, 148, 24);

    chMixLbl.setBounds   (874, 200, 92, 13); chMixSlider.setBounds   (877, 214, 86, 90);
    chDecayLbl.setBounds (1004, 200, 92, 13); chDecaySlider.setBounds (1007, 214, 86, 90);
    chPreLbl.setBounds   (874, 322, 92, 13); chPreSlider.setBounds   (877, 336, 86, 90);
    chWidthLbl.setBounds (1004, 322, 92, 13); chWidthSlider.setBounds (1007, 336, 86, 90);

    eqOnBtn.setBounds   (496, 469, 76, 22);
    eqTypeLbl.setBounds (42,  498, 80, 14); eqTypeBox.setBounds (122, 494, 220, 24);

    eqHpfLbl.setBounds  (52,  538, 96, 14); eqHpfSlider.setBounds  (52,  554, 96, 110);
    eqLowLbl.setBounds  (182, 538, 96, 14); eqLowSlider.setBounds  (182, 554, 96, 110);
    eqMidLbl.setBounds  (312, 538, 96, 14); eqMidSlider.setBounds  (312, 554, 96, 110);
    eqHighLbl.setBounds (442, 538, 96, 14); eqHighSlider.setBounds (442, 554, 96, 110);

    compOnBtn.setBounds     (886, 469, 80, 22);
    compLbl.setBounds       (614, 536, 175, 14); compBox.setBounds       (614, 554, 185, 28);
    compThreshLbl.setBounds (830, 538, 102, 14); compThreshSlider.setBounds (833, 554, 96, 110);

    outGainLbl.setBounds    (1015, 538, 96, 14); outGainSlider.setBounds (1015, 554, 96, 110);
}
