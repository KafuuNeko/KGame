#pragma once

#include "context/context.hpp"
#include "sqlite/sqlite3.hpp"

#include <cinttypes>
#include <string>
#include <string_view>

namespace core::basic_items {

struct Record {
    int64_t itemBaseId;
    std::string itemName;
    std::string itemDescribe;
    int32_t itemCateogory;
    std::string itemProperties;
};

/**
 * @brief 通过基础物品id查询记录
 * @return 若返回空指针则表示查询失败
 */
inline std::unique_ptr<Record> queryById(const std::shared_ptr<Context>& context, int64_t itemBaseId)
{
    auto stmt = context->getGameDB().query("BasicItems", "itemName, itemDescribe, itemCateogory, itemProperties", "WHERE itemBaseId=@id");
    if (!stmt.bind(itemBaseId) || stmt.step() != SQLITE_ROW) {
        return nullptr;
    }

    return std::unique_ptr<Record>(new Record { itemBaseId, stmt.column_text(0), stmt.column_text(1), stmt.column_int32(2), stmt.column_text(3) });
}

/**
 * @brief 添加一条基础物品记录
 * @return 新基础物品id
 */
inline int64_t insert(const std::shared_ptr<Context>& context, std::string_view name, std::string_view describe, int32_t cateogory, std::string_view properties)
{
    return context->getGameDB().singleStep("INSERT INTO BasicItems(itemName, itemDescribe, itemCateogory, itemProperties) VALUES(?, ?, ?, ?)",
               name, describe, cateogory, properties)
        ? context->getGameDB().last_insert_rowid()
        : 0;
}

/**
 * @brief 更新基础物品记录
 */
inline bool update(const std::shared_ptr<Context>& context, const Record& record)
{
    return context->getGameDB().singleStep("UPDATE BasicItems SET itemName=?, itemDescribe=?, itemCateogory=?, itemProperties=? WHERE itemBaseId=?",
               record.itemName, record.itemDescribe, record.itemCateogory, record.itemProperties, record.itemBaseId)
        && context->getGameDB().changes() > 0;
}

/**
 * @brief 移除指定基础物品id
 * 注意：在调用此函数前必须保证已经将移除的基础物品有关联的记录全部删除，否则调用失败
 */
inline bool removeById(const std::shared_ptr<Context>& context, int64_t itemBaseId)
{
    return context->getGameDB().singleStep("DELETE FROM BasicItems WHERE itemBaseId=?", itemBaseId) && context->getGameDB().changes() > 0;
}

}
