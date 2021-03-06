﻿
#include "gl_traits.hpp"
#include "helpers.h"


template <class TexTarSeq>
struct get_tar_indx;

template <glt::TextureTarget ... tar>
struct get_tar_indx<glt::values_list<glt::TextureTarget, tar...>>
{
    constexpr static std::array<glt::TextureTarget, sizeof...(tar)> targets{ tar... };
};

int main()
{

    constexpr auto& targets = get_tar_indx<glt::TextureTargetList>::targets;

    SmartGLFW glfw{ 3, 3 };
    SmartGLFWwindow window{ SCR_WIDTH, SCR_HEIGHT, "Testing textures" };

    glfw.MakeContextCurrent(window);
    glt::LoadOpenGL(glfw.GetOpenGLLoader());

    glt::Texture2D<glt::TexInternFormat::rgba> tex2D;

    tex2D.Bind();
	tex2D.SetImage(0, 2048, 1024);

	glt::Texture1D<glt::TexInternFormat::rgba> tex1D;
	tex1D.Bind();
	tex1D.SetImage(0, 2048);

	GLenum error = glGetError();


    //tex2D.

   // glt::Texture2D<glt::TexInternFormat::rgba>::SetStorage

    return 0;
}
