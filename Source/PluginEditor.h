#pragma once
#include "PluginProcessor.h"

class RusticStudioLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RusticStudioLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool isHighlighted, bool isDown) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class AnalogVuMeter : public juce::Component, private juce::Timer
{
public:
    explicit AnalogVuMeter (BenBocazStudioProProcessor& proc);
    void paint (juce::Graphics& g) override;
    void setShowGainReduction (bool showGr) { showGR = showGr; }
    bool isShowingGainReduction() const { return showGR; }

private:
    void timerCallback() override;
    BenBocazStudioProProcessor& processor;
    bool showGR = false;
    float smoothedNeedle = 0.0f;
};

class Studio4MicStagePad : public juce::Component, private juce::Timer
{
public:
    explicit Studio4MicStagePad (juce::AudioProcessorValueTreeState& apvtsRef);
    void setRoomImage (const juce::Image& img) { roomImage = img; repaint(); }
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

private:
    void timerCallback() override { repaint(); }
    juce::Point<float> getStandScreenPos (int idx, juce::Rectangle<float> area) const;
    void updateStandFromMouse (int idx, const juce::MouseEvent& e);

    juce::AudioProcessorValueTreeState& apvts;
    juce::Image roomImage;
    int selectedStand = 0;
};

class BenBocazStudioProEditor : public juce::AudioProcessorEditor
{
public:
    explicit BenBocazStudioProEditor (BenBocazStudioProProcessor&);
    ~BenBocazStudioProEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void loadEmbeddedImages();
    void setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void setupCombo (juce::ComboBox& box, const juce::StringArray& items, juce::Label& label, const juce::String& text);
    void openIRFileChooser();

    BenBocazStudioProProcessor& audioProcessor;
    RusticStudioLookAndFeel rusticLAF;
    AnalogVuMeter vuMeter;
    Studio4MicStagePad roomPad;
    juce::Image chassisImage;

    juce::ToggleButton phaseAlignBtn { "PHASE SAFE" };
    juce::ToggleButton phaseInvBtn   { "POLARITY INV" };
    juce::ToggleButton vocalModBtn   { "VOCAL MIC MOD" };
    juce::ToggleButton chamberOnBtn  { "ON" };
    juce::ToggleButton eqOnBtn       { "EQ ON" };
    juce::ToggleButton compOnBtn     { "COMP ON" };

    juce::TextButton vuModeBtn { "VU: OUT" };
    juce::TextButton loadIRBtn { "LOAD IR (.WAV)" };
    juce::Label irNameLbl;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ComboBox modeBox, chainBox, vocalSrcBox, vocalTgtBox;
    juce::ComboBox sourceBox, monitorBox, roomBox, wallsBox;
    juce::ComboBox chamberBox, eqTypeBox, compBox;

    juce::Label modeLbl, chainLbl, vocalSrcLbl, vocalTgtLbl;
    juce::Label sourceLbl, monitorLbl, roomLbl, wallsLbl;
    juce::Label chamberLbl, eqTypeLbl, compLbl;

    juce::Slider driveSlider;
    juce::Slider mic1GainSlider, mic2GainSlider, mic3GainSlider, mic4GainSlider;
    juce::Slider chMixSlider, chDecaySlider, chPreSlider, chWidthSlider;
    juce::Slider eqHpfSlider, eqLowSlider, eqMidSlider, eqHighSlider;
    juce::Slider compThreshSlider, outGainSlider;

    juce::Label driveLbl;
    juce::Label mic1GainLbl, mic2GainLbl, mic3GainLbl, mic4GainLbl;
    juce::Label chMixLbl, chDecayLbl, chPreLbl, chWidthLbl;
    juce::Label eqHpfLbl, eqLowLbl, eqMidLbl, eqHighLbl;
    juce::Label compThreshLbl, outGainLbl;

    using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAtt> phaseAlignAtt, phaseInvAtt, vocalModAtt, chamberOnAtt, eqOnAtt, compOnAtt;
    std::unique_ptr<ComboAtt>  modeAtt, chainAtt, vocalSrcAtt, vocalTgtAtt;
    std::unique_ptr<ComboAtt>  sourceAtt, monitorAtt, roomAtt, wallsAtt;
    std::unique_ptr<ComboAtt>  chamberAtt, eqTypeAtt, compAtt;

    std::unique_ptr<SliderAtt> driveAtt;
    std::unique_ptr<SliderAtt> mic1GainAtt, mic2GainAtt, mic3GainAtt, mic4GainAtt;
    std::unique_ptr<SliderAtt> chMixAtt, chDecayAtt, chPreAtt, chWidthAtt;
    std::unique_ptr<SliderAtt> eqHpfAtt, eqLowAtt, eqMidAtt, eqHighAtt;
    std::unique_ptr<SliderAtt> compThreshAtt, outGainAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BenBocazStudioProEditor)
};
