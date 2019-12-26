#pragma once

#include "basic_types.hpp"

// what to do with this????
#include "ParseAlgorithm.h"

#include <algorithm>
#include <utility>

namespace glt
{
	struct shader_traits
	{
		static inline ParseAlgorithm<ShaderFileInfo::text_source> algorithm;

		template <class = void, class = void>
		struct source_checker
		{
			static bool check_source(std::string_view)
			{
				return true;
			}
		};

		template <typename ... VarsIn, typename ... VarsOut>
		struct source_checker<std::tuple<VarsIn...>, std::tuple<VarsOut...>>
		{
			static inline ParseAlgorithm<ShaderFileInfo::text_source> algorithm;

			static bool check_source(std::string_view source)
			{
				std::vector<Variable> vars = algorithm.Parse(source);
				std::vector<Variable>::iterator start_remove = 
					std::remove_if(vars.begin(), vars.end(),
					[](const Variable& var)
				{
					return var.type != Variable::var_in &&
						var.type != Variable::vertex_in &&
						var.type != Variable::var_out;
				});
				vars.erase(start_remove, vars.cend());

				return true;
			}
		};

		template <class TupleIn = void, class TupleOut = void>
		static bool check_source(std::string_view source)
		{
			return source_checker<TupleIn, TupleOut>::check_source(source);
		}

		static bool Compile(const HandleShader& handle, const std::string& source)
		{
			return Compile(std::forward<const HandleShader&>(handle),
				std::forward<const char*>(source.c_str()), std::forward<size_t>(source.size()));
		}

		static bool Compile(const HandleShader& handle, const char *source, size_t length)
		{
			assert(handle && "Shader::Invalid shader handle!");

			glShaderSource(handle_accessor(handle), 1, &source, (GLint*)&length);
			glCompileShader(handle_accessor(handle));
			GLint res = false;
			glGetShaderiv(handle_accessor(handle), GL_COMPILE_STATUS, &res);
			return (bool)res;
		}
	};


	template <ShaderTarget target, class VarsIn = std::tuple<>, class VarsOut = std::tuple<>>
	class Shader
	{
		HandleShader handle_;
		bool compiled_ = false;

	public:

		// default case
		constexpr Shader(HandleShader&& handle = Allocator::Allocate(target)) noexcept
			: handle_(std::move(handle))
		{}

		template <size_t length>
		constexpr Shader(const char(&source)[length],
			HandleShader&& handle = Allocator::Allocate(target))
			: handle_(std::move(handle)),
			compiled_(shader_traits::Compile(handle_, source, length))
		{
			assert(IsValid() && "Shader::Failed to compile shader!");
		}

		constexpr Shader(const char* source, size_t length,
			HandleShader&& handle = Allocator::Allocate(target))
			: handle_(std::move(handle)),
			compiled_(shader_traits::Compile(handle_, source, length))
		{
			assert(IsValid() && "Shader::Failed to compile shader!");
		}

		constexpr Shader(const std::string& source,
			HandleShader&& handle = Allocator::Allocate(target))
			: handle_(std::move(handle)),
			compiled_(shader_traits::Compile(handle_, source.data(), source.size()))
		{
			assert(IsValid() && "Shader::Failed to compile shader!");
		}

		// shaders actually may be allowed to be copied, but what about the handle?
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		bool Compile(const std::string& source)
		{
			assert(shader_traits::check_source<VarsIn, VarsOut>(std::string_view(source)) &&
				"Invalid shader source file! Variables mismatch!");
			compiled_ = shader_traits::Compile(handle_, source.data(), source.length());
			return IsValid();
		}

		bool Compile(const char *source, size_t length)
		{
			assert(shader_traits::check_source<VarsIn, VarsOut>(std::string_view(source, length)) &&
				"Invalid shader source file! Variables mismatch!");
			compiled_ = shader_traits::Compile(handle_, source, length);
			return IsValid();
		}

		template <size_t length>
		bool Compile(const char(&source)[length])
		{
			assert(shader_traits::check_source<VarsIn, VarsOut>(std::string_view(source, length)) &&
				"Invalid shader source file! Variables mismatch!");
			compiled_ = shader_traits::Compile(handle_, source, length);
			return IsValid();
		}

		constexpr bool IsValid() const noexcept
		{
			return compiled_;
		}

		constexpr operator bool() const noexcept
		{
			return IsValid();
		}

		constexpr const HandleShader& GetHandle() const noexcept
		{
			return handle_;
		}

	};

	using VertexShader = Shader<ShaderTarget::vertex>;
	using FragmentShader = Shader<ShaderTarget::fragment>;
}