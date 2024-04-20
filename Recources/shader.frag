#version 330 core
//in vec3 color;

out vec4 FragColor;
layout(location = 0) out vec3 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform sampler2D diffuse0;
uniform sampler2D specular0;


struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  
uniform bool enableSpecular;
uniform Material material;
uniform bool lighting;

float near = 0.5; 
float far  = 10.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
uniform vec3 fogColor;
//vec3 fogColor = vec3(.631f, 0.553f, 0.66f);

void main()
{
// vec2 correctedTexCoord = TexCoords + vec2(FragPos.x * Depth, FragPos.y * Depth);
   float depth = LinearizeDepth(gl_FragCoord.z) / far;
   
   if(lighting){
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
     vec3 ambient  = light.ambient * vec3(texture(diffuse0, TexCoords));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(diffuse0, TexCoords));

    if(enableSpecular){
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * lightColor;
    vec3 result = ambient + diffuse + specular * lightColor;
    vec3 finalColor = mix(result.rgb, fogColor, depth);
    FragColor = vec4(finalColor, 1.0);
    }
    else {

     vec3 specular = vec3(0.0, 0.0, 0.0);
     vec3 result = ambient + diffuse * lightColor;
     vec3 finalColor = mix(result.rgb, fogColor, depth);
     FragColor = vec4(finalColor, 1.0);

       }
    }
    else{
      vec4 materialColor = texture(diffuse0, TexCoords);
      vec3 finalColor = mix(materialColor.rgb, fogColor, depth);
      FragColor = vec4(finalColor, 1.0);
    }
   
   // vec3 finalColor = mix(FragColor.rgb, fogColor, depth);
    //FragColor = vec4(finalColor, 1.0);
};