#include "Renderer.hpp"

int main() 
{
    try 
    {
        Renderer& renderer = Renderer::GetInstance();
        renderer.Init();
        renderer.Run();
    } 
    catch (std::exception const& exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
