/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent():readAheadThread("read Ahead thread"){
    // Format manager
    audioFormatManager.registerBasicFormats();
    
    // Device manager
    audioDeviceManager = new AudioDeviceManager();
    audioDeviceManager->initialise(0, 2, 0, true);
    readAheadThread.startThread(3);
    
    // Read file
    File sfile (File::getSpecialLocation (File::userDocumentsDirectory).getChildFile("lyd3_000_ortf_48k.wav"));
    AudioFormatReader* audioFormatReader = audioFormatManager.createReaderFor(sfile);
    ScopedPointer<AudioSampleBuffer> audioBuffer = new AudioSampleBuffer(1, audioFormatReader->lengthInSamples); //audioBuffer para leer archivo wav
    audioFormatReader->read(audioBuffer, 0, audioFormatReader->lengthInSamples, 0, true, false);
    
    // Cambiar Fs del AudioDevice segun sea la del archivo wav
    juce::AudioDeviceManager::AudioDeviceSetup newAudioSetup;
    audioDeviceManager->getAudioDeviceSetup(newAudioSetup);
    newAudioSetup.sampleRate=audioFormatReader->sampleRate;
    audioDeviceManager->setAudioDeviceSetup(newAudioSetup, true);
    
    //Playback preparation
    audioFormatReaderSource = new AudioFormatReaderSource(audioFormatReader, true);
    audioTransportSource.setSource(audioFormatReaderSource, 32768, &readAheadThread, 0, 2);  //el 0 es para que no haga resamplig....
    audioSourcePlayer.setSource(&audioTransportSource);                                      //..depende de haber cambiado Fs en AudioDevice
    audioDeviceManager->addAudioCallback(&audioSourcePlayer);
    //Playback start
    audioTransportSource.start();
    
    Logger::writeToLog ("Total length: --> " + String(audioFormatReader->lengthInSamples));
    
    int bandas=30;
    for (int i=0;i<bandas;i++) {
        AudioSampleBuffer* buffer = new AudioSampleBuffer(1,audioFormatReader->lengthInSamples);
        filteredAudioArray.add(buffer);         //filteredAudioArray es un OwnedArray y debe ser declarado como una variable de la clase para que
    }                                           //le pertenezca y sea la clase la que lo borre.
    
    filterBank = new FilterBank(bandas);        //La clase filterBank necesita un WritePointer a un canal de un AudioSampleBuffer donde esta la entrada...
    filterBank->setCoeficientes();              //y un puntero a un OwnedArray<Buffer> del tamaño del filterBank donde quedan diferentes salidas filtradas
    filterBank->processSamples(audioBuffer->getWritePointer(0),&filteredAudioArray, audioFormatReader->lengthInSamples);
    
    const int N = 1;                                                        //downsamplig rate
    const int M = 1;                                                        //length fraction
    bufferWaveform = new AudioSampleBuffer(1,audioFormatReader->lengthInSamples/(M*N));  //buffer para downSamplig con el que se pinta waveForm
    bufferWaveform->clear();
    audioDownSamplig(audioBuffer,bufferWaveform,N);
    
    addAndMakeVisible(tabsComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop));
    tabsComponent->addTab("Respuesta al Impulso", Colour(0xff2f2f2f), new AudioWaveForm(bufferWaveform,true), true);
    for(int i=0;i<bandas;i++){
        tabsComponent->addTab("Filtered IR", Colour(0xff2f2f2f), new AudioWaveForm(filteredAudioArray.getUnchecked(i),true), true);
    }

    setSize (1200, 400);
}

//==============================================================================
MainContentComponent::~MainContentComponent(){
}

//==============================================================================
void MainContentComponent::paint (Graphics& g){
    
}

//==============================================================================
void MainContentComponent::resized(){
    const int w = getWidth();
    const int h = getHeight();
    const int gap = 250;
        
    tabsComponent->setBounds(gap, 0, w-gap-5, h-5);
}

//==============================================================================
void MainContentComponent::audioDownSamplig(AudioSampleBuffer* input, AudioSampleBuffer* output,int downSampligFactor){
    
    int sampleCounter=0;
    float* dataWaveform = output->getWritePointer(0);
    ScopedPointer<AudioSampleBuffer>  auxBuf = new AudioSampleBuffer(1, downSampligFactor);
    auxBuf->clear();
    int sampleIndexModulo;
    
    for(int j = 0; j < input->getNumSamples(); j++){
        float sample = input->getSample(0, j);
        sampleIndexModulo = j%downSampligFactor;
        auxBuf->setSample(0, sampleIndexModulo, sample);
        if(sampleIndexModulo==0){
            sampleCounter++;
            if (sampleCounter < output->getNumSamples()) {
                if (sample > 0.0f)
                    dataWaveform[sampleCounter] = auxBuf->findMinMax(0, 0, auxBuf->getNumSamples()).getEnd();
                else
                    dataWaveform[sampleCounter] = auxBuf->findMinMax(0, 0, auxBuf->getNumSamples()).getStart();
            }
        }
    }
}
