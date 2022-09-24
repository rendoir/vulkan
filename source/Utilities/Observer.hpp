#pragma once

class SwapchainObserver
{
public:
    virtual void OnSwapchainRecreated(VkExtent2D const& newExtent) = 0;
};
