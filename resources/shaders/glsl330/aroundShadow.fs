#version 330

// Hash without Sine
// MIT License...
/* Copyright (c)2014 David Hoskins.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 textureSize;
uniform float maxDistance;
uniform float spread;
uniform vec4 outlineColor;

// Output fragment color
out vec4 finalColor;

const float threshold = 0.9f;

bool isOpaque(vec4 color){
	return color.a > threshold;
}

#define PI (355.0 / 113.0)
#define SAMPLES 128
//#define RANDOMIZE_BEGINNING true

//Very crude. Worsens with distance. Only use if you're like 3 pixels away.
float shadow(sampler2D tex, vec2 xy, float maxDistance, vec2 pixelToTexture){
	float shad = 0.0;

	//Sample in a spiral. Thanks https://blog.voxagon.se/2018/05/04/bokeh-depth-of-field-in-single-pass.html
	for(int sampleNumber = 1; sampleNumber < SAMPLES; ++sampleNumber){
#ifdef RANDOMIZE_BEGINNING
		float rads = hash12(xy) + float(sampleNumber);	//It's good this isn't in terms of PI, because that means we'll get semi random samples
#else
		float rads = float(sampleNumber);	//It's good this isn't in terms of PI, because that means we'll get semi random samples
#endif
		const float startingDistance = 1.0;
		float distance = startingDistance + ((maxDistance - startingDistance) * (float(sampleNumber) / float(SAMPLES)));
		
		vec2 sampleXY = vec2(cos(rads), sin(rads)) * distance;
		vec4 sample = texture2D(tex, xy + (sampleXY * pixelToTexture));
		shad = max(shad, sample.a * ((maxDistance - distance + startingDistance) / maxDistance)) * spread;
	}
	return shad;
}

void main(){
	vec4 texelColor = texture2D(texture0, fragTexCoord);
	
	float shad = shadow(texture0, fragTexCoord, maxDistance, 1.0 / textureSize);
	
	//If we are inside the opaque object
	if(isOpaque(texelColor)){
		//keep the same color and return.
		finalColor = texelColor;
	}else{
		finalColor = vec4(0.0, 0.0, 0.0, shad);
	}
}