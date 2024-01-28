#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor_buffer.h"
#include "sol-material/graphics/graphics_material_instance2.h"
#include "sol-mesh/mesh.h"
#include "sol-scenegraph/scenegraph.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"
#include "testutils/meshes.h"

class Scenegraphs
{
public:
    struct Graphics
    {
        enum class Name
        {
            // A simple scene using two different materials with 2 descriptors each.
            // Only 3 of the meshes are preceded by a valid material and dynamic state stack.
            //                                          ┌────┐
            //                                          │root├──────────────────────────────────────────────┐
            //                                          └─┬──┘                                              │
            //                                            │                                                 │
            //                                            │                                                 │
            //                                    ┌───────▼────────┐                                        │
            //                                 ┌──┤SCISSOR│VIEWPORT├──┐                                     │
            //                                 │  └───────┴────────┘  │                                     │
            //                                 │                      │                                     │
            //                             ┌───▼───┬───┬───┐      ┌───▼───┬───┬───┐                     ┌───▼───┬───┬───┐
            //     ┌─────────────────┬─────┤mtlA[0]│ 1 │ 0 │      │mtlB[0]│ 1 │ 0 ├──────┐              │mtlA[0]│ 1 │ 0 │
            //     │                 │     └───┬───┴───┴───┘      └───┬───┴───┴───┘      │              └───┬───┴───┴───┘
            //     │                 │         │                      │                  │                  │
            // ┌───▼───┬───┬───┐     │     ┌───▼───┬───┬───┐      ┌───▼───┬───┬───┐  ┌───▼───┬───┬───┐  ┌───▼───┬───┬───┐
            // │mtlA[1]│ 0 │ 1 │     │     │mtlA[2]│ 0 │ 1 │      │mtlB[1]│ 1 │ 0 │  │mtlB[2]│ 1 │ 1 │  │mtlA[1]│ 0 │ 1 │
            // └───┬───┴───┴───┘     │     └───┬───┴───┴───┘      └───┬───┴───┴───┘  └───┬───┴───┴───┘  └───┬───┴───┴───┘
            //     │                 │         │                      │                  │                  │
            //  ┌──▼──┐           ┌──▼──┐   ┌──▼──┐                ┌──▼──┐            ┌──▼──┐            ┌──▼──┐
            //  │meshA│           │meshA│   │meshB│                │meshA│            │meshB│            │meshA│
            //  └─────┘           └─────┘   └─────┘                └─────┘            └─────┘            └─────┘
            Simple
        };

        sol::DescriptorBufferPtr                       descriptorBuffer;
        std::vector<Materials::Graphics>               materials;
        std::vector<sol::GraphicsMaterialInstance2Ptr> materialInstances;
        std::vector<sol::MeshPtr>                      meshes;
        sol::ScenegraphPtr                             scenegraph;
    };


    [[nodiscard]] static Graphics load(Graphics::Name name, sol::GeometryBufferAllocator& allocator);
};
