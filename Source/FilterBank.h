/*
  ==============================================================================

    FilterBank.h
    Created: 12 Sep 2015 2:56:38pm
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef FILTERBANK_H_INCLUDED
#define FILTERBANK_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

using namespace drow;

class BicuadFilter{
public:
    BicuadFilter();
    ~BicuadFilter();
    
    void makeInactive();
    void setCoeficientes (const double* _coeff,const double* G);
    void reset();
    void processSamples (float* samples, int numSamples);

protected:
    
    double      coeff[5];
    float       u1, u2;
    bool        active;
    JUCE_LEAK_DETECTOR (BicuadFilter)
};

//==============================================================================
class BandFilter{
public:
    BandFilter(int numBqs);
    ~BandFilter();
    void processSamples (float* samples, int numSamples);
    void reset();
    
    OwnedArray<BicuadFilter>        bicuadFiltersArray;    
    JUCE_LEAK_DETECTOR (BandFilter)
};

//==============================================================================
class FilterBank{
public:
    FilterBank(int bandas);
    ~FilterBank();
    void setCoeficientes();
    void processSamples(float* const input, OwnedArray<Buffer>* outputs, int numSamples);
    void processSamples(float* const input,float* const output,int numSamples);
    
protected:
    OwnedArray<BandFilter>          bandFiltersArray;
    int                             bandFilterOrder;
    int                             numeroBandas;
    JUCE_LEAK_DETECTOR (FilterBank)
};



#endif  // FILTERBANK_H_INCLUDED
