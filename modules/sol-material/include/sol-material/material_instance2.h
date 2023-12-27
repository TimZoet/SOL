#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-descriptor/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class MaterialInstance2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct DescriptorPair
        {
            Descriptor& descriptor;
            size_t      set;
        };

        struct DescriptorIterator
        {
            using difference_type = std::make_signed_t<size_t>;
            using element_type    = DescriptorPair;

            DescriptorIterator() { throw std::runtime_error("Not implemented"); }

            DescriptorIterator(MaterialInstance2& mtl, const size_t index) : material(&mtl), set(index) {}

            [[nodiscard]] element_type operator*() const
            {
                return {.descriptor = *material->descriptors[set], .set = set};
            }

            auto& operator++()
            {
                // Next set may be null, look for first enabled one.
                do {
                    set++;
                } while (set < material->descriptors.size() && !material->descriptors[set]);

                return *this;
            }

            auto operator++(int)
            {
                const auto tmp = *this;
                ++*this;
                return tmp;
            }

            auto operator<=>(const DescriptorIterator& rhs) const noexcept = default;// { return set <=> rhs.set; }

        private:
            MaterialInstance2* material = nullptr;
            size_t             set      = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaterialInstance2() = delete;

        explicit MaterialInstance2(Material2& mtl);

        MaterialInstance2(const MaterialInstance2&) = delete;

        MaterialInstance2(MaterialInstance2&&) = delete;

        virtual ~MaterialInstance2() noexcept;

        MaterialInstance2& operator=(const MaterialInstance2&) = delete;

        MaterialInstance2& operator=(MaterialInstance2&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the UUID.
         * \return UUID.
         */
        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        /**
         * \brief Get the user friendly name.
         * \return Name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get the material this object is an instance of.
         * \return Material.
         */
        [[nodiscard]] Material2& getMaterial() noexcept;

        /**
         * \brief Get the material this object is an instance of.
         * \return Material.
         */
        [[nodiscard]] const Material2& getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the user friendly name.
         * \param value Name.
         */
        void setName(std::string value);

        ////////////////////////////////////////////////////////////////
        // Descriptors.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Enable a descriptor. Will be allocated from the provided buffer. If descriptor was already enabled, this does nothing.
         * \param index Index of the descriptor.
         * \param buffer Buffer to allocate new descriptor from.
         * \return New descriptor.
         */
        Descriptor& enableDescriptor(size_t index, DescriptorBuffer& buffer);

        /**
         * \brief Enable a descriptor. Will be allocated from the provided buffer. If descriptor was already enabled, this does nothing.
         * \param layout Layout to enable descriptor for. Must be part of this material.
         * \param buffer Buffer to allocate new descriptor from.
         * \return New descriptor.
         */
        Descriptor& enableDescriptor(const DescriptorLayout& layout, DescriptorBuffer& buffer);

        /**
         * \brief Disable a descriptor. Will also deallocate the descriptor.
         * \param index Index of the descriptor.
         */
        void disableDescriptor(size_t index);

        /**
         * \brief Disable a descriptor. Will also deallocate the descriptor.
         * \param layout Layout to disable descriptor for. Must be part of this material.
         */
        void disableDescriptor(const DescriptorLayout& layout);

        /**
         * \brief Get a descriptor.
         * \param index Index of the descriptor.
         * \return Descriptor.
         */
        [[nodiscard]] Descriptor& operator[](size_t index) const;

        /**
         * \brief Get a descriptor.
         * \param layout Layout to get descriptor for. Must be part of this material.
         * \return Descriptor.
         */
        [[nodiscard]] Descriptor& operator[](const DescriptorLayout& layout) const;

        [[nodiscard]] DescriptorIterator begin();

        [[nodiscard]] DescriptorIterator end();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief User friendly name.
         */
        std::string name;

        Material2* material = nullptr;

        std::vector<DescriptorPtr> descriptors;
    };
}  // namespace sol
