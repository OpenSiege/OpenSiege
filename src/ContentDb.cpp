
#include "ContentDb.hpp"

#include <vector>
#include <spdlog/spdlog.h>
#include <osgDB/FileNameUtils>
#include "IFileSys.hpp"

namespace ehb
{
    void ContentDb::init(IFileSys& fileSys, const std::string& directory)
    {
        auto log = spdlog::get("log");
        // log->set_level(spdlog::level::debug);
        log->debug("Starting init of ContentDb");

        std::vector<std::unique_ptr<Fuel>> docs;
        std::unordered_map<std::string, FuelBlock*> tmplMap;

        fileSys.eachGasFile(directory, [&docs, &tmplMap](const std::string& filename, auto doc)
            {
                for (auto node : doc->eachChild())
                {
                    const auto result = tmplMap.emplace(osgDB::convertToLowerCase(node->name()), node);

                    if (result.second != true)
                    {
                        spdlog::get("log")->warn("{}: duplicate entry {} found", filename, node->name());
                    }
                }

                docs.emplace_back(std::move(doc));
            });

        log->debug("ContentDb is resolving {} templates", tmplMap.size());

        std::function<void(const std::string&)> resolve;

        resolve = [&resolve, &tmplMap, this](const std::string& name)
        {
            auto log = spdlog::get("log");

            if (db.count(name) == 0)
            {
                if (const auto itr = tmplMap.find(name); itr != tmplMap.end())
                {
                    FuelBlock* node = itr->second;

                    const std::string specializes = osgDB::convertToLowerCase(node->valueOf("specializes"));

                    FuelBlock* super = nullptr;

                    if (!specializes.empty())
                    {
                        resolve(specializes);

                        if (const auto itr = db.find(specializes); itr != db.end())
                        {
                            super = itr->second.get();
                        }
                    }

                    FuelBlock* newNode = super ? super->clone() : node->clone();

                    if (super)
                    {
                        node->merge(newNode);
                    }

                    db.emplace(name, newNode);
                }
                else
                {
                    log->error("could not find {}", name);
                }
            }
        };

        for (const auto& entry : tmplMap)
        {
            resolve(entry.first);
        }

        log->debug("ContentDB has finished loading and resolving {} templates", db.size());
    }

    const std::string& ContentDb::queryString(const std::string& query, const std::string& defaultValue) const
    {
        if (const auto colon = query.find(':'); colon != std::string::npos)
        {
            if (const auto itr = db.find(query.substr(0, colon)); itr != db.end())
            {
                return itr->second->valueOf(query.substr(colon + 1));
            }
        }

        return defaultValue;
    }

    const FuelBlock* ContentDb::getGameObjectTmpl(const std::string& tmpl) const
    {
        const auto itr = db.find(tmpl);

        return itr != db.end() ? itr->second.get() : nullptr;
    }
}
