#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform bool hdr;
uniform float exposure;
uniform bool bloom;
uniform bool grayscale;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    vec3 result;

    if(bloom){
        hdrColor += bloomColor; // additive blending
    }
    if(hdr){
        // reinhard
        result = vec3(1.0) - exp(-hdrColor * exposure);
        //gamma correct
        result = pow(result, vec3(1.0 / gamma));
    }
    else
    {
        result = pow(hdrColor, vec3(1.0 / gamma));
    }

    if(grayscale){
        float gray = 0.2126 * result.r + 0.7152 * result.g + 0.0722 * result.b;
        result = vec3(gray);
    }

    FragColor = vec4(result, 1.0);
}