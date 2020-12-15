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

    addAndMakeVisible(bypassButton);
    bypassButton.setClickingTogglesState(true);
    bypassButton.setToggleState(true, true);
    bypassButton.onClick = [this] { bypassButtonPressed(); };

    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setValue(threshold);
    thresholdSlider.setRange(-40.0, 0.0);
    thresholdSlider.onValueChange = [this] { threshold = (float)thresholdSlider.getValue(); };

    addAndMakeVisible(gainSlider);
    gainSlider.setValue(juce::Decibels::gainToDecibels(gain));
    gainSlider.setRange(-10.0, 10.0);
    gainSlider.onValueChange = [this] { gain = juce::Decibels::decibelsToGain((float)gainSlider.getValue()); };

    addAndMakeVisible(thresholdLabel);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setText("threshold", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("gain", juce::NotificationType::dontSendNotification);


}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mBufferSize = samplesPerBlockExpected;
    mSampleRate = sampleRate;
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    auto currentGain = gain;
    auto currentThreshold = threshold;

    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();

    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeInputChannels.getHighestBit() + 1;

    for (auto channel = 0; channel < maxOutputChannels; channel++)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels;

            if (!activeInputChannels[channel])
            {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel, bufferToFill.startSample);
                auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

                if (!Bypass)
                {
                    for (auto sample = 0; sample < bufferToFill.numSamples; sample++)
                    {
                        auto currentSamp = inBuffer[sample];
                        auto sampLevel = juce::Decibels::gainToDecibels(std::abs(currentSamp));
                        if (sampLevel > currentThreshold)
                        {
                            auto sampCompDB = ((sampLevel - currentThreshold) / ratio) + currentThreshold;
                            auto sampCompGAIN = juce::Decibels::decibelsToGain(sampCompDB);

                            if (currentSamp < 0)
                                outBuffer[sample] = -1.0 * sampCompGAIN * currentGain;
                            else
                                outBuffer[sample] = sampCompGAIN * currentGain;
                        }
                        else
                            outBuffer[sample] = currentSamp * currentGain;

                    }
                }
                else
                {
                    for (auto sample = 0; sample < bufferToFill.numSamples; sample++)
                        outBuffer[sample] = inBuffer[sample];
                }

            }

        }
    }
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
    auto area = getLocalBounds();

    bypassButton.setBounds(20, 150, 60, 20);
    thresholdSlider.setBounds(40, 30, area.reduced(5).getWidth() - 30 , 30);
    gainSlider.setBounds(40, 80, area.reduced(5).getWidth() - 30, 30);
}

void MainComponent::bypassButtonPressed()
{
    Bypass = !Bypass;
}



