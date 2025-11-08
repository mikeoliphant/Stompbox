using System.Numerics;
using SkiaSharp;
using SDL2;

namespace UILayout
{
    public class LayoutWindow
    {
        IntPtr window = IntPtr.Zero;
        GRContext glContext;
        SKSurface fbSurface;
        SkiaLayout layout;
        bool needRepaint = true;
        int currentWidth = -1;
        int currentHeight = -1;

        public LayoutWindow(string name, int width, int height)
        {            
            window = SDL.SDL_CreateWindow(name,
                SDL.SDL_WINDOWPOS_CENTERED,
                SDL.SDL_WINDOWPOS_CENTERED,
                width,
                height,
                SDL.SDL_WindowFlags.SDL_WINDOW_RESIZABLE | SDL.SDL_WindowFlags.SDL_WINDOW_OPENGL
            );

            if (window == IntPtr.Zero)
            {
                Console.WriteLine($"There was an issue creating the window. {SDL.SDL_GetError()}");
            }

            IntPtr sdlContext = SDL.SDL_GL_CreateContext(window);

            int result = SDL.SDL_GL_MakeCurrent(window, sdlContext);

            glContext = GRContext.CreateGl();

            // Turn on vsync
            SDL.SDL_GL_SetSwapInterval(1);
        }

        void CreateSurface(int width, int height)
        {
            if (fbSurface != null)
            {
                fbSurface.Dispose();
                fbSurface = null;
            }
            
            GRBackendRenderTarget renderTarget = new GRBackendRenderTarget(
                width: width,
                height: height,
                sampleCount: 0,
                stencilBits: 0,
                glInfo: new GRGlFramebufferInfo(fboId: 0, format: SKColorType.Rgba8888.ToGlSizedFormat()));

            fbSurface = SKSurface.Create(glContext, renderTarget, GRSurfaceOrigin.BottomLeft, SKColorType.Rgba8888);

            currentWidth = width;
            currentHeight = height;
        }

        public void SetLayout(SkiaLayout layout)
        {
            this.layout = layout;

            if (fbSurface != null)
            {
                UpdateLayout();
            }
        }

        void UpdateLayout()
        {
            layout.GraphicsContext.Canvas = fbSurface.Canvas;

            RectF bounds = new RectF(0, 0, currentWidth, currentHeight);

            layout.SetBounds(bounds);
            layout.AddDirtyRect(bounds);
            layout.UpdateLayout();
        }

        public void Run()
        {
            bool running = true;

            while (running)
            {
                while (SDL.SDL_PollEvent(out SDL.SDL_Event e) == 1)
                {
                    switch (e.type)
                    {
                        case SDL.SDL_EventType.SDL_QUIT:
                            running = false;
                            break;

                        case SDL.SDL_EventType.SDL_MOUSEBUTTONDOWN:
                            if (layout != null)
                            {
                                layout.HandleTouch(new Touch()
                                {
                                    Position = new Vector2(e.button.x, e.button.y),
                                    TouchState = ETouchState.Pressed
                                });
                            }
                            break;

                        case SDL.SDL_EventType.SDL_MOUSEBUTTONUP:
                            if (layout != null)
                            {
                                layout.HandleTouch(new Touch()
                                {
                                    Position = new Vector2(e.button.x, e.button.y),
                                    TouchState = ETouchState.Released
                                });
                            }
                            break;

                        case SDL.SDL_EventType.SDL_MOUSEMOTION:
                            if (layout != null)
                            {
                                int x, y;

                                if ((SDL.SDL_GetMouseState(out x, out y) & SDL.SDL_BUTTON_LMASK) != 0)
                                {
                                    layout.HandleTouch(new Touch()
                                    {
                                        Position = new Vector2(e.button.x, e.button.y),
                                        TouchState = ETouchState.Moved
                                    });
                                }
                            }
                            break;

                        case SDL.SDL_EventType.SDL_WINDOWEVENT:
                            switch (e.window.windowEvent)
                            {
                                case SDL.SDL_WindowEventID.SDL_WINDOWEVENT_SIZE_CHANGED:
                                    needRepaint = true;
                                    break;
                            }
                            break;
                    }
                }
                
                if (needRepaint)
                {
                    int width;
                    int height;

                    SDL.SDL_GetWindowSize(window, out width, out height);

                    if ((fbSurface == null) || (width != currentWidth) || (height != currentHeight))
                        CreateSurface(width, height);

                    if (layout != null)
                    {
                        UpdateLayout();
                    }

                    needRepaint = false;
                }

                if (layout != null)
                {
                    RectF bounds = new RectF(0, 0, currentWidth, currentHeight);

                    layout.AddDirtyRect(bounds);

                    if (layout.HaveDirty)
                    {
                        layout.Draw();

                        fbSurface.Canvas.Flush();

                        SDL.SDL_GL_SwapWindow(window);
                    }
                }
            }

            // Clean up
            SDL.SDL_DestroyWindow(window);
            SDL.SDL_Quit();
        }

    }
}
