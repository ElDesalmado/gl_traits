#include "ArgShaderExtensions.h"

ArgShaderExtensions::ArgShaderExtensions(const char * tag, const char * name, const char * descr, ShaderFileInfo::ShaderType type, bool validByDefault)
    : IArgument(tag, name, validByDefault),
    type_(type)
{
    IArgument::SetDescription(descr);
}

bool ArgShaderExtensions::SetValue(std::string_view s)
{
    extensions_.clear();

    static std::match_results<std::string_view::const_iterator> sm;


    if (s.empty() )//||
       // !std::regex_match(s.cbegin(), s.cend(), sm, extPat))
    {
        valid_ = false;
        return IsValid();
    }

    std::regex_iterator<std::string_view::const_iterator> start{ s.cbegin(), s.end(), extPat},
        end{};

    while (start != end)
    {
#pragma message("Need to check if all the strings are valid extensions")

        extensions_.emplace(std::next(s.data(), start->position()),
            (size_t)start->length());
        ++start;
    }

    valid_ = (bool)extensions_.size();
    return IsValid();
}

std::pair<ShaderFileInfo::ShaderType, const std::set<std::string_view>&> ArgShaderExtensions::SpecValue() const
{
    return std::pair<ShaderFileInfo::ShaderType,
        const std::set<std::string_view>&>(type_, extensions_);
}

std::any ArgShaderExtensions::SpecificValue() const
{
    return SpecValue();
}
