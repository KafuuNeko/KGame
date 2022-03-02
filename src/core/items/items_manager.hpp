#pragma once

#include "context/manager_base.hpp"

#include <memory>
#include <string_view>

namespace core {

class ItemsManager : public ManagerBase {
    friend class BasicItems;

public:
    explicit ItemsManager(std::shared_ptr<Context> context)
        : ManagerBase(std::move(context))
    {
    }

private:
};

}
