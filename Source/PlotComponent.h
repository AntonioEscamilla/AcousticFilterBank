/*
  ==============================================================================

    PlotComponent.h
    Created: 11 Jul 2015 10:39:09pm
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef PLOTCOMPONENT_H_INCLUDED
#define PLOTCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

using namespace drow;

//==============================================================================
/*
*/
class PlotComponent    : public Component
{
public:
    PlotComponent(Buffer* _buffer,bool _isActive);
    PlotComponent(OwnedArray<Buffer>* _bufferArray,bool _isActive);
    ~PlotComponent();

    void paint (Graphics&);
    void resized();
    void refreshPath();
    void refillBuffer();
    void setPlotColor(Colour c);
    void setActive(bool decision);
    void changeBuffer(Buffer* _buffer);
    void setYlabelOffset(int i);

    bool                 isActive;
    Image                background;
    Buffer*              buffer;
    Path                 path;
    const int            GAP=100;
    std::vector<String>  yLabels;
    std::vector<String>  xLabels;
    float                outMax;
    float                outMin;
    Colour               plotColor;
    int                  yLabelOffset=0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
};

class AudioWaveForm     : public PlotComponent{
public:
    AudioWaveForm(Buffer* _buffer,bool _isActive):PlotComponent(_buffer,_isActive){}
    
    void paint (Graphics& g){
        const int w = getWidth()-GAP;
        const int h = getHeight()-GAP;
        
        float* bufferData = buffer->getData();
        const int bufferSize = buffer->getSize();
        const float xScale = (float) w / (float) bufferSize;
        const float yScale = (float) h ;
        
        g.fillAll(Colour(0xff2f2f2f));
        if ( w > 0 && h > 0){
            g.setOrigin(GAP/2, GAP/2);              //translacion de ejes segun la mitad del GAP
            g.drawImageAt (background, 0, 0);       //imagen de fondo con la cuadricula
            g.setColour (Colours::greenyellow);
            
            float zero = outMin/(outMin-outMax);    //el cero de la forma de onda del audio en el buffer normalizado
            for (int i = 0; i < bufferSize; i++){
                if (bufferData[i] > zero) {
                    g.drawVerticalLine (i * xScale , h  - (bufferData[i] * yScale), h - (zero * yScale));
                }else{
                    g.drawVerticalLine(i * xScale, h - (zero * yScale) , h  - (bufferData[i] * yScale));
                }
            }
            g.drawLine(0, h - (zero * yScale), w, h - (zero * yScale), 0.3f);
            
            float yScale = h / 5.0f;
            g.setColour (Colour::greyLevel (0.5f));
            for (int i = 0; i < 6; i++){
                g.drawText(yLabels[i], 0 - GAP/2.0, (int) (i * yScale - GAP/4.0),GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
            }
        }
    }
    
    void resized(){
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
        repaint();
    }
    
};

static const char* bandasOctava[10] = {"32","63","125","250","500","1k","2k","4k","8k","16k"};

class OctaveBandPlot    : public PlotComponent{
public:
    OctaveBandPlot(Buffer* _buffer,bool _isActive):PlotComponent(_buffer,_isActive){
        for (int i = 0; i < 10; i++){
            xLabels.push_back(bandasOctava[i]);
        }
    }
    
    OctaveBandPlot(OwnedArray<Buffer>* _bufferArray,bool _isActive):PlotComponent(_bufferArray,_isActive){
        for (int i = 0; i < 10; i++){
            xLabels.push_back(bandasOctava[i]);
        }
    }
    
    void paint (Graphics& g){
        const int w = getWidth()-GAP;
        const int h = getHeight()-GAP;
        
        float* bufferData = buffer->getData();
        const int bufferSize = buffer->getSize();

        
        g.fillAll (Colour(0xff2f2f2f));
        if ( w > 0 && h > 0){
            g.setOrigin(GAP/2, GAP/2);              //translacion de ejes segun la mitad del GAP
            g.drawImageAt (background, 0, 0);       //imagen de fondo con la cuadricula
            
            float yScale = h / 5.0f;
            g.setColour (Colour::greyLevel (0.5f));
            for (int i = 0; i < 6; i++){
                g.drawText(yLabels[6*yLabelOffset + i], 0 - GAP/2.0, (int) (i * yScale - GAP/4.0),GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
            }
            
            float xScale = w / 10.0f;
            g.setOrigin(xScale/2, 0);               //translacion de eje x para pintar en la mitad de las casillas
            
            for (int i = 0; i < 10; i++){
                g.drawText(xLabels[i], (int) (i * xScale - GAP/4.0), h ,GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
            }
            if(isActive){
                g.setColour (plotColor);
                g.strokePath (path, PathStrokeType (2.0f));     //curva
                
                xScale = (float) w / (float) bufferSize;
                yScale = (float) h ;
                float radio = 3.5f;
                for (int i = 0; i < bufferSize; i++){
                    g.drawEllipse(i * xScale - radio, h  - (bufferData[i] * yScale) - radio, 2*radio, 2*radio, 2.5f);
                }
                radio = 1.5f;
                g.setColour (Colours::black);
                for (int i = 0; i < bufferSize; i++){
                    g.drawEllipse(i * xScale - radio, h  - (bufferData[i] * yScale) - radio, 2*radio, 2*radio, 3.0f);
                }
            }
        }
    }
};

//static const char* bandasTercio[31] = {"20","25","32","40","50","63","80","100","125","160","200","250","315","400","500","630","800","1k","1.25k","1.6k","2k","2.5k","3.15k","4k","5k","6k","8k","10k","12.5k","16k","20k"};
static const char* bandasTercio[31] = {" "," ","32"," "," ","63"," "," ","125"," "," ","250"," "," ","500"," "," ","1k"," "," ","2k"," "," ","4k"," "," ","8k"," "," ","16k"," "};

class ThirdBandPlot    : public PlotComponent{
public:
    ThirdBandPlot(Buffer* _buffer,bool _isActive):PlotComponent(_buffer,_isActive){
        for (int i = 0; i < 31; i++){
            xLabels.push_back(bandasTercio[i]);
        }
    }
    
    ThirdBandPlot(OwnedArray<Buffer>* _bufferArray,bool _isActive):PlotComponent(_bufferArray,_isActive){
        for (int i = 0; i < 31; i++){
            xLabels.push_back(bandasTercio[i]);
        }
    }
    
    void paint (Graphics& g){
        const int w = getWidth()-GAP;
        const int h = getHeight()-GAP;
        
        float* bufferData = buffer->getData();
        const int bufferSize = buffer->getSize();
        
        g.fillAll(Colour(0xff2f2f2f));
        if ( w > 0 && h > 0){
            g.setOrigin(GAP/2, GAP/2);              //translacion de ejes segun la mitad del GAP
            g.drawImageAt (background, 0, 0);       //imagen de fondo con la cuadricula
            
            float yScale = h / 5.0f;
            g.setColour (Colour::greyLevel (0.5f));
            for (int i = 0; i < 6; i++){
                g.drawText(yLabels[6*yLabelOffset+i], 0 - GAP/2.0, (int) (i * yScale - GAP/4.0),GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
            }
            
            float xScale = w / 31.0f;
            g.setOrigin(xScale/2, 0);               //translacion de eje x para pintar en la mitad de las casillas
            
            for (int i = 0; i < 31; i++){
                g.drawText(xLabels[i], (int) (i * xScale - GAP/4.0), h ,GAP/2.0,GAP/2.0, Justification::horizontallyCentred);
            }
            
            if(isActive){
                g.setColour (plotColor);
                g.strokePath (path, PathStrokeType (2.0f));     //curva
                
                xScale = (float) w / (float) bufferSize;
                yScale = (float) h ;
                float radio = 3.5f;
                for (int i = 0; i < bufferSize; i++){
                    g.drawEllipse(i * xScale - radio, h  - (bufferData[i] * yScale) - radio, 2*radio, 2*radio, 2.5f);
                }
                radio = 1.5f;
                g.setColour (Colours::black);
                for (int i = 0; i < bufferSize; i++){
                    g.drawEllipse(i * xScale - radio, h  - (bufferData[i] * yScale) - radio, 2*radio, 2*radio, 3.0f);
                }
            }
        }
    }
};

#endif  // PLOTCOMPONENT_H_INCLUDED
