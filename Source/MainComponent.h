/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlotComponent.h"
#include "FilterBank.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();
    void paint (Graphics&);
    void resized();
    void audioDownSamplig(AudioSampleBuffer* input, AudioSampleBuffer* output,int downSampligFactor);
    
private:
    AudioFormatManager                      audioFormatManager;
    ScopedPointer<AudioFormatReaderSource>  audioFormatReaderSource;
    ScopedPointer<AudioDeviceManager>       audioDeviceManager;
    AudioTransportSource                    audioTransportSource;
    AudioSourcePlayer                       audioSourcePlayer;
    TimeSliceThread                         readAheadThread;
    
    ScopedPointer<AudioSampleBuffer>        bufferWaveform;
    OwnedArray<AudioSampleBuffer>           filteredAudioArray;
    ScopedPointer<TabbedComponent>          tabsComponent;
    
    ScopedPointer<FilterBank>               filterBank;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
