#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/resources/i_task_resource.h"

namespace sol
{
    class IndexResource final : public ITaskResource
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IndexResource() = delete;

        IndexResource(TaskGraph& taskGraph, uint32_t r);

        IndexResource(const IndexResource&) = delete;

        IndexResource(IndexResource&&) = delete;

        ~IndexResource() noexcept override;

        IndexResource& operator=(const IndexResource&) = delete;

        IndexResource& operator=(IndexResource&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] uint32_t getRange() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        uint32_t range = 0;
    };
}  // namespace sol
