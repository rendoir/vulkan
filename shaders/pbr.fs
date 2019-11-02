#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV0;
layout (location = 3) in vec2 inUV1;

// Scene bindings
layout (set = 0, binding = 0) uniform UBO {
	mat4 projection;
	mat4 model;
	mat4 view;
	vec3 camPos;
} ubo;

// IBL textures
layout (set = 0, binding = 1) uniform samplerCube irradianceMap;
layout (set = 0, binding = 2) uniform samplerCube prefilterMap;
layout (set = 0, binding = 3) uniform sampler2D brdflutMap;

// Material bindings
layout (set = 1, binding = 0) uniform sampler2D colorMap;
layout (set = 1, binding = 1) uniform sampler2D physicalDescriptorMap;
layout (set = 1, binding = 2) uniform sampler2D normalMap;
layout (set = 1, binding = 3) uniform sampler2D aoMap;
layout (set = 1, binding = 4) uniform sampler2D emissiveMap;

layout (push_constant) uniform Material {
	vec4 baseColorFactor;
	vec4 emissiveFactor;
	int colorTextureSet;
	int physicalTextureSet;
	int normalTextureSet;	
	int occlusionTextureSet;
	int emissiveTextureSet;
	float metallicFactor;	
	float roughnessFactor;	
	float alphaMask;
	float alphaMaskCutoff;
} material;

layout (location = 0) out vec4 outColor;

// Constants
const float PI = 3.1415926535897932384626433832795;
const float gamma = 2.2;
const float exposure = 4.5;
const vec3 dielectricF0 = vec3(0.04);
const float iblStrength = 1.0;
const float epsilon = 1.175495e-38;

// Lights - TODO make uniform
const int numberLights = 1;
vec3 lightColors[numberLights] = vec3[](
    vec3(2.0, 2.0, 2.0)
);
vec3 lightPositions[numberLights] = vec3[](
    vec3(1.0, 1.0, 1.0)
);


// Normal Mapping without Precomputed Tangents by Christian Schüler
vec3 getNormalFromMap() {
	vec2 inUV = material.normalTextureSet == 0 ? inUV0 : inUV1;
	vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

	vec3 q1 = dFdx(inWorldPos);
	vec3 q2 = dFdy(inWorldPos);
	vec2 st1 = dFdx(inUV);
	vec2 st2 = dFdy(inUV);

	vec3 N = normalize(inNormal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

vec3 getNormal() {
	return (material.normalTextureSet > -1) ? getNormalFromMap() : normalize(inNormal);
}

vec4 sRGBtoLinear(vec4 srgbIn) {
	return vec4(pow(srgbIn.rgb, vec3(gamma)), srgbIn.a);
}

vec3 sRGBtoLinear(vec3 srgbIn) {
	return pow(srgbIn, vec3(gamma));
}

vec4 linearTosRGB(vec4 linearIn) {
	return vec4(pow(linearIn.rgb, vec3(1.0 / gamma)), linearIn.a);
}

vec3 linearTosRGB(vec3 linearIn) {
	return pow(linearIn, vec3(1.0 / gamma));
}


// Basic Ambient Occlusion
void applyAmbientOcclusion(inout vec3 color) {
	const float occlusionStrength = 1.0f; // TODO - Should be a parameter
	if (material.occlusionTextureSet > -1) {
		float ao = texture(aoMap, (material.occlusionTextureSet == 0 ? inUV0 : inUV1)).r;
		color = mix(color, color * ao, occlusionStrength);
	}
}

void applyEmissiveness(inout vec3 color) {
	const float emissiveFactor = 1.0f; // TODO - Should be a parameter
	if (material.emissiveTextureSet > -1) {
		vec3 emissive = sRGBtoLinear(texture(emissiveMap, material.emissiveTextureSet == 0 ? inUV0 : inUV1)).rgb * emissiveFactor;
		color += emissive;
	}
}

void applyGammaCorrection(inout vec3 color) {
	linearTosRGB(color);
}


// Uncharted2 Tonemapping
vec3 Uncharted2Tonemap(vec3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;

	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void applyHDRTonemapping(inout vec3 color) {
	float W = 11.2;

	vec3 current = Uncharted2Tonemap(color * exposure);
	vec3 whiteScale = 1.0f / Uncharted2Tonemap(vec3(W));

	color = current * whiteScale;
}


void getAlbedo(out vec4 albedo) {
	if (material.colorTextureSet > -1) {
		albedo = sRGBtoLinear(texture(colorMap, material.colorTextureSet == 0 ? inUV0 : inUV1)) * material.baseColorFactor;
	} else {
		albedo = material.baseColorFactor;
	}

	if (albedo.a < material.alphaMaskCutoff) {
		discard;
	}
}

void getMetallicRoughness(out float metallic, out float roughness) {
	metallic = material.metallicFactor;
	roughness = material.roughnessFactor;

	if (material.physicalTextureSet > -1) {
		vec4 mrSample = texture(physicalDescriptorMap, material.physicalTextureSet == 0 ? inUV0 : inUV1);
		roughness = mrSample.g * roughness;
		metallic = mrSample.b * metallic;
	}
}


// Trowbridge-Reitz GGX Normal Distribution Function
float NormalDistributionFunction(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float aSquare = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotHSquare = NdotH * NdotH;

    float numerator = aSquare;
    float denominator = (NdotHSquare * (aSquare - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / max(denominator, epsilon); // Prevent division by zero.
}


// Shlick Fresnel Function 
vec3 FresnelFunction(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Shlick Fresnel Function with Roughness
vec3 FresnelRoughnessFunction(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


// Schlick Smith GGX Geometry Function 
float GeometryFunction(vec3 N, vec3 V, vec3 L, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float gl = NdotL / (NdotL * (1.0 - k) + k);
    float gv = NdotV / (NdotV * (1.0 - k) + k);

    return gl * gv;
}


// Cook-Torrance Bidirectional Reflective Distribution Function
vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 radiance, vec4 albedo, float metallic, float roughness, vec3 F0) {
	vec3 H = normalize (V + L);

	float D = NormalDistributionFunction(N, H, roughness);   
	float G = GeometryFunction(N, V, L, roughness);      
	vec3  F = FresnelFunction(max(dot(H, V), 0.0), F0);

	vec3 numerator = D * G * F; 
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0); 
	vec3 specular = numerator / max(denominator, epsilon); // Prevent division by zero.
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;	  

	float NdotL = max(dot(N, L), 0.0);        

	vec3 color = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
	return color;
}


vec3 IBL(vec4 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 F0) {
    vec3 F = FresnelRoughnessFunction(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;
    
	vec3 R = reflect(-V, N);
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * float(textureQueryLevels(prefilterMap)-1)).rgb;    
    vec2 brdf  = texture(brdflutMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    return iblStrength * (kD * diffuse + specular);
}


void main()
{
	vec4 albedo;
	float metallic;
	float roughness;

	getAlbedo(albedo);
	getMetallicRoughness(metallic, roughness);

	vec3 N = getNormal();
	vec3 V = normalize(ubo.camPos - inWorldPos);

	vec3 F0 = mix(dielectricF0, albedo.rgb, metallic);

	// Reflectance equation
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < numberLights; i++) {
		vec3 uL = lightPositions[i] - inWorldPos;
		vec3 L = normalize(uL);
		float lightDistance = length(uL);
        float attenuation = 1.0 / (lightDistance * lightDistance);
		vec3 radiance = sRGBtoLinear(lightColors[i]) * attenuation;
		
		Lo += BRDF(L, V, N, radiance, albedo, metallic, roughness, F0);
	}
	
	applyEmissiveness(Lo);

	vec3 ambient = IBL(albedo, metallic, roughness, N, V, F0);
	applyAmbientOcclusion(ambient);

	vec3 color = ambient + Lo;
	
	applyHDRTonemapping(color);
	applyGammaCorrection(color);
	
	outColor = vec4(color, albedo.a);
}
