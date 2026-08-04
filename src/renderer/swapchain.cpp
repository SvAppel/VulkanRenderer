#include "swapchain.h"

#include "../logging/logger.h"
#include "image.h"


void Swapchain::build(vk::raii::Device& logicalDevice, vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface, uint32_t width, uint32_t height)
{
    Logger* logger = Logger::get_logger();

    SurfaceDetails support = query_surface_support(physicalDevice, surface);

    format = choose_surface_format(support.formats);
    vk::PresentModeKHR presentMode = choose_present_mode(support.presentModes);
    extent = choose_extent(width, height, support.capabilities);
    imageCount = std::min(support.capabilities.maxImageCount, support.capabilities.minImageCount + 1);

    vk::SwapchainCreateInfoKHR swapChainCreatInfo
    {
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = support.capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = presentMode,
        .clipped = true
    };

    swapChainCreatInfo.oldSwapchain = nullptr;

    chain = vk::raii::SwapchainKHR(logicalDevice, swapChainCreatInfo);

    images = chain.getImages();

    for(uint32_t i = 0; i<images.size(); i++)
    {
        //vk::raii::ImageView imageView = make_image_view(logicalDevice, images[i], format.format);
        imageViews.push_back(make_image_view(logicalDevice, images[i], format.format));
    }

    logger->print("Successfully created swapchain!");
}

void Swapchain::rebuild(vk::raii::Device& logicalDevice, vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface, GLFWwindow* window)
{
    Logger* logger = Logger::get_logger();
    logger->print("Rebuilding the swapchain.");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    while(width == 0 || height == 0)
    {
        logger->print("Window minimized");
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    logger->set_mode(false);

    logicalDevice.waitIdle();
    cleanupSwapchain();

    build(logicalDevice, physicalDevice, surface, width, height);
    logger->set_mode(true);
}

void Swapchain::cleanupSwapchain()
{
    images.clear();
    imageViews.clear();

    chain = nullptr;
}

SurfaceDetails Swapchain::query_surface_support(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface)
{
    Logger* logger = Logger::get_logger();

    SurfaceDetails support;

    support.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    logger->log(support.capabilities);

    support.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    logger->log(support.formats);

    support.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    logger->log(support.presentModes);
    
    return support;
}

vk::Extent2D Swapchain::choose_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)
{
    if(capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    vk::Extent2D extent;

    extent.width = std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, width));
    extent.height = std::min(capabilities.maxImageExtent.height, std::max(capabilities.minImageExtent.height, height));

    return extent;
}

vk::PresentModeKHR Swapchain::choose_present_mode(std::vector<vk::PresentModeKHR> presentModes)
{
    for(vk::PresentModeKHR presentMode: presentModes)
    {
        if(presentMode == vk::PresentModeKHR::eMailbox)
            return presentMode;
    }

    return vk::PresentModeKHR::eFifo;
}

vk::SurfaceFormatKHR Swapchain::choose_surface_format(std::vector<vk::SurfaceFormatKHR> formats)
{
    assert(formats.size() > 0);

    for(vk::SurfaceFormatKHR format: formats)
    {
        if(format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;
    }

    return formats[0];
}