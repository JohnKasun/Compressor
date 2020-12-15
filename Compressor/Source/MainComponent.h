#pragma once

#include <JuceHeader.h>
#include "DecibelSlider.h"


class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void bypassButtonPressed();

private:
    juce::TextButton bypassButton{ "bypass" };

    DecibelSlider thresholdSlider;
    DecibelSlider gainSlider;

    juce::Label thresholdLabel;
    juce::Label gainLabel;

    bool Bypass{ true };
    int mBufferSize{ 0 };
    double mSampleRate{ 0.0 };
    float gain{ 1.0 };
    float threshold{ 0.0 };
    float ratio{ 2.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

