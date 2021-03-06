#include "gl_traits.hpp"

#include <iostream>

using namespace glt;

//template class MapStatus_<BufferTargetList>;

template <class>
struct init_map;

template <BufferTarget ... targets>
struct init_map<std::integer_sequence<BufferTarget, targets...>>
{
    operator std::map<BufferTarget, buffer_base*>() const
    {
        return { {targets, nullptr}... };
    }
};

std::map<BufferTarget, buffer_base*> buffer_base::targets_{ init_map<BufferTargetList>() };

vao_base* vao_base::active_vao_ = nullptr;
program_base *program_base::active_prog_ = nullptr;

std::array<texture_base*, TextureTargetList::size>
    texture_base::active_textures_{ nullptr };

std::map<FrameBufTarget, framebuffer_base*> framebuffer_base::bound_{
	{FrameBufTarget::draw, nullptr},
{FrameBufTarget::framebuffer, nullptr},
{FrameBufTarget::read, nullptr}
};

renderbuffer_base * renderbuffer_base::bound_ = nullptr;

bool glt::AssertGL()
{
    GLenum error = glGetError();

    if (!error)
        return true;

    std::cerr << error << std::endl;
    return false;
}

void glt::LoadOpenGL(void(*loader)(void))
{
    bool loaded = gladLoadGLLoader((GLADloadproc)loader);
    assert(loaded && "failed to load GLAD");
    if (!loaded)
        throw std::exception{ "Failed to initialize GLAD" };
}
