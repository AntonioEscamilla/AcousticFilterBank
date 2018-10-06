# AcousticFilterBank

## Octave and Third Octave Filter Bank implementation 
C++ and JUCE Implementation of a Filter Bank for Impulse Response Processing.

A FilterBank is implemented as an OwnedArray of BandFilter objects, an each BandFilter is implemented as a OwnedArray of BicuadFilter objects.

Coefficients and Gains are provided for each Bicuad Filter as a simple Second Order IIR System.
