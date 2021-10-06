2D {

    (Set 0, Binding 0, Frag) Texture "imageSampler"

    PushConstants (Vert) {
        mat4 model
        mat4 camera
        vec4 color
    }

}

3D {

    (Set 0, Binding 0, Vert) Buffer "Camera" {
        mat4 view
        mat4 proj
        vec3 position
    }

    (Set 1) {
        (Binding 0, Vert) Buffer "DirLightMat" {
            mat4 dirLightMat
        }

        (Binding 1, Frag) Buffer "Lights" {
            struct DirLight {
                vec3 direction
                vec4 ambientColor
                float intensity
            }

            array[MAX_POINT_LIGHTS] struct PointLight {
                vec3 position
                vec4 color
                vec4 infos
            }
        }

        (Binding 2, Frag) Texture "shadowsTexture"
    }

    (Set 2) {
        (Binding 0, Frag) Texture "Albedo"
        (Binding 1, Frag) Texture "Specular"
    }

    (Set 3, Binding 0, Vert) DynamicBuffer "Bones"

    PushConstants (Vert) {
        mat4 model
    }

}

Shadows3D {

    (Set 0, Binding 0, Vert) Buffer "Camera" {
        mat4 view
        mat4 proj
        vec3 position
    }

    (Set 1, Binding 0, Vert) Buffer "DirLightMat" {
            mat4 dirLightMat
    }


    (Set 2, Binding 0, Vert) DynamicBuffer "Bones"

    PushConstants (Vert) {
        mat4 model
    }

}

PostProcess {

    (Set 0, Binding 0, Frag) Texture "texture"

    PushConstants (Frag) Buffer "Settings"  {
        int useFxaa
        int screenSizeX
        int screenSizeY
        float renderRes
    }

}   

Skybox {

    (Set 0, Binding 0, Vert) Buffer "Camera" {
        mat4 view
        mat4 proj
        vec3 position
    }

    (Set 1, Binding 0, Frag) Cubemap "skybox"

    PushConstant (Vert) {
        mat4 model
    }

}
