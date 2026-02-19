
#include "../../include/toolbox/gfx/api/queue.hpp"
#include "../../include/toolbox/gfx/api/device.hpp"
#include "toolbox/base/logger/logger.hpp"

namespace ct::gfx {

result<ref<Queue>> Queue::Create(ref<Device> device, const QueueDesc& desc) noexcept {
    log::Critical("Queue creation failed: no graphics backend available");
    return {};
}

} // namespace ct::gfx
