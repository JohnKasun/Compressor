#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    setSize (800, 600);

    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels (2, 2);
    }

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(gainSlider);

    addAndMakeVisible(thresholdLabel);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);


}

MainComponent::~MainComponent()
{

    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
 
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{

}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    thresholdSlider.setBounds(20, 20, getWidth() - 20, getHeight() - 300);
}
