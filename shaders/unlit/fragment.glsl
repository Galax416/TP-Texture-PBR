#version 330 core

in vec3 o_positionWorld;
in vec3 o_normalWorld;
in vec2 o_uv0;
in mat3 TBN;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D colorTexture;
uniform sampler2D normalTexture;

// ---Phong----
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

float ambientStrength = 0.7;
float specularStrength = 0.7;
float shininess = 32.0;
// ------------


vec3 ligth_position = vec3(0.3,0.5,0.2); // test phong


// -------Réflexion---------
uniform samplerCube skybox;
// -------------------------


// -------PBR-------
vec3  albedo;
float metallic = 0.9;
float roughness = 0.1;
float ao = 10;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main() {

    // -----------------------Phong--------------------------------------
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = texture(normalTexture, o_uv0).rgb;
    norm = norm * 2.0 - 1.0;   
    norm = normalize(TBN * norm); 

    vec3 lightDir = TBN * normalize(ligth_position - o_positionWorld);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = TBN * normalize(viewPos - o_positionWorld);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular);
    vec4 objectColorTex = texture(colorTexture, o_uv0) * color;
    // FragColor = vec4(result * objectColorTex.rgb, 1.0); // Phong
    // -------------------------------------------------------------------


    // ---------Réflexion-----------
    vec3 I = normalize(o_positionWorld - viewPos);
    vec3 R = reflect(I, normalize(o_normalWorld));
    // FragColor = vec4(texture(skybox, R).rgb, 1.0); // Éffet Miroir
    // -----------------------------
   
    
    // -----------PBR----------------
    albedo = texture(colorTexture, o_uv0).rgb;



    vec3 N = norm;
    vec3 V = normalize(viewPos - o_positionWorld);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    vec3 L = normalize(ligth_position - o_positionWorld);
    vec3 H = normalize(V + L);
    float distance    = length(ligth_position - o_positionWorld);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = lightColor * attenuation;     

    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specularPBR     = numerator / denominator;  
    
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specularPBR) * radiance * NdotL; 

    vec3 ambientPBR = vec3(0.03) * albedo * ao;
    vec3 color = ambientPBR + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    // FragColor = vec4(color, 1.0); // PBR
    // ------------------------------------
    
    // Choisir le type de rendu
    //FragColor = vec4(result * objectColorTex.rgb, 1.0); // Phong
    // FragColor = vec4(texture(skybox, R).rgb, 1.0); // Éffet Miroir
    FragColor = vec4(color, 1.0); // PBR



    //FragColor = color;
    // FragColor = texture(colorTexture, o_uv0) * color;
    // DEBUG: position
    // FragColor = vec4(o_positionWorld, 1.0);
    // DEBUG: normal
    // FragColor = vec4(0.5 * o_normalWorld + vec3(0.5) , 1.0);
    // DEBUG: uv0
    // FragColor = vec4(o_uv0, 1.0, 1.0);
}
