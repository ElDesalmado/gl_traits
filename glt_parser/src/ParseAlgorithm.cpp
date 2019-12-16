﻿#include "ParseAlgorithm.h"

#include <filesystem>

#include <fstream>
#include <streambuf>
#include <regex>

#include <cassert>

namespace fsys = std::filesystem;

struct glsl_variable_info
{
	using Mask = unsigned char;

	// out in loc unif layout
	constexpr static Mask m_empty = 0b00000000,
		m_layout = 0b00000001,
		m_uniform = 0b00000010,
		m_location = 0b00000100,
		m_in = 0b00001000,
		m_out = 0b00010000,

		m_vertex_loc_in_type = m_layout | m_location | m_in,
		m_vertex_in_type = m_layout | m_in,
		m_uniform_loc_type = m_uniform | m_location,
		m_uniform_type = m_uniform,
		m_var_in_type = m_in,
		m_var_out_type = m_out;

	constexpr static size_t layout_slots = 5;

	Mask mask_;
	IVariable::VarType var_type_;

	glsl_variable_info(const bool(&layout_pat)[layout_slots])
		: mask_(GetMask(layout_pat, std::make_index_sequence<layout_slots>())),
		var_type_(DeduceType(mask_))
	{

	}

	IVariable::VarType GetVarType() const
	{
		return var_type_;
	}

	// TODO: chande this! ITS VERY CLUMSY 
	template <size_t sz, size_t ... indx>
	constexpr static IVariable::VarType DeduceType(const bool(&layout_pat)[sz],
		std::index_sequence<indx...>)
	{
		switch (((layout_pat[indx] << indx) | ...))
		{
		case m_out:
			return IVariable::VarType::var_out;
		case m_in:
			return IVariable::VarType::var_in;
		case m_uniform_type:
		case m_uniform_loc_type:
			return IVariable::VarType::uniform;
		case m_vertex_in_type:
		case m_vertex_loc_in_type:
			return IVariable::VarType::vertex_in;
		default:
			return IVariable::VarType::unknown;
		}
	}


	template <size_t sz, size_t ... indx>
	constexpr static Mask GetMask(const bool(&layout_pat)[sz],
		std::index_sequence<indx...>)
	{
		return ((layout_pat[indx] << indx) | ...);
	}

	static IVariable::VarType DeduceType(Mask mask)
	{
		switch (mask)
		{
		case m_out:
			return IVariable::VarType::var_out;
		case m_in:
			return IVariable::VarType::var_in;
		case m_uniform_type:
		case m_uniform_loc_type:
			return IVariable::VarType::uniform;
		case m_vertex_in_type:
		case m_vertex_loc_in_type:
			return IVariable::VarType::vertex_in;
		default:
			return IVariable::VarType::unknown;
		}
	}

};


template <>
std::vector<std::unique_ptr<IVariable>> 
ParseAlgorithm<ISourceFile::text_source>::Parse(const std::string& filePath)
{
	std::vector<std::unique_ptr<IVariable>> out;
	static std::regex regVarGLSL
	{
		R"((?:(layout)\s+|(uniform)\s+)?)"
		R"((?:[(]location\s*=\s*(\d+)\s*[)]\s+)?)"
		R"((?:(in)\s+|(out)\s+)?)"
		R"((\w+)\s+)"
		R"((\w+)\s*;)"
	};

	static size_t subs = regVarGLSL.mark_count() + 1;

	std::fstream f{ filePath, std::fstream::in };
	if (!f.is_open())
		throw std::exception(std::string("Failed to open for parsing: " +
			filePath).data());

	std::string fileLoaded{ std::istreambuf_iterator<char>(f),
			std::istreambuf_iterator<char>() };

	// TODO: do not load huge files. Load file only up to "void main()"

	std::sregex_iterator start{ fileLoaded.cbegin(),
		fileLoaded.cend(),
		regVarGLSL },
		end{};

	size_t found = std::distance(start, end);

	while (start != end)
	{
		bool pat[glsl_variable_info::layout_slots];

		const std::smatch& sm = *start;
		auto iter = ++sm.cbegin();

		for (size_t i = 0; i != glsl_variable_info::layout_slots; ++i)
			pat[i] = (iter++)->matched;

		auto &type = (iter++)->str(),
			&name = (iter++)->str();

		IVariable::VarType var_type =
			glsl_variable_info::DeduceType(pat,
				std::make_index_sequence<glsl_variable_info::layout_slots>());

		assert(var_type != IVariable::unknown && "Unknown variable type received!");

		out.emplace_back(IVariable::Create(name, type, var_type));

		++start;
	}

	return out;
}

template <>
std::vector<std::unique_ptr<IVariable>>
ParseAlgorithm<ISourceFile::header_common>::Parse(const std::string& filePath)
{
	std::vector<std::unique_ptr<IVariable>> out;

	return out;
}