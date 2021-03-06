﻿#pragma once

#include "IShaderParser.h"
#include "IVariable.h"

#include <vector>
#include <list>

struct ShaderInfo : public IShaderInfo
{
	std::string filePath;

	std::vector<std::unique_ptr<IVariable>> vao_,
		uniforms_,
		var_in_,
		var_out;


	ShaderInfo(const std::string& fpath);

	size_t VarsCount(IVariable::VarType type) const override;
};


class ShaderParser : public IShaderParser
{
	struct glsl_variable_info;

	struct ParseImpl;
	

	std::string name_;
	std::list<ShaderInfo> shaders_;

	static std::unique_ptr<ParseImpl> parseImpl_;

	using container_t = decltype(shaders_);


	

public:
	ShaderParser(const std::string& fpath,
		const std::string& namePred,
		const std::string& extensions = std::string());

	const std::string& ProgName() const;
	size_t SourceFilesCount(const std::string& extension) const override;

	std::optional<crIShaderInfo> GetShader(size_t indx,
		const std::string& extension = std::string()) const override;

	void ParseSource(size_t indx,
		const std::string& extension = std::string()) override;


private:

	static container_t GetFilePaths(const std::string& fpath,
		const std::string& namePred, const std::string& extensions);

	static void ParseSource(ShaderInfo& sInf);

};


