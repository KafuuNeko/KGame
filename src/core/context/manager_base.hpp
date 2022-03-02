#pragma once

#include <memory>
#include "context_fwd.hpp"

namespace core
{

class ManagerBase {
protected:
    ManagerBase(std::shared_ptr<Context> context)
        : mContext(std::move(context)) {}
        
    std::shared_ptr<Context> mContext;

public:
    virtual ~ManagerBase() noexcept = default;

    std::shared_ptr<Context> getContext() const noexcept {
        return mContext;
    }
};

}
