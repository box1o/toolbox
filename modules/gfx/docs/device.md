## To Create a device call the static method on the Device class it wil return a exception (result)  and to acces the device call .value or returned result 

``` cpp

auto res = gfx::Device::Create({
    .backend = gfx::DeviceBackend::Vulkan
    //TODO: Other selection settings 
}); 
if (!res) {
    log::Error("Failed to create device: {}", res.error().Message());
    return -1;
}
auto device = res.value();


// Functions to fetch resources like queue and more 

// Functions to acces native resources like VKDevice VkInstance ... return a void * reinterpret cast the memory to the corect return class you need , make shure the code is safe 


```
