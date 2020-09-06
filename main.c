#include <SFML/Graphics.h>

#include "global.h"
#include "complex.h"

int main()
{
    cxRun();

    //sfContextSettings context_settings;
    //context_settings.antialiasingLevel = 4;
    //context_settings.depthBits = 0;
    //context_settings.stencilBits = 0;
    //context_settings.majorVersion = 4;
    //context_settings.minorVersion = 5;

    //sfVideoMode video_mode;
    //video_mode.width = 700;
    //video_mode.height = 500;

    //sfRenderWindow* window = sfRenderWindow_create(video_mode, "Hello", 4, &context_settings);

    //while (sfRenderWindow_isOpen(window))
    //{
    //    sfEvent event;
    //    while (sfRenderWindow_pollEvent(window, &event))
    //    {
    //        if (event.type == sfEvtClosed)
    //            sfRenderWindow_close(window);
    //    }

    //    sfRenderWindow_clear(window, sfColor_fromRGB(200, 20, 20));
    //    sfRenderWindow_display(window);
    //}
}