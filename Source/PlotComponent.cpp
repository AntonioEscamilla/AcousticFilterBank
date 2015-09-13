/*
  ==============================================================================

    PlotComponent.cpp
    Created: 11 Jul 2015 10:39:09pm
    Author:  Tony Ducks

  ==============================================================================
*/

#include "PlotComponent.h"

/*************************************************************************/
PlotComponent::PlotComponent(Buffer* _buffer,bool _isActive) :isActive(_isActive)/*,isInitialized (false)*/{
    buffer=_buffer;
    float* bufferData = buffer->getData();
    const int bufferSize = buffer->getSize();
    
    myNormalise(bufferData, bufferSize,outMin,outMax);
    
    Logger::writeToLog ("outMax: --> " + String(outMax));
    Logger::writeToLog ("outMin: --> " + String(outMin));
    
    float yScale = (outMax - outMin)/5.0f;
    for (int i = 0; i < 6; i++){
        yLabels.push_back(String(outMax - (i * yScale),2));
    }
    plotColor = Colours::white;
}

/*************************************************************************/
PlotComponent::PlotComponent(OwnedArray<Buffer>* _bufferArray,bool _isActive) :isActive(_isActive){
    
    for(int i=0;i<_bufferArray->size();i++){
        myNormalise(_bufferArray->getUnchecked(i)->getData(),_bufferArray->getUnchecked(i)->getSize(),outMin,outMax);
        Logger::writeToLog ("outMax: --> " + String(outMax));
        Logger::writeToLog ("outMin: --> " + String(outMin));
        
        float yScale = (outMax - outMin)/5.0f;
        for (int i = 0; i < 6; i++){
            yLabels.push_back(String(outMax - (i * yScale),2));
        }
        Logger::writeToLog ("yLabels size: --> " + String((int)yLabels.size()));
    }
    
    buffer=_bufferArray->getFirst();
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
    float* bufferData = buffer->getData();
    const int bufferSize = buffer->getSize();
    const int w = getWidth()-GAP;
    const int h = getHeight()-GAP;
    
    const float xScale = (float) w / (float) bufferSize;
    const float yScale = (float) h ;
    
    path.clear();
    for (int i = 0; i < bufferSize; i++){
        if (i==0) {
            path.startNewSubPath (i * xScale , h  - (bufferData[i] * yScale));
        }else{
            path.lineTo (i * xScale , h  - (bufferData[i] * yScale));
        }
    }
    repaint();
}

/*************************************************************************/
void PlotComponent::changeBuffer (Buffer* _buffer){
    buffer=_buffer;
//    float* bufferData = buffer->getData();
//    const int bufferSize = buffer->getSize();
//    
//    myNormalise(bufferData, bufferSize,outMin,outMax);
//    yLabels.clear();
//    float yScale = (outMax - outMin)/5.0f;
//    for (int i = 0; i < 6; i++){
//        yLabels.push_back(String(outMax - (i * yScale),2));
//    }
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
