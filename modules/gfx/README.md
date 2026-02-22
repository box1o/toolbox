# GFX Code Index

This is the full code index for `modules/gfx`, grouped by layer.

## 1) Build + Module Entry

- `modules/gfx/CMakeLists.txt`: module build, backend selection (`USE_WEBGPU`), dependencies, compiler/link options.
- `modules/gfx/include/toolbox/gfx/gfx.hpp`: umbrella include for all public gfx APIs.
- `modules/gfx/include/toolbox/gfx/common.hpp`: shared/common gfx include surface.

## 2) Public API Headers (`include/toolbox/gfx/api`)

- `modules/gfx/include/toolbox/gfx/api/types.hpp`: backend, power preference, queue type enums.
- `modules/gfx/include/toolbox/gfx/api/device.hpp`: `Device` interface + adapter info, limits, native handles.
- `modules/gfx/include/toolbox/gfx/api/surface.hpp`: `Surface` interface.
- `modules/gfx/include/toolbox/gfx/api/swapchain.hpp`: `Swapchain`, `Frame`, present mode, swapchain config.
- `modules/gfx/include/toolbox/gfx/api/queue.hpp`: `Queue` interface and submit API.
- `modules/gfx/include/toolbox/gfx/api/command_buffer.hpp`: command buffer abstraction.
- `modules/gfx/include/toolbox/gfx/api/command_encoder.hpp`: command encoder abstraction + render pass begin.
- `modules/gfx/include/toolbox/gfx/api/render_pass.hpp`: render pass descriptors and draw calls.
- `modules/gfx/include/toolbox/gfx/api/shader.hpp`: shader module API (WGSL/file).
- `modules/gfx/include/toolbox/gfx/api/vertex_layout.hpp`: vertex binding/attribute layout description.
- `modules/gfx/include/toolbox/gfx/api/pipeline.hpp`: render pipeline/raster state.
- `modules/gfx/include/toolbox/gfx/api/buffer.hpp`: GPU buffer API + usage flags.
- `modules/gfx/include/toolbox/gfx/api/texture.hpp`: texture format + index format enums.
- `modules/gfx/include/toolbox/gfx/api/texture_resource.hpp`: `Texture`, `TextureView`, texture descriptors.
- `modules/gfx/include/toolbox/gfx/api/sampler.hpp`: sampler API and filtering/addressing modes.
- `modules/gfx/include/toolbox/gfx/api/bindings.hpp`: bind group layout, bind groups, pipeline layout.
- `modules/gfx/include/toolbox/gfx/api/frame_graph.hpp`: frame graph API (`FrameGraph`, `CompiledFrameGraph`).

## 3) Public Window API (`include/toolbox/gfx/window`)

- `modules/gfx/include/toolbox/gfx/window/window.hpp`: `Window` abstraction on GLFW, resize callbacks, cursor controls.

## 4) API Frontend Implementations (`src/api`)

- `modules/gfx/src/api/device.cpp`: `Device::Create` frontend dispatcher.
- `modules/gfx/src/api/surface.cpp`: `Surface::Create` dispatcher.
- `modules/gfx/src/api/swapchain.cpp`: `Swapchain::Create` dispatcher.
- `modules/gfx/src/api/queue.cpp`: `Queue::Create` dispatcher.
- `modules/gfx/src/api/command_encoder.cpp`: `CommandEncoder::Create` dispatcher.
- `modules/gfx/src/api/shader.cpp`: `ShaderModule::FromWGSL/FromFile` dispatcher.
- `modules/gfx/src/api/pipeline.cpp`: `RenderPipeline::Create` dispatcher.
- `modules/gfx/src/api/buffer.cpp`: `Buffer::Create` dispatcher.
- `modules/gfx/src/api/texture.cpp`: texture creation and texture-from-file dispatcher.
- `modules/gfx/src/api/sampler.cpp`: `Sampler::Create` dispatcher.
- `modules/gfx/src/api/bindings.cpp`: bind group layout/group/pipeline layout dispatchers.
- `modules/gfx/src/api/frame_graph.cpp`: frame graph compile/record logic and pass execution.

## 5) Window Implementation (`src/window`)

- `modules/gfx/src/window/window.cpp`: GLFW lifecycle, callbacks, native handle, resize/content-scale handling, cursor modes.

## 6) WebGPU Backend (`backend/webgpu`)

### Core Device + Surface + Swapchain
- `modules/gfx/backend/webgpu/device_impl.hpp`
- `modules/gfx/backend/webgpu/device_impl.cpp`
- `modules/gfx/backend/webgpu/surface_impl.hpp`
- `modules/gfx/backend/webgpu/surface_impl.cpp`
- `modules/gfx/backend/webgpu/native_surface.cpp`
- `modules/gfx/backend/webgpu/swapchain_impl.hpp`
- `modules/gfx/backend/webgpu/swapchain_impl.cpp`
- `modules/gfx/backend/webgpu/conversion.hpp`

### Command Submission
- `modules/gfx/backend/webgpu/queue_impl.hpp`
- `modules/gfx/backend/webgpu/queue_impl.cpp`
- `modules/gfx/backend/webgpu/command_impl.hpp`
- `modules/gfx/backend/webgpu/command_impl.cpp`

### Resources
- `modules/gfx/backend/webgpu/buffer_impl.hpp`
- `modules/gfx/backend/webgpu/buffer_impl.cpp`
- `modules/gfx/backend/webgpu/texture_impl.hpp`
- `modules/gfx/backend/webgpu/texture_impl.cpp`
- `modules/gfx/backend/webgpu/sampler_impl.hpp`
- `modules/gfx/backend/webgpu/sampler_impl.cpp`

### Shader + Pipeline + Bindings
- `modules/gfx/backend/webgpu/shader_impl.hpp`
- `modules/gfx/backend/webgpu/shader_impl.cpp`
- `modules/gfx/backend/webgpu/pipeline_impl.hpp`
- `modules/gfx/backend/webgpu/pipeline_impl.cpp`
- `modules/gfx/backend/webgpu/bindings_impl.hpp`
- `modules/gfx/backend/webgpu/bindings_impl.cpp`

## 7) Quick Navigation Tips

- Start at `modules/gfx/include/toolbox/gfx/gfx.hpp` to see the full API surface.
- For creation flow, follow: API header -> `src/api/*.cpp` -> `backend/webgpu/*_impl.*`.
- For rendering issues, inspect in this order:
  1. `modules/gfx/backend/webgpu/swapchain_impl.cpp`
  2. `modules/gfx/backend/webgpu/command_impl.cpp`
  3. `modules/gfx/backend/webgpu/pipeline_impl.cpp`
  4. `modules/gfx/backend/webgpu/shader_impl.cpp`
