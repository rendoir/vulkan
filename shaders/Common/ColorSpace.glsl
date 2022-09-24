// Color space
vec4 SRGBtoLinear(vec4 srgbIn) 
{
	return vec4(pow(srgbIn.rgb, vec3(c_gamma)), srgbIn.a);
}

vec3 SRGBtoLinear(vec3 srgbIn)
{
	return pow(srgbIn, vec3(c_gamma));
}

vec4 LinearToSRGB(vec4 linearIn)
{
	return vec4(pow(linearIn.rgb, vec3(1.0 / c_gamma)), linearIn.a);
}

vec3 LinearToSRGB(vec3 linearIn)
{
	return pow(linearIn, vec3(1.0 / c_gamma));
}

void ApplyGammaCorrection(inout vec3 color)
{
	color = LinearToSRGB(color);
}

// Uncharted2 Tonemapping
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void ApplyHDRTonemapping(inout vec3 color)
{
	float W = 11.2;

	vec3 current = Uncharted2Tonemap(color * c_exposure);
	vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));

	color = current * whiteScale;
}
