#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>

namespace sol
{
    class MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class UpdateDetection : uint32_t
        {
            /**
             * \brief Query material instance to see if data was updated.
             */
            Manual = 0,

            /**
             * \brief Automatically detect if the data changed by comparing it with the previous data.
             */
            Automatic = 1,

            /**
             * \brief Always update data, regardless of any changes.
             */
            Always = 2
        };

        enum class UpdateFrequency : uint32_t
        {
            /**
             * \brief Data is never updated after it is set for the first time.
             */
            Never = 0,

            /**
             * \brief Data is not expected to be updated often.
             */
            Low = 1,

            /**
             * \brief Data is expected to be updated at a high frequency, though not every frame.
             */
            High = 2,

            /**
             * \brief Data is expected to update every frame.
             */
            Frame = 3
        };

        enum class SharingMethod
        {
            /**
             * \brief Do not share any buffers.
             */
            None = 0,

            /**
             * \brief Share buffers for all bindings of a single material instance.
             */
            Instance = 1,

            /**
             * \brief Share buffers for the same binding of multiple material instances.
             */
            Binding = 2,

            /**
             * \brief Share buffers for multiple bindings of multiple material instances.
             */
            InstanceAndBinding = 3
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaterialLayout() = default;

        MaterialLayout(const MaterialLayout&) = delete;

        MaterialLayout(MaterialLayout&&) = delete;

        virtual ~MaterialLayout() = default;

        MaterialLayout& operator=(const MaterialLayout&) = delete;

        MaterialLayout& operator=(MaterialLayout&&) = delete;
    };
}  // namespace sol