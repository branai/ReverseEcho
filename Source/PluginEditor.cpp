/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverseEchoAudioProcessorEditor::ReverseEchoAudioProcessorEditor (ReverseEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(500, 500);
    startTimer(1.0 / 44100.0);
    
    // Initialize delay slider
    delaySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary); // Set the slider style to vertical
    delaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    delaySlider.setRange(juce::Range<double>(0.01, 1.0), 0.01); // Adjust range as needed
    delaySlider.addListener(this);
    addAndMakeVisible(&delaySlider);
        
}

ReverseEchoAudioProcessorEditor::~ReverseEchoAudioProcessorEditor()
{
}

//==============================================================================
void ReverseEchoAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);

    for (int i = 0; i < audioProcessor.soundLine.size(); ++i)
    {
        
        float xMain = i + 125.0;
        float yMain = mapToCoordMain(audioProcessor.soundLine[i]);
        float yDelay = mapToCoordDelay(audioProcessor.delayLine[i]);
        
        g.drawLine(xMain, 100, xMain, yMain);
        g.drawLine(xMain, 300, xMain, yDelay);
        
    }
    

}



float ReverseEchoAudioProcessorEditor::mapToCoordMain(float x) {
    return (x * 100) + 100;
}

float ReverseEchoAudioProcessorEditor::mapToCoordDelay(float x) {
    return (x * 100) + 300;
}

void ReverseEchoAudioProcessorEditor::resized()
{
    delaySlider.setBounds(200,412 ,100,75); // Adjust position and size as needed
}

void ReverseEchoAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &delaySlider)
    {
        audioProcessor.setDelayTime(slider->getValue());
    }
}
