#pragma once

#include "context/context_fwd.hpp"
#include "context/manager_base.hpp"
#include <memory>

namespace core {

class PlayerManager : public ManagerBase {
    friend class BasicItems;

public:
    explicit PlayerManager(const std::shared_ptr<Context> &context)
        : ManagerBase(context) {};

};

}

