#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>

/**
 * @brief Handy bundle of info describing what a surface can do
 */
struct SurfaceDetails
{
    /**
     * @brief No. of images and supported sizes
     */
    vk::SurfaceCapabilitiesKHR capabilities;

    /**
     * @brief Supported pixel formats
     */
    std::vector<vk::SurfaceFormatKHR> formats;
    
    /**
     * @brief Available presentation modes
     */
    std::vector<vk::PresentModeKHR> presentModes;

};

/**
 * @brief A swapchain
 */
class Swapchain
{
public:

    /**
     * @brief Construct a new Swapchain object
     * 
     * @param logicalDevice: Vulkan device
     * @param physicalDevice: Physical device
     * @param surface: The window surface to present to
     * @param width: Requested swapchain width
     * @param height: Requested swapchain height
     */
    void build(
        vk::raii::Device& logicalDevice,
        vk::raii::PhysicalDevice physicalDevice,
        vk::raii::SurfaceKHR& surface,
        uint32_t width, uint32_t height
    );

    /**
     * @brief The number of images
     */
    uint32_t imageCount;

    /**
     * @brief The underlying swapchain resource
     */
    vk::raii::SwapchainKHR chain = nullptr;

    /**
     * @brief Image format
     */
    vk::SurfaceFormatKHR format;

    /**
     * @brief Image size
     */
    vk::Extent2D extent;

    /**
     * @brief Swapchain images to render to
     */
    std::vector<vk::Image> images;

    /**
     * @brief View of the swapchain images
     */
    std::vector<vk::raii::ImageView> imageViews;

private:

    /**
     * @brief Check the properties of a surface
     * 
     * @param physicalDevice: The physical device
     * @param surface: Window surface
     * 
     * @return SurfaceDetails: The support details
     */
    SurfaceDetails query_surface_support(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface);

    /**
     * @brief Chooe an extent, working within the give constraints
     * 
     * @param width: Requested width
     * @param height: Requested height
     * @param capabilities: Surface capability support
     * 
     * @return vk::Extend2D: The chosen extent 
     */
    vk::Extent2D choose_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities);

    /**
     * @brief Choose a present mode
     * 
     * @param presentModes: Available present modes
     * 
     * @return vk::PresentModeKHR: The chosen present mode
     */
    vk::PresentModeKHR choose_present_mode(std::vector<vk::PresentModeKHR> presentModes);

    /**
     * @brief Choose a surface format
     * 
     * @param formats: Supported formats to choose from
     * 
     * @return vk::SurfaceFormatKHR: The chosen format
     */
    vk::SurfaceFormatKHR choose_surface_format(std::vector<vk::SurfaceFormatKHR> formats);
};