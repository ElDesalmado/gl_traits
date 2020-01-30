#include "gl_traits.hpp"

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

// 

//const Handle<VAOTarget::vao> *glt_buffers::currentVAO_ = nullptr;

/*

std::array<std::pair<TextureTarget, const void*>, gltActiveTexture::max_gl_textures>
	gltActiveTexture::textureUnits_{};
size_t gltActiveTexture::currentUnit_ = 0;




//templates instantiations

void gl_debug::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
*/