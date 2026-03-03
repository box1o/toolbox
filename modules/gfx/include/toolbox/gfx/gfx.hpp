#pragma once

// IWYU pragma: begin_exports
#include <toolbox/gfx/window/window.hpp>

// NOTE: API (RHI)
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/queue.hpp>
#include <toolbox/gfx/api/swapchain.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/gfx/api/sampler.hpp>
#include <toolbox/gfx/api/shader.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>
#include <toolbox/gfx/api/bind_group_layout.hpp>
#include <toolbox/gfx/api/bind_group.hpp>
#include <toolbox/gfx/api/pipeline.hpp>
#include <toolbox/gfx/api/render_pass.hpp>
#include <toolbox/gfx/api/command_encoder.hpp>
#include <toolbox/gfx/api/command_buffer.hpp>

// NOTE: Renderer block
#include "toolbox/gfx/renderer/renderer.hpp"


// NOTE: Events
#include "toolbox/gfx/events/application/events.hpp"
#include "toolbox/gfx/events/input/events.hpp"
#include "toolbox/gfx/events/window/events.hpp"
#include "toolbox/gfx/events/renderer/events.hpp"
#include "toolbox/gfx/events/base.hpp"
#include "toolbox/gfx/events/format.hpp"
#include "toolbox/gfx/events/category.hpp"
#include "toolbox/gfx/events/dispatcher.hpp"

// IWYU pragma: end_exports
