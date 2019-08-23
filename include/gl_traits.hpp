#pragma once

#ifndef _gl_traits_
#define _gl_traits_
#endif // !_gl_traits_


#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_XYZW_ONLY 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#define POD_EXTENDS std::tuple<glm::vec1, glm::vec2, glm::vec3, glm::vec4, \
                               glm::uvec1, glm::uvec2, glm::uvec3, glm::uvec4 \
>
#include "pod_reflection.hpp"
#include "dhconstexpr_lib.hpp"


#include <thread>
#include <functional>

//debug
#include <iostream>

#include <vector>
#include <array>

#include "glad/glad.h"

#include "traits_common.hpp"

#include "uniform_traits.hpp"
#include "buffer_traits.hpp"
#include "texture_traits.hpp"


template <auto target, class = typename decltype(target)>
class gltHandle;

template <auto target>
class gltHandle<target, typename decltype(target)>
{
    //TODO: add Glsync handle type
    GLuint handle_;

public:

    using Target = typename decltype(target);

    gltHandle(GLuint handle)
        : handle_(handle)
    {
        //ownership?
        //handle = 0;
    }

    gltHandle(const gltHandle<target>& other) = delete;
    gltHandle<target>& operator=(const gltHandle<target>& other) = delete;

    gltHandle(gltHandle<target>&& other)
        : handle_(other.handle_)
    {
        other.handle_ = 0;
    }
    gltHandle<target>& operator=(gltHandle<target>&& other)
    {
        if (handle_)
            DestroyHandle();
        handle_ = other.handle_;
        other.handle_ = 0;
    }

    bool operator==(const gltHandle<target>& other) const
    {
        return handle_ == other.handle_;
    }

    bool operator!=(const gltHandle<target>& other) const
    {
        return !operator==(other);
    }

    bool IsValid() const
    {
        return handle_;
    }

    bool operator!() const
    {
        return !IsValid();
    }

    constexpr operator GLuint() const
    {
        return handle_;
    }

    ~gltHandle()
    {
        if (handle_)
            DestroyHandle();
    }

private:
    void DestroyHandle();
};





class buffer_traits
{
public:
    enum Target {
        array_buffer = GL_ARRAY_BUFFER,
        atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
        copy_read_buffer = GL_COPY_READ_BUFFER,
        copy_write_buffer = GL_COPY_WRITE_BUFFER,
        dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
        draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
        element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
        pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
        pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
        query_buffer = GL_QUERY_BUFFER,
        shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
        texture_buffer = GL_TEXTURE_BUFFER,
        transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
        uniform_buffer = GL_UNIFORM_BUFFER
    };

    enum Usage {
        stream_draw = GL_STREAM_DRAW,
        stream_read = GL_STREAM_READ,
        stream_copy = GL_STREAM_COPY,
        static_draw = GL_STATIC_DRAW,
        static_read = GL_STATIC_READ,
        static_copy = GL_STATIC_COPY,
        dynamic_draw = GL_DYNAMIC_DRAW,
        dynamic_read = GL_DYNAMIC_READ,
        dynamic_copy = GL_DYNAMIC_COPY
    };
private:
    template <Target target>
    static const gltHandle<target> *currentBuffer_ ;

public:

    template <Target target>
    static gltHandle<target> GenBuffer()
    {
        GLuint handle = 0;
        glGenBuffers(1, &handle);
        assert(handle && "Failed to generate buffer!");
        return gltHandle<target>(handle);
    }

    template <Target target>
    static bool IsCurrentHandle(const gltHandle<target>& handle)
    {
        return currentBuffer_<target> ? *currentBuffer_<target> == handle : false;
    }

    template <Target target>
    static void BindBuffer(const gltHandle<target>& handle)
    {
        glBindBuffer(target, handle);
        currentBuffer_<target> = &handle;
    }

    //TODO: restrict usage for array_buffer
    template<size_t sz, typename dataType>
    static void BufferData(Target target, const dataType(&data)[sz], Usage usage)
    {
        glBufferData(target, sizeof(data), data, usage);
    }

    template <class dataType>
    static void BufferData(Target target, const std::vector<dataType>& data, Usage usage)
    {
        glBufferData(target, data.size() * sizeof(dataType), data.data(), usage);
    }

};

template <buffer_traits::Target target>
const gltHandle<target> *buffer_traits::currentBuffer_ = nullptr;

struct frameBuffer_traits;          //-
struct programm_traits;             //-
struct programPipelines_traits;     //-
struct queries_traits;              //-
struct renderBuffers_traits;        //-
struct samplers_traits;             //-
struct shader_traits;               //-
struct sync_traits;                 //-

struct primitive_traits
{
    enum Mode {
        points = GL_POINTS,
        line_strip = GL_LINE_STRIP,
        line_loop = GL_LINE_LOOP,
        lines = GL_LINES,
        line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
        lines_adjacency = GL_LINES_ADJACENCY,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan = GL_TRIANGLE_FAN,
        triangles = GL_TRIANGLES,
        triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        triangles_adjacency = GL_TRIANGLES_ADJACENCY,
        patches = GL_PATCHES
    };

    //there's no errors mentioned for negative first, but it's very unlikely to accept negatives anyway
    template <Mode mode>
    static void DrawArrays(size_t first, size_t count)
    {
        glDrawArrays(mode, (GLint)first, (GLsizei)count);
    }

};

//template <size_t textr_count>
class texture_traits
{

#ifndef MAX_GL_TEXTURES
    constexpr static size_t max_gl_textures = 80;
#else
    constexpr static size_t max_gl_textures = MAX_GL_TEXTURES;
#endif

public:
    enum Target {

        //for glBindTexture
        texture_1d = GL_TEXTURE_1D,							//+
        texture_2d = GL_TEXTURE_2D,							//+
        texture_3d = GL_TEXTURE_3D,							//+
        texture_1d_array = GL_TEXTURE_1D_ARRAY,				//+
        texture_2d_array = GL_TEXTURE_2D_ARRAY,				//+
        texture_rectangle = GL_TEXTURE_RECTANGLE,			//+	
        texture_cube_map = GL_TEXTURE_CUBE_MAP,				//?
        texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,	//??
        texture_buffer = GL_TEXTURE_BUFFER,					//??
        texture_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,				//+
        texture_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,	//+

        //for glImageTexture and glTexStorage
        proxy_texture_1d =              GL_PROXY_TEXTURE_1D,  //glTexImage1D

        proxy_texture_2d =              GL_PROXY_TEXTURE_2D, //glTexImage2D
		proxy_texture_1d_array =		GL_PROXY_TEXTURE_1D_ARRAY,
        proxy_texture_rectangle =       GL_PROXY_TEXTURE_RECTANGLE,
        texture_cube_map_positive_x =   GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        texture_cube_map_negative_x =   GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        texture_cube_map_positive_y =   GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        texture_cube_map_negative_y =   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        texture_cube_map_positive_z =   GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        texture_cube_map_negative_z =   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		proxy_texture_cube_map =		GL_PROXY_TEXTURE_CUBE_MAP,

		proxy_texture_2d_multisample =	GL_PROXY_TEXTURE_2D_MULTISAMPLE,	//glTexImage2DMultisample

		proxy_texture_3d =				GL_PROXY_TEXTURE_3D,
		proxy_texture_2d_array =		GL_PROXY_TEXTURE_2D_ARRAY,

		proxy_texture_2d_multisample_array = GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
    };

private:
	using gl_allowed_targets_1d = std::integer_sequence<Target,
		texture_1d, 
		proxy_texture_1d>;

	using gl_allowed_targets_2d = std::integer_sequence<Target,
		texture_2d,
		proxy_texture_2d,
		texture_1d_array,
		proxy_texture_1d_array,
		texture_rectangle,
		proxy_texture_rectangle,
		texture_cube_map_positive_x,
		texture_cube_map_negative_x,
		texture_cube_map_positive_y,
		texture_cube_map_negative_y,
		texture_cube_map_positive_z,
		texture_cube_map_negative_z,
		proxy_texture_cube_map>;

	using gl_allowed_targets_2d_multisample = std::integer_sequence<Target,
		texture_2d_multisample,
		proxy_texture_2d_multisample>;

	using gl_allowed_targets_3d = std::integer_sequence<Target,
		texture_3d,
		proxy_texture_3d,
		texture_2d_array,
		proxy_texture_2d_array>;

	using gl_allowed_targets_3d_multisample = std::integer_sequence<Target,
		texture_2d_multisample_array,
		proxy_texture_2d_multisample_array
	>;


	//TODO: write unit tester
	using map_allowed_targets = cexpr_generic_map<
		//1d
		cexpr_pair<auto_t<texture_1d>, gl_allowed_targets_1d>,

		//2d
		cexpr_pair<auto_t<texture_2d>, gl_allowed_targets_2d>,
		cexpr_pair<auto_t<texture_1d_array>, gl_allowed_targets_2d>,
		cexpr_pair<auto_t<texture_rectangle>, gl_allowed_targets_2d>,

		//2d_multisample
		cexpr_pair<auto_t<texture_2d_multisample>, gl_allowed_targets_2d_multisample>,


		//3d
		cexpr_pair<auto_t<texture_3d>, gl_allowed_targets_3d>,
		cexpr_pair<auto_t<texture_2d_array>, gl_allowed_targets_3d>,

		//3d_multisample
		cexpr_pair<auto_t<texture_2d_multisample_array>, gl_allowed_targets_3d_multisample>
	>;

	template <Target target>
	using allowed_targets_ = typename map_allowed_targets::found_pair<auto_t<target>>::value;

	template<Target target, Target texStorage, class allowed = typename allowed_targets_<target>>
	struct target_validate;

public:
	template<Target target, Target texStorage, Target ... allowed>
	struct target_validate<target, texStorage, std::integer_sequence<Target, allowed...>>
	{
		constexpr static bool value = is_any<texStorage, allowed...>;
	};

    enum PName {
        depth_stencil_texture_mode = GL_DEPTH_STENCIL_TEXTURE_MODE,
        texture_base_level =    GL_TEXTURE_BASE_LEVEL,
        texture_compare_func =  GL_TEXTURE_COMPARE_FUNC,
        texture_compare_mode =  GL_TEXTURE_COMPARE_MODE,
        texture_lod_bias = GL_TEXTURE_LOD_BIAS,
        texture_min_filter = GL_TEXTURE_MIN_FILTER,
        texture_mag_filter = GL_TEXTURE_MAG_FILTER,
        texture_min_lod = GL_TEXTURE_MIN_LOD,
        texture_max_lod = GL_TEXTURE_MAX_LOD,
        texture_max_level = GL_TEXTURE_MAX_LEVEL,
        texture_swizzle_r = GL_TEXTURE_SWIZZLE_R,
        texture_swizzle_g = GL_TEXTURE_SWIZZLE_G,
        texture_swizzle_b = GL_TEXTURE_SWIZZLE_B,
        texture_swizzle_a = GL_TEXTURE_SWIZZLE_A,
        texture_wrap_s = GL_TEXTURE_WRAP_S,
        texture_wrap_t = GL_TEXTURE_WRAP_T,
        texture_wrap_r = GL_TEXTURE_WRAP_R,
        texture_border_color = GL_TEXTURE_BORDER_COLOR,
        texture_swizzle_rgba = GL_TEXTURE_SWIZZLE_RGBA
    };


    //////////////////////////////////////////////
    //predefined values
    //maybe use structs with auto tags instead? Some 

    //////////////////////////////////////////////
    struct gl_depth_component : public gl_value<GL_DEPTH_COMPONENT>{};
    struct gl_stencil_components : public gl_value<GL_STENCIL_COMPONENTS> {};

    struct gl_compare_ref_to_texture : public gl_value<GL_COMPARE_REF_TO_TEXTURE> {};

    struct gl_nearest : public gl_value<GL_NEAREST> {};
    struct gl_linear : public gl_value<GL_LINEAR> {};

    struct gl_nearest_mipmap_nearest : public gl_value<GL_NEAREST_MIPMAP_NEAREST> {};
    struct gl_linear_mipmamp_nearest : public gl_value<GL_LINEAR_MIPMAP_NEAREST> {};
    struct gl_nearest_mipmap_linear : public gl_value<GL_NEAREST_MIPMAP_LINEAR> {};
    struct gl_linear_mipmap_linear : public gl_value<GL_LINEAR_MIPMAP_LINEAR> {};

    struct gl_clamp_to_edge : public gl_value<GL_CLAMP_TO_EDGE> {};
    struct gl_clamp_to_border : public gl_value<GL_CLAMP_TO_BORDER> {};
    struct gl_mirrored_repeat : public gl_value<GL_MIRRORED_REPEAT> {};
    struct gl_repeat : public gl_value<GL_REPEAT> {};
    struct gl_mirror_clamp_to_edge : public gl_value<GL_MIRROR_CLAMP_TO_EDGE> {};


    /*
    enum v_depth_stencil_texture_mode {
        depth_component = GL_DEPTH_COMPONENT,
        stencil_component = GL_STENCIL_COMPONENTS //THERE"S NO "S" IN THE KHRONOS REFERENCE!
    };

    enum v_texture_compare_func {
        lequal = GL_LEQUAL,
        gequal = GL_GEQUAL,
        less = GL_LESS,
        greater = GL_GREATER,
        equal = GL_EQUAL,
        notequal = GL_NOTEQUAL,
        always = GL_ALWAYS,
        never = GL_NEVER
    };

    enum v_texture_compare_mode {
        compare_ref_to_texture = GL_COMPARE_REF_TO_TEXTURE,
        none = GL_NONE
    };

    enum v_texture_min_filter {
        nearest = GL_NEAREST,
        linear = GL_LINEAR,
        nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
        linear_mipmamp_nearest = GL_LINEAR_MIPMAP_NEAREST,
        nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
        linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR
    };*/

private:


    template <Target target>
    static const gltHandle<target> * currentTexture_;

	/*
	template<Target target>
	static std::array<const gltHandle<target>*, max_gl_textures> currentTextures_;
	*/

	static std::array<std::pair<Target, const void*>, max_gl_textures> textureUnits_;
	
	static size_t currentUnit_;

    typedef std::tuple<gl_depth_component,
        gl_stencil_components> v_depth_stencil_texture_mode;

    typedef std::tuple<gl_lequal,
        gl_gequal,
        gl_less,
        gl_greater,
        gl_equal,
        gl_notequal,
        gl_always,
        gl_never
    > v_texture_compare_func;

    typedef std::tuple<
        gl_compare_ref_to_texture,
        gl_none
    > v_texture_compare_mode;

    typedef std::tuple<
        gl_linear,
        gl_nearest,
        gl_nearest_mipmap_nearest,
        gl_nearest_mipmap_linear,
        gl_linear_mipmamp_nearest,
        gl_linear_mipmap_linear
    > v_texture_min_filter;

    typedef std::tuple<
        gl_linear,
        gl_nearest
    > v_texture_mag_filter;

    typedef std::tuple<
        gl_clamp_to_edge,
        gl_clamp_to_border,
        gl_mirrored_repeat,
        gl_repeat,
        gl_mirror_clamp_to_edge
    > v_wrap;

    typedef cexpr_generic_map<
        cexpr_pair<auto_t<depth_stencil_texture_mode>, v_depth_stencil_texture_mode>,
        cexpr_pair<auto_t<texture_compare_func>, v_texture_compare_func>,
        cexpr_pair<auto_t<texture_compare_mode>, v_texture_compare_mode>,
        cexpr_pair<auto_t<texture_min_filter>, v_texture_min_filter>,
        cexpr_pair<auto_t<texture_mag_filter>, v_texture_mag_filter>,
        cexpr_pair<auto_t<texture_wrap_s>, v_wrap>,
        cexpr_pair<auto_t<texture_wrap_r>, v_wrap>,
        cexpr_pair<auto_t<texture_wrap_t>, v_wrap>

    > pname_values;

    //for static use
    template <PName paramName, class alias, class gl_param_v>
    struct glTexParam_base
    {
        constexpr static void TexParameter(Target target, auto_t<paramName>&&, gl_param_v&& val) noexcept
        {
            glTexParameteri(target, paramName, val);
        }
    };

public:

    //template <Target target, class alias = decltype(target)>
    class glTexParam_class_base
    {
    protected:
        std::function<bool()> isCurrent_;

        glTexParam_class_base()
        {
            //std::cout << target << " default base class constructor" << std::endl;
            assert(false && "default constructor of virtual base class was called!");
        }

    public:
        glTexParam_class_base(std::function<bool()>&& isCurrent)
            : isCurrent_(std::move(isCurrent))
        {
            //std::cout << "glTexParam base class constructor" << std::endl;
        }
    };

private:
    //for per class object use
    template <Target target, PName paramName, class alias, class gl_param_v>
    class glTexParam_class : protected virtual glTexParam_class_base//<target>
    {

    public:

        //TODO: check if calling instance is currently bound!
        //Problem with using reference: class size skyrockets to 288 bites due to storing references...
        constexpr void TexParameter(auto_t<paramName>&&, gl_param_v&& val) noexcept
        {
            assert(isCurrent_() && "Setting texture parameters for not active texture");
            glTexParameteri(target, paramName, val);
        }
    };

    //intermediate helper
    template <class ... bases>
    struct glTexParam_impl : public bases...
    {
        using bases::TexParameter...;
    };

    //intermediate helper for class
    template <Target target, class ... bases>
    struct glTexParam_impl_class : public bases...
    {
        using bases::TexParameter...;
    };

    //static
    template <PName paramName, class allowed_vals = typename pname_values::found_pair<auto_t<paramName>>::value>
    struct glTexParamCollect;

    template <PName paramName, class ... allowed_vals>
    struct glTexParamCollect<paramName, std::tuple<allowed_vals...>> :
        public glTexParam_impl<glTexParam_base<paramName, decltype(paramName), allowed_vals>...> {};

    //per class object
    template <Target target, PName paramName, class allowed_vals = typename pname_values::found_pair<auto_t<paramName>>::value>
    struct glTexParamCollect_class;

    template <Target target, PName paramName, class ... allowed_vals>
    struct glTexParamCollect_class<target, paramName, std::tuple<allowed_vals...>> :
        public glTexParam_impl_class<target, glTexParam_class<target, paramName, decltype(paramName), allowed_vals>...>
    {};

    //static
    template <PName ... params>
    struct glTexParametersPredefined : public glTexParam_impl<glTexParamCollect<params> ...>
    {};

    //per class
    template <Target target, PName ... params>
    struct glTexParametersPredefined_class : public glTexParam_impl_class<target, glTexParamCollect_class<target, params> ...>
    {};

public:

    using DefinedTexParams = glTexParametersPredefined<depth_stencil_texture_mode,
        texture_compare_func,
        texture_compare_mode,
        texture_min_filter,
        texture_mag_filter,
        texture_wrap_s,
        texture_wrap_r,
        texture_wrap_t>;

    template <Target target>
    using DefinedTexParams_class = glTexParametersPredefined_class<target, depth_stencil_texture_mode,
        texture_compare_func,
        texture_compare_mode,
        texture_min_filter,
        texture_mag_filter,
        texture_wrap_s,
        texture_wrap_r,
        texture_wrap_t>;

    template <Target target>
    static gltHandle<target> GenTexture()
    {
        GLuint handle = 0;
        glGenTextures(1, &handle);
        assert(handle && "Failed to generate texture!");
        return gltHandle<target>(handle);
    }

    template <Target target, size_t ... indx>
    static std::array<gltHandle<target>, sizeof...(indx)> GenTextures(std::index_sequence<indx...>&&)
    {
        constexpr size_t sz = sizeof...(indx);
        GLuint handle[sz];
        glGenTextures(sz, handle);
        assert((handle[indx] && ...) && "Failed to generate texture");

        return std::array<gltHandle<target>, sz>{handle[indx] ...};
    }

	//ISSUE
	//TODO: is Texture Units array uniform for all targets???
	/*
	template <Target target>
	static gltHandle<target>* GetActiveTexture(size_t texUnit)
	{
		assert(texUnit < max_gl_textures && "Texture unit out of bounds!");
		return currentTextures_[texUnit];
	}*/

	static size_t ActiveUnit()
	{
		return currentUnit_;
	}

	//template <size_t texture_I>
	static void ActiveTextue(size_t texUnit) noexcept
	{
		assert(texUnit < max_gl_textures && "Texture unit out of bounds!");
		currentUnit_ = texUnit;
		glActiveTexture((GLenum)texUnit + GL_TEXTURE0);

		//static_assert(texture_I < max_gl_textures, "Texture id exceeded!");
		//constexpr int gl_texture_i = (int)texture_I + GL_TEXTURE0;
		//glActiveTexture(gl_texture_i);
	}


	//to currently active texture unit
    template <Target target>
    static void BindTexture(const gltHandle<target>& hTexture)
    {
        glBindTexture(target, hTexture);
        if (!hTexture)
        {
            currentTexture_<target> = nullptr;
            return;
        }
        currentTexture_<target> = &hTexture;
    }

	template <Target target>
	static void BindToUnit(const gltHandle<target>& hTexture, size_t texUnit)
	{
		ActiveTextue(texUnit);
		BindTexture(hTexture);
//		currentTextures_<target>[texUnit] = &hTexture;
		textureUnits_[texUnit] = std::make_pair(target, (const void*)&hTexture);
	}

	//temporary
	template <Target texStorage, Target target>
	static void TexImage2D(const gltHandle<target>&, size_t detailLevel,
		GLint internalFormat,
		size_t width,
		size_t height,
		GLint border /*= 0*/,
		GLenum format,
		GLenum type,
		const GLvoid * data)
	{
		static_assert(target_validate<target, texStorage>::value,
			"Invalid texture storage!");

		//assert that handle is active or current

		glTexImage2D(texStorage, detailLevel, internalFormat, width, height, border,
			format, type, data);
	}

	static size_t ActiveTexUnit()
	{
		return currentUnit_;
	}

	static std::pair<Target, const void*> CurrentTexUnit()
	{
		return textureUnits_[ActiveTexUnit()];
	}

	//ISSUE
	//TODO: check implementation!
    template <Target target>
    static bool IsCurrentTexture(const gltHandle<target>& hTexture)
    {
		std::pair<Target, const void*> curUnit = CurrentTexUnit();
		if (!curUnit.second)
			return false;

		if (curUnit.first != target)
		{
			assert(*(const gltHandle<target>*)curUnit.second != hTexture &&
				"IsCurrentTexture false output. Invalid target type returned!");
			return false;
		}
		return *(const gltHandle<target>*)curUnit.second == hTexture;

		/*
		return currentTextures_<target>[texUnit] ?
			*currentTextures_<target>[texUnit] == hTexture : false;
			*/

        //return currentTexture_<target> ? *currentTexture_<target> == hTexture : false;
    }


    template <Target target>
    static void GenerateMipMap() noexcept
    {
        glGenerateMipmap(target);
    }

};

template <texture_traits::Target target>
const gltHandle<target> * texture_traits::currentTexture_ = nullptr;

/*
template<texture_traits::Target target>
std::array<const gltHandle<target>*, texture_traits::max_gl_textures>
	texture_traits::currentTextures_{};
	*/

struct transformFeedbacks_traits;   //-
struct vertexArrays_traits;          //-






//////////////////////////////////////////////////////////////////////////////////////
//classes 
///////////////////////////////////////////////////////////////////////////////////////

template <buffer_traits::Target, class ... params>
class gltBuffer;

template <buffer_traits::Target target>
class gltBuffer<target>
{
    static_assert(target != buffer_traits::array_buffer, "Array buffer target is not allowed, use corresponding gltBuffer specialization");
    static_assert(target != buffer_traits::element_array_buffer, "Element Array buffer target is not allowed, use corresponding gltBuffer specialization");

    //TODO: add support for multiple threads. Use static map or something
    //static const gltHandle<target> *currentBuffer_;

    gltHandle<target> handle_ = buffer_traits::GenBuffer<target>();

public:
    //TODO: move semantics!

    void Bind()
    {
        buffer_traits::BindBuffer(handle_);
        //currentBuffer_ = &handle_;
    }

    void UnBind()
    {
        assert(IsCurrent() && "Trying to unbind non-active buffer object!");
        buffer_traits::BindBuffer<target>(0);
        currentBuffer_ = nullptr;
    }

    bool IsCurrent() const
    {
        if (!handle_)
            return false;
        return buffer_traits::IsCurrentHandle(handle);
    }

    ~gltBuffer()
    {
        if (IsCurrent())
            UnBind();
    }
};

//TODO: move to buffer_traits?
//template <buffer_traits::Target target>
//const gltHandle<target> gltBuffer<target, void>::*currentBuffer_ = nullptr;


//TODO: move validation to vao_traits?
// to validate a user-defined attribute
template <class attrib, class members_pack = typename refl_traits<attrib>::field_types>
struct vertex_attrib_validate;

template <class attrib, class ... fields>
struct vertex_attrib_validate<attrib, std::tuple<fields...>>
{
    constexpr static bool valid = (sizeof...(fields) <= 4 && 
        sizeof...(fields) &&
        all_same_v<fields...>) ? true : false;

};

template <class ... attribs>
constexpr bool all_valid_v = (vertex_attrib_validate<attribs>::valid && ...);

//tracking of active array_buffer should be where? in VAO?
template <class ... vaoAttribs>
class gltBuffer<buffer_traits::array_buffer, std::tuple<vaoAttribs...>>// : public Buffer_base
{
    gltHandle<buffer_traits::array_buffer> handle_ = buffer_traits::GenBuffer<buffer_traits::array_buffer>();

    mutable size_t vertexesLoaded_ = 0;

public:
    //assert vaoAttribs to meet gl requirements (1 - 4 elements, etc)
    //this somehow makes the compiler angry!
    //static_assert((vertex_attrib_validate<vaoAttribs>::valid && ...), "Invalid VAO attributes!");
    static_assert(all_valid_v<vaoAttribs...>, "Invalid VAO attributes!");

    template <class vertexInfo>
    void BufferData(const std::vector<vertexInfo>& vertexes, buffer_traits::Usage usage) const
    {
        static_assert(std::is_same_v<typename refl_traits<vertexInfo, std::tuple<vaoAttribs...>>::field_types,
            std::tuple<vaoAttribs...>>, "Attributes mismatch!");
        //assert vertexInfo to have attributes
        //glNamedBufferData(handle_, vertexes.size() * sizeof(vertexInfo), vertexes.data(), usage);     //glNamedBufferData is nullptr!!!!!

        buffer_traits::BufferData(buffer_traits::array_buffer, vertexes, usage);
        vertexesLoaded_ = vertexes.size();

    }

    const gltHandle<buffer_traits::array_buffer>& Handle() const
    {
        return handle_;
    }

    size_t VertexesLoaded() const
    {
        return vertexesLoaded_;
    }
};

template <texture_traits::Target target, texture_traits::Target texStorage = target>
class gltTexture : public texture_traits::DefinedTexParams_class<target>
{
	static_assert(texture_traits::target_validate<target, texStorage>::value,
		"Invalid texture storage!");

    gltHandle<target> handle_ = texture_traits::GenTexture<target>();

	mutable size_t texUnit_ = std::numeric_limits<size_t>::max();
public:

    gltTexture()
        : texture_traits::glTexParam_class_base/*<target>*/(std::bind(&gltTexture<target, texStorage>::IsCurrent, this))
    {
    }

    //to use when array of handles is created

	template <class other>
	gltTexture(other) = delete;
	template <class other>
	gltTexture(const other&) = delete;
	template <class other>
	gltTexture(other&&) = delete;

    gltTexture(gltHandle<target>&& handle)
        : texture_traits::glTexParam_class_base/*<target>*/(std::bind(&gltTexture<target, texStorage>::IsCurrent, this)),
        handle_(std::move(handle))
    {}

	const gltHandle<target>& GetHandle() const
	{
		return handle_;
	}

    bool IsCurrent() const
    {
		bool res = texture_traits::IsCurrentTexture(handle_);
		if (!res)
			texUnit_ = std::numeric_limits<size_t>::max();
        return res;
    }

    void BindToUnit(size_t texUnit)
    {
        texture_traits::BindToUnit(handle_, texUnit);
		texUnit_ = texUnit;
    }

	//unbind changing active unit!
    void UnBind()
    {
        assert(IsCurrent() && "Unbind texture wich is not currently active!");
        texture_traits::BindToUnit(gltHandle<target>(0), texUnit_);
    }

    void GenerateMipMap()
    {
        assert(IsCurrent() && "Updating parameters of non-active texture!");
        texture_traits::GenerateMipMap<target>();
    }
};


class VAO_base
{
    static GLuint vaoCurrent_;

protected:
    GLuint handle_ = 0;

public:
    VAO_base();

    VAO_base(const VAO_base&) = delete;
    VAO_base& operator=(const VAO_base&) = delete;

    VAO_base(VAO_base&& other);
    VAO_base& operator=(VAO_base&&);
    
    bool IsCurrent() const;
    virtual void Bind();
    virtual void UnBind();

    ~VAO_base();
};

template <typename>
class vao_enabler;

template <typename ... vaoAttribs>
class vao_enabler<std::tuple<vaoAttribs...>>
{
public:

    template <size_t nAttrib>
    static void VertexAttribPointer(bool normalized)
    {
        typedef std::tuple<vaoAttribs...> vaoPack;
        typedef std::tuple_element_t<nAttrib, std::tuple<vaoAttribs...>> curAttrib;
        typedef std::tuple_element_t<0, refl_traits<curAttrib>::field_types> cType;

        glVertexAttribPointer((GLuint)nAttrib,
            (GLuint)refl_traits<curAttrib>::fields_count(),
            gl_types_map::found_pair<cType>::value::value,
            (GLboolean)normalized,
            mem_layout_info<vaoPack>::class_size,
            (const void*)mem_layout_info<vaoPack>::padding<nAttrib>
        );
    }

    static void EnableVertexAttrib(size_t attrib)
    {
        glEnableVertexAttribArray((GLuint)attrib);
    }

private:

    //to generate sizeof...(vaoAttribs) number of bool arguments for "normalized" parameters
    template<class From>
    using to_bool = bool;

    template <size_t ... indx>
    static void VertexAttribPointersPrivate(std::index_sequence<indx...>&&, to_bool<vaoAttribs> ... normalized)
    {
        (VertexAttribPointer<indx>(normalized), ...);
    }

    template <size_t ... indx>
    static void EnableVertexAttributesPrivate(std::index_sequence<indx...>&&)
    {
        (EnableVertexAttrib(indx), ...);
    }

public:

    void VertexAttribPointers(to_bool<vaoAttribs> ... normalized)
    {
        VertexAttribPointersPrivate(std::make_index_sequence<sizeof...(vaoAttribs)>(), normalized...);
    }

    void EnableVertexAttributes()
    {
        EnableVertexAttributesPrivate(std::make_index_sequence<sizeof...(vaoAttribs)>());
    }

};


template <class>
class gltVAO;

template <class ... vaoAttribs>
class gltVAO<std::tuple<vaoAttribs...>> : public VAO_base, public vao_enabler<std::tuple<vaoAttribs...>>
{
    static_assert(all_valid_v<vaoAttribs...>, "Invalid VAO attributes!");
public:
    using VBO = gltBuffer<buffer_traits::array_buffer, std::tuple<vaoAttribs...>>;

private:
    VBO vbo_{};
    //gltBuffer<buffer_traits //add ebo
    bool vboPending_ = true;

public:

    using vao_enabler<std::tuple<vaoAttribs...>>::VertexAttribPointers;
    using vao_enabler<std::tuple<vaoAttribs...>>::EnableVertexAttributes;

    const VBO& GetVBO() const
    {
        return vbo_;
    }

    void AssignVBO(VBO&& vbo)
    {
        vbo_ = std::move(vbo);
        if (!IsCurrent())
        {
            vboPending_ = true;
            return;
        }
        buffer_traits::BindBuffer<buffer_traits::array_buffer>(vbo_.Handle());
    }

    void Bind() override
    {
        VAO_base::Bind();
        if (vboPending_)
        {
            buffer_traits::BindBuffer<buffer_traits::array_buffer>(vbo_.Handle());
            vboPending_ = false;
        }
    }

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//primitives traits
////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//texture traits
////////////////////////////////////////////////////////////////


//TODO: add handle creator functions??
typedef cexpr_generic_map<
    cexpr_pair<buffer_traits::Target, auto_t<&glDeleteBuffers>>,
    // cexpr_pair<target_enum<frameBuffer_traits>,          auto_t<&glDeleteFramebuffers>>,
    // cexpr_pair<target_enum<programm_traits>,             auto_t<&glDeleteProgram>>,
    // cexpr_pair<target_enum<programPipelines_traits>,     auto_t<&glDeleteProgramPipelines>>,
    // cexpr_pair<target_enum<renderBuffers_traits>,        auto_t<&glDeleteRenderbuffers>>,
   //  cexpr_pair<target_enum<samplers_traits>,             auto_t<&glDeleteSamplers>>,
    // cexpr_pair<target_enum<shader_traits>,               auto_t<&glDeleteShader>>,
    // cexpr_pair<target_enum<sync_traits>,                 auto_t<&glDeleteSync>>,
    cexpr_pair<texture_traits::Target, auto_t<&glDeleteTextures>>
    //  cexpr_pair<target_enum<transformFeedbacks_traits>,   auto_t<&glDeleteTransformFeedbacks>>,
    //  cexpr_pair<target_enum<vertexArrays_traits>,         auto_t<&glDeleteBuffers>>
> gl_deleters;


//This definition must go after all gl_traits and Target enums are registered in 
template<auto target>
void gltHandle<target, typename decltype(target)>::DestroyHandle()
{
    constexpr auto pDestroy =
        gl_deleters::found_pair<decltype(target)>::value::value;
    (*pDestroy)(1, &handle_);
}
