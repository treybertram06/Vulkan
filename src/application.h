#pragma once

#include "vk_window.h"
#include "vk_pipeline.h"
#include "vk_device.h"

namespace VKEngine {
    class Application {
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
        Window m_window {WIDTH, HEIGHT, "Vulkan window"};
        Device m_device {m_window};
        Pipeline m_pipeline {
            m_device,
            "../shaders/shader.vert.spv",
            "../shaders/shader.frag.spv",
            Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}
