/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverseEchoAudioProcessor::ReverseEchoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (int i = 0; i < 250; ++i)
    {
        soundLine.push_back(0.0);
        delayLine.push_back(0.0);
    }
    
    for (int i = 0; i < globalSamplingRate * 4; ++i)
    {
        delayDataLeft.push_back(0.0);
        delayDataRight.push_back(0.0);
    }
}

ReverseEchoAudioProcessor::~ReverseEchoAudioProcessor()
{
}

//==============================================================================
const juce::String ReverseEchoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverseEchoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverseEchoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverseEchoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverseEchoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverseEchoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverseEchoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverseEchoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverseEchoAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverseEchoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverseEchoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    globalSamplingRate = sampleRate;
}

void ReverseEchoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverseEchoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReverseEchoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto* channelDataL = buffer.getReadPointer(0);
    auto* outputChannelDataL = buffer.getWritePointer(0);

    auto* channelDataR = buffer.getReadPointer(1);
    auto* outputChannelDataR = buffer.getWritePointer(1);
    
    
    float avgFloatMain = 0.0;
    float avgFloatDelay = 0.0;



    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float inputSampleL = channelDataL[sample];
        float inputSampleR = channelDataR[sample];
        
        int moddedIndex = delayIndex % currentDelayLength;
        
        avgFloatMain = avgFloatMain + ((inputSampleL + inputSampleR) / 2);
        avgFloatDelay = avgFloatDelay + ((delayDataLeft[moddedIndex] + delayDataRight[moddedIndex]) / 2);
        
        inputSampleL = inputSampleL + (delayDataLeft[moddedIndex] * 0.5);
        inputSampleR = inputSampleR + (delayDataRight[moddedIndex] * 0.5);
        
        delayDataLeft[moddedIndex] = delayDataLeft[moddedIndex] + inputSampleL;
        delayDataRight[moddedIndex] = delayDataRight[moddedIndex] + inputSampleR;
        
        delayDataLeft[moddedIndex] = delayDataLeft[moddedIndex] * 0.5;
        delayDataRight[moddedIndex] = delayDataRight[moddedIndex] * 0.5;
        
        
        delayIndex += 1;
        
        outputChannelDataL[sample] = inputSampleL;
        outputChannelDataR[sample] = inputSampleR;
    }
    
    avgFloatMain = avgFloatMain / buffer.getNumSamples();
    avgFloatDelay = avgFloatDelay / buffer.getNumSamples();
    
    if(soundLine.size() > 0) {
    
        for (int i = (int)(soundLine.size()) - 1; i > 0; --i)
        {
            soundLine[i] = soundLine[i - 1];
            delayLine[i] = delayLine[i - 1];
        }
        
        soundLine[0] = avgFloatMain;
        delayLine[0] = avgFloatDelay;
    }
    

    
    

}

void ReverseEchoAudioProcessor::setDelayTime(double delayTime)
{
    
    long newDelayLength = delayTime * globalSamplingRate * 4;
    currentDelayLength = newDelayLength;
    
}


//==============================================================================
bool ReverseEchoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverseEchoAudioProcessor::createEditor()
{
    return new ReverseEchoAudioProcessorEditor (*this);
}

//==============================================================================
void ReverseEchoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, true);
}

void ReverseEchoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverseEchoAudioProcessor();
}
