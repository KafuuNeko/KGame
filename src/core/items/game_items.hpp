#pragma once

#include "context/context.hpp"
#include "sqlite/sqlite3.hpp"

#include <cinttypes>
#include <string>
#include <string_view>

namespace core::game_items {

struct Record {
    int64_t itemId;
    int64_t itemBaseId;
    std::string itemName;
    std::string itemProperties;
};

inline int64_t insert(const std::shared_ptr<Context>& context, int64_t itemBaseId, std::string_view itemProperties) noexcept
{
    return context->getGameDB().singleStep("INSERT INTO GameItems(itemBaseId, itemName, itemProperties) VALUES(?, NULL, ?)", itemBaseId, itemProperties) ? context->getGameDB().last_insert_rowid() : 0;
}

inline bool updateProperties(const std::shared_ptr<Context>& context, int64_t itemId, std::string_view itemProperties) noexcept
{
    return context->getGameDB().singleStep("UPDATE GameItems SET itemProperties=? WHERE itemId=?", itemId, itemProperties, itemId) && context->getGameDB().changes() > 0;
}

inline bool updateName(const std::shared_ptr<Context>& context, int64_t itemId, std::string_view itemName) noexcept
{
    return context->getGameDB().singleStep("UPDATE GameItems SET itemName=? WHERE itemId=?", itemId, itemName, itemId) && context->getGameDB().changes() > 0;
}

}
