// GLSL version
#version 330 core
// vertex input
layout (location = 0)   in  vec3    in_Vertex;
layout (location = 2)   in  vec2    in_TexCoord0;
layout (location = 3)   in  vec3    in_Normal;
layout (location = 4)   in  ivec4   in_Bone_ID;
layout (location = 5)   in  vec4    in_BoneW;

const   int MAX_BONES   =   100;
// uniform
uniform mat4    MVP;
uniform mat4    Bones[MAX_BONES];
uniform int     skinned;
uniform mat4    projection;
uniform mat4    modelview;
// output
out VertexAttrib
{
    vec2    CoordTexture;
    vec3    WorldPos0;
    vec3    Normal0;
}vertex;
void    main()
{
    if (skinned == 1)
    {
        mat4    BoneTransform=  Bones[in_Bone_ID[0]] * in_BoneW[0];
        BoneTransform       +=  Bones[in_Bone_ID[1]] * in_BoneW[1];
        BoneTransform       +=  Bones[in_Bone_ID[2]] * in_BoneW[2];
        BoneTransform       +=  Bones[in_Bone_ID[3]] * in_BoneW[3];

        vec4    positionL   =   BoneTransform * vec4(in_Vertex, 1.0);
        //  Final position
        gl_Position =   MVP * positionL;
    }
    else
    {
    // Final position
    gl_Position =   MVP * vec4(in_Vertex,1.0);
    }
    // output
    vertex.CoordTexture =   in_TexCoord0;
    vertex.Normal0      =   (modelview * vec4(in_Normal,0.0)).xyz;
    vertex.WorldPos0    =   (modelview * vec4(in_Vertex,1.0)).xyz;
}

