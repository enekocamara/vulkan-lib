#pragma once
#include "Config.h"

namespace vkl{
    class ComputePipeline{
        public:
            struct CreateInfo{
                vk::Device device;
                vk::ShaderModule shader_module;
                vk::PipelineLayout pipeline_layout;
            };

        ComputePipeline(const CreateInfo& info);
        vk::Pipeline m_pipeline;
    };
}