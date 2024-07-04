/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/**
*/

struct PointData {
    juce::Point<float> point;
    std::pair<float, float> vel;
};


class ReverseEchoAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReverseEchoAudioProcessor();
    ~ReverseEchoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    double softClip(float inputValue, float softClipFactor);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //std::deque<juce::Point<float>> movingPoints;
    std::vector<float> soundLine;//250);
    std::vector<float> delayLine;//250);
    std::vector<float> delayDataLeft;//250);
    std::vector<float> delayDataRight;//250);
    
    int delayIndex = 0;
    
    
    
    
    void setDelayTime(double delayTime);
    
    

private:
    
    long currentDelayLength = 441;
    double globalSamplingRate = 44100.0;
    //std::mutex delayTimeMutex;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverseEchoAudioProcessor)
};
