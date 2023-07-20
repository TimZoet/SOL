#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class UpdateTextureManagerTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateTextureManagerTask();

        UpdateTextureManagerTask(std::string taskName, ITaskResource<TextureManager>* manager);

        UpdateTextureManagerTask(const UpdateTextureManagerTask&) = delete;

        UpdateTextureManagerTask(UpdateTextureManagerTask&&) noexcept;

        ~UpdateTextureManagerTask() noexcept override;

        UpdateTextureManagerTask& operator=(const UpdateTextureManagerTask&) = delete;

        UpdateTextureManagerTask& operator=(UpdateTextureManagerTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<TextureManager>* textureManager = nullptr;
    };
}  // namespace sol
