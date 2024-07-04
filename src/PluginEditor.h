/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ReverseEchoAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer, public juce::Slider::Listener
{
public:
    ReverseEchoAudioProcessorEditor (ReverseEchoAudioProcessor&);
    ~ReverseEchoAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;

  
    void resized() override;
    float mapToCoordMain(float x);
    float mapToCoordDelay(float x);
    void timerCallback() override {
        repaint();
    }
    void chooseCoordAndShow(juce::Graphics& g);

    void sliderValueChanged(juce::Slider* slider) override;
   

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ReverseEchoAudioProcessor& audioProcessor;
    
    

    juce::Slider delaySlider;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverseEchoAudioProcessorEditor)
};
