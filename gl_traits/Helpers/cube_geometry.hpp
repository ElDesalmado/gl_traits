#pragma once

#include <vector>

// compound user-defined type
struct vertex
{
    glm::vec3 posCoords;
    glm::vec2 textureCoords;

    bool operator==(const vertex& other) const
    {
        return posCoords == other.posCoords &&
            textureCoords == other.textureCoords;
    }

    bool operator!=(const vertex& other) const
    {
        return !operator==(other);
    }

};

struct vertexR
{
    glm::vec2 textureCoords;
    glm::vec3 posCoords;

    bool operator==(const vertex& other) const
    {
        return posCoords == other.posCoords &&
            textureCoords == other.textureCoords;
    }

    bool operator!=(const vertex& other) const
    {
        return !operator==(other);
    }

};

struct posVec3
{
    glm::vec3 coord;

    bool operator==(const posVec3& other) const
    {
        return coord == other.coord;
    }

    bool operator!=(const posVec3& other) const
    {
        return !(*this == other);
    }
};

struct posXYZf
{
    float x,
        y,
        z;

    bool operator==(const posXYZf& other) const
    {
        return std::tie(x, y, z) ==
            std::tie(other.x, other.y, other.z);
    }

    bool operator!=(const posXYZf& other) const
    {
        return !(*this == other);
    }
};


std::vector<glm::vec3> glm_cube_positions()
{
    return std::vector<glm::vec3>{
        glm::vec3{ -0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f,  0.5f, -0.5f },
            glm::vec3{ 0.5f,  0.5f, -0.5f },
            glm::vec3{ -0.5f,  0.5f, -0.5f },
            glm::vec3{ -0.5f, -0.5f, -0.5f },

            glm::vec3{ -0.5f, -0.5f,  0.5f },
            glm::vec3{ 0.5f, -0.5f,  0.5f },
            glm::vec3{ 0.5f,  0.5f,  0.5f },
            glm::vec3{ 0.5f,  0.5f,  0.5f },
            glm::vec3{ -0.5f,  0.5f,  0.5f },
            glm::vec3{ -0.5f, -0.5f,  0.5f },

            glm::vec3{ -0.5f,  0.5f,  0.5f },
            glm::vec3{ -0.5f,  0.5f, -0.5f },
            glm::vec3{ -0.5f, -0.5f, -0.5f },
            glm::vec3{ -0.5f, -0.5f, -0.5f },
            glm::vec3{ -0.5f, -0.5f,  0.5f },
            glm::vec3{ -0.5f,  0.5f,  0.5f },

            glm::vec3{ 0.5f,  0.5f,  0.5f },
            glm::vec3{ 0.5f,  0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f,  0.5f },
            glm::vec3{ 0.5f,  0.5f,  0.5f },

            glm::vec3{ -0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f, -0.5f },
            glm::vec3{ 0.5f, -0.5f,  0.5f },
            glm::vec3{ 0.5f, -0.5f,  0.5f },
            glm::vec3{ -0.5f, -0.5f,  0.5f },
            glm::vec3{ -0.5f, -0.5f, -0.5f },

            glm::vec3{ -0.5f,  0.5f, -0.5f },
            glm::vec3{ 0.5f,  0.5f, -0.5f },
            glm::vec3{ 0.5f,  0.5f,  0.5f },
            glm::vec3{ 0.5f,  0.5f,  0.5f },
            glm::vec3{ -0.5f,  0.5f,  0.5f },
            glm::vec3{ -0.5f,  0.5f, -0.5f }
    };
}

std::vector<posVec3> posVec3_cube_positions()
{
    std::vector<glm::vec3> positions = glm_cube_positions();
    std::vector<posVec3> out{ positions.size() };
    for (size_t i = 0; i != positions.size(); ++i)
        out[i].coord = positions[i];

    return out;
}

std::vector<posXYZf> posXYZf_cube_positions()
{
    std::vector<glm::vec3> positions = glm_cube_positions();
    std::vector<posXYZf> out{ positions.size() };
    for (size_t i = 0; i != positions.size(); ++i)
    {
        out[i].x = positions[i].x;
        out[i].y = positions[i].y;
        out[i].z = positions[i].z;
    }

    return out;
}

std::vector<glm::vec2> glm_cube_texCoords()
{
    return std::vector<glm::vec2>{

        glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 0.0f },

            glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 0.0f },

            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },

            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },

            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 0.0f, 1.0f },

            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 1.0f, 1.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ 0.0f, 0.0f },
            glm::vec2{ 0.0f, 1.0f }
    };
}

std::vector<vertex> cube_vertices()
{
    auto positions = glm_cube_positions();
    auto texCoords = glm_cube_texCoords();

    std::vector<vertex> out;

    for (size_t i = 0; i != positions.size(); ++i)
        out.push_back(vertex{ positions[i], texCoords[i] });
    return out;
}