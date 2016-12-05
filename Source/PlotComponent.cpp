/*
  ==============================================================================

    PlotComponent.cpp
    Created: 11 Jul 2015 10:39:09pm
    Author:  Tony Ducks

  ==============================================================================
*/

#include "PlotComponent.h"

/*************************************************************************/
PlotComponent::PlotComponent(AudioSampleBuffer* _buffer,bool _isActive) :isActive(_isActive){
    buffer=_buffer;

    for(int i=0;i<buffer->getNumChannels();i++){
        outMin = buffer->findMinMax(i, 0, buffer->getNumSamples()).getStart();
        outMax = buffer->findMinMax(i, 0, buffer->getNumSamples()).getEnd();
    
        Logger::writeToLog ("outMax: --> " + String(outMax));
        Logger::writeToLog ("outMin: --> " + String(outMin));
        
        float yScale = (outMax - outMin)/5.0f;
        for (int i = 0; i < 6; i++){
            yLabels.push_back(String(outMax - (i * yScale),2));
        }
        Logger::writeToLog ("yLabels size: --> " + String((int)yLabels.size()));
    }
    activeChannel = 0;
    plotColor = Colours::white;
}

/*************************************************************************/
PlotComponent::~PlotComponent(){
}

/*************************************************************************/
void PlotComponent::paint (Graphics& g){
    const int w = getWidth()-GAP;
    const int h = getHeight()-GAP;
    
    g.fillAll (Colour(0xff2f2f2f));
    if ( w > 0 && h > 0){
        g.setOrigin(GAP/2, GAP/2);              //translacion de ejes segun la mitad del GAP
        g.drawImageAt (background, 0, 0);       //imagen de fondo con la cuadricula
        g.setColour (Colours::greenyellow);
        
        g.strokePath (path, PathStrokeType (0.5f));     //curva.... PathStrokeType (2.0f)
        
        float yScale = h / 5.0f;
        g.setColour (Colour::greyLevel (0.5f));
        for (int i = 0; i < 6; i++){
            g.drawText(yLabels[6*yLabelOffset + i], 0 - GAP/2.0, (int) (i * yScale - GAP/4.0),GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
        }
    }
}

/*************************************************************************/
void PlotComponent::resized(){
    const int w = getWidth()-GAP;
    const int h = getHeight()-GAP;

    background = Image (Image::RGB, jmax (1, w), jmax (1, h), false);
    Graphics g (background);
    g.fillAll (Colour(0xff2f2f2f));
    g.setColour (Colour::greyLevel (0.25f));
    g.drawRect(0, 0, w, h);
    float xScale = w / 10.0f;
    float yScale = h / 10.0f;
    for (int i = 1; i < 10; i++){
        g.drawHorizontalLine ((int) (i * yScale), 0.0f, (float) w);
        g.drawVerticalLine ((int) (i * xScale), 0.0f, (float) h);
    }
    refreshPath();
}

/*************************************************************************/
void PlotComponent::refreshPath(){
    float* bufferData = buffer->getWritePointer(activeChannel);
    const int bufferSize = buffer->getNumSamples();
    const int w = getWidth()-GAP;
    const int h = getHeight()-GAP;
    
    const float xScale = (float) w / (float) bufferSize;
    const float yScale = (float) h ;
    float normSample;
    
    path.clear();
    for (int i = 0; i < bufferSize; i++){
        normSample = (bufferData[i] - outMin) / (outMax - outMin);    //se normaliza el sample que se lee del bufferData[i]
        if (i==0) {
            path.startNewSubPath (i * xScale , h  - (normSample * yScale));
        }else{
            path.lineTo (i * xScale , h  - (normSample * yScale));
        }
    }
    repaint();
}

/*************************************************************************/
void PlotComponent::changeActiveChannel (int _channel){
    activeChannel = _channel;
    outMin = buffer->findMinMax(activeChannel, 0, buffer->getNumSamples()).getStart();
    outMax = buffer->findMinMax(activeChannel, 0, buffer->getNumSamples()).getEnd();

    refreshPath();
}

/*************************************************************************/
void PlotComponent::setPlotColor(Colour c){
    plotColor = c;
}

/*************************************************************************/
void PlotComponent::setActive(bool decision){
    isActive=decision;
}

/*************************************************************************/
void PlotComponent::setYlabelOffset(int index){
    yLabelOffset=index;
}
