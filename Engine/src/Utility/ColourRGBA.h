//--------------------------------------------------------------------------------------
// ColourRGBA class (cut down version), to hold colours with float values
//--------------------------------------------------------------------------------------
#pragma once

class ColourRGBA
{
// Concrete class - public access
public:
    // Vector components
    float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
    float a = 0.0f;

	//--------------------------------------------------------------------------------------------

	// Default constructor - leaves values uninitialised (for performance)
    ColourRGBA() {}

	// Construct by value
    ColourRGBA( const float rIn, const float gIn, const float bIn, const float aIn = 1.0f)
	{
		r = rIn;
		g = gIn;
		b = bIn;
        a = aIn;
    }
	
	// Set the vector through a pointer to three floats
    void Set( const float* pfElts )
	{
		r = pfElts[0];
		g = pfElts[1];
		b = pfElts[2];
        a = pfElts[3];
    }
};
