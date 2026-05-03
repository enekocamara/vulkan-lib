#include "ComputePipeline.hpp"

namespace vkl{
    ComputePipeline::ComputePipeline(const CreateInfo &info)
    {
        vk::PipelineShaderStageCreateInfo stage_info{};
        stage_info.stage = vk::ShaderStageFlagBits::eCompute;
        stage_info.module = info.shader_module;
        stage_info.pName = "main";

        vk::ComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.stage = stage_info;
        pipelineInfo.layout = info.pipeline_layout;

        auto pipeline = info.device.createComputePipeline(nullptr, pipelineInfo);
        if (pipeline.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create compute pipeline");
        m_pipeline = pipeline.value;
    }
}