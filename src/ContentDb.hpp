
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "gas/Fuel.hpp"

namespace gas
{
    class Node;
}

namespace ehb
{
    class IFileSys;
    class ContentDb final
    {
    public:

        void init(IFileSys& fileSys, const std::string& directory = "/world/contentdb/templates/");

        //! query a string from a given template, for example: "2w_gargoyle:aspect:experience_value"
        const std::string& queryString(const std::string& query, const std::string& defaultValue = "") const;

        const FuelBlock* getGameObjectTmpl(const std::string& tmpl) const;

    private:

        std::unordered_map<std::string, std::unique_ptr<FuelBlock>> db;
    };
}
