#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class MeshLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Attribute
        {
            /**
             * \brief Attribute name.
             */
            std::string name;

            /**
             * \brief Location index.
             */
            uint32_t location = 0;

            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Format.
             */
            VkFormat format = VK_FORMAT_UNDEFINED;

            /**
             * \brief Offset in bytes.
             */
            uint32_t offset = 0;
        };

        struct Binding
        {
            /**
             * \brief Binding name.
             */
            std::string name;

            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Stride in bytes.
             */
            uint32_t stride = 0;

            /**
             * \brief Input rate.
             */
            VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MeshLayout();

        MeshLayout(MeshManager& manager, uuids::uuid id);

        MeshLayout(const MeshLayout&) = delete;

        MeshLayout(MeshLayout&&) = delete;

        ~MeshLayout() noexcept;

        MeshLayout& operator=(const MeshLayout&) = delete;

        MeshLayout& operator=(MeshLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the MeshManager.
         * \return MeshManager.
         */
        [[nodiscard]] MeshManager& getMeshManager() noexcept;

        /**
         * \brief Get the MeshManager.
         * \return MeshManager.
         */
        [[nodiscard]] const MeshManager& getMeshManager() const noexcept;

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

        [[nodiscard]] bool isFinalized() const noexcept;

        /**
         * \brief Get the attribute descriptions.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return List of attribute descriptions.
         */
        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const;

        /**
         * \brief Get the binding descriptions.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return List of binding descriptions.
         */
        [[nodiscard]] const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the user friendly name.
         * \param value Name.
         */
        void setName(std::string value);

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add an attribute.
         * \param attrName Optional attribute name.
         * \param location Attribute location.
         * \param binding Attribute binding index.
         * \param format Attribute format.
         * \param offset Offset in bytes.
         * \return New attribute.
         */
        Attribute&
          addAttribute(std::string attrName, uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

        /**
         * \brief Add a binding.
         * \param bindingName Optional binding name.
         * \param binding Binding index.
         * \param stride Stride in bytes.
         * \param inputRate Vertex input rate.
         * \return New binding.
         */
        Binding& addBinding(std::string bindingName, uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

        /**
         * \brief Finalize this layout, creating the attribute and binding descriptions. The layout will no longer be modifiable.
         */
        void finalize();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief MeshManager.
         */
        MeshManager* meshManager = nullptr;

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief User friendly name.
         */
        std::string name;

        bool finalized = false;

        /**
         * \brief List of attributes.
         */
        std::vector<Attribute> attributes;

        /**
         * \brief List of bindings.
         */
        std::vector<Binding> bindings;

        /**
         * \brief List of finalized attribute descriptions.
         */
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        /**
         * \brief List of finalized binding descriptions.
         */
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    };
}  // namespace sol