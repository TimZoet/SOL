#include "sol-mesh/mesh_layout.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MeshLayout::MeshLayout() = default;

    MeshLayout::MeshLayout(MeshManager& manager, const uuids::uuid id) : meshManager(&manager), uuid(id) {}

    MeshLayout::~MeshLayout() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MeshManager& MeshLayout::getMeshManager() noexcept { return *meshManager; }

    const MeshManager& MeshLayout::getMeshManager() const noexcept { return *meshManager; }

    const uuids::uuid& MeshLayout::getUuid() const noexcept { return uuid; }

    const std::string& MeshLayout::getName() const noexcept { return name; }

    bool MeshLayout::isFinalized() const noexcept { return finalized; }

    const std::vector<VkVertexInputAttributeDescription>& MeshLayout::getAttributeDescriptions() const
    {
        if (!isFinalized()) throw SolError("Cannot get attribute descriptions. Mesh layout was not yet finalized.");
        return attributeDescriptions;
    }

    const std::vector<VkVertexInputBindingDescription>& MeshLayout::getBindingDescriptions() const
    {
        if (!isFinalized()) throw SolError("Cannot get binding descriptions. Mesh layout was not yet finalized.");
        return bindingDescriptions;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MeshLayout::setName(std::string value) { name = std::move(value); }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    MeshLayout::Attribute& MeshLayout::addAttribute(std::string    attrName,
                                                    const uint32_t location,
                                                    const uint32_t binding,
                                                    const VkFormat format,
                                                    const uint32_t offset)
    {
        if (isFinalized()) throw SolError("Mesh layout was already finalized.");
        return attributes.emplace_back(std::move(attrName), location, binding, format, offset);
    }

    MeshLayout::Binding& MeshLayout::addBinding(std::string             bindingName,
                                                const uint32_t          binding,
                                                const uint32_t          stride,
                                                const VkVertexInputRate inputRate)
    {
        if (isFinalized()) throw SolError("Mesh layout was already finalized.");
        return bindings.emplace_back(std::move(bindingName), binding, stride, inputRate);
    }

    void MeshLayout::finalize()
    {
        if (isFinalized()) throw SolError("Mesh layout was already finalized.");
        if (attributes.empty() && bindings.empty()) throw SolError("Cannot finalize an empty mesh layout");

        // Create attribute descriptions.
        attributeDescriptions.reserve(attributes.size());
        for (const auto& [name, location, binding, format, offset] : attributes)
            attributeDescriptions.emplace_back(location, binding, format, offset);

        // Create binding descriptions.
        bindingDescriptions.reserve(bindings.size());
        for (const auto& [name, binding, stride, inputRate] : bindings)
            bindingDescriptions.emplace_back(binding, stride, inputRate);

        finalized = true;
    }
}  // namespace sol
