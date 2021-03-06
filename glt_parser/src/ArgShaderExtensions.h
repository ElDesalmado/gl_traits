#pragma once

#include "IParser.h"

#include "IDataType.h"


class ArgShaderExtensions : public IArgument
{
    ShaderFileInfo::ShaderType type_;
    std::set<std::string_view> extensions_;

    static inline std::regex extPat{ R"([.]\w+)" };

public:

    ArgShaderExtensions(const char *tag,
        const char *name,
        const char *descr, ShaderFileInfo::ShaderType type, bool validByDefault = false);

    virtual bool SetValue(std::string_view s) override;

    std::pair<ShaderFileInfo::ShaderType,
        const std::set<std::string_view>&> SpecValue() const;

    std::any SpecificValue() const;
};