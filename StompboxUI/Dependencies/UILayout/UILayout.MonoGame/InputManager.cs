using System;
using System.Collections.Generic;
using System.Numerics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Input.Touch;

namespace UILayout
{
    public partial class InputManager
    {
        KeyboardState lastState;
        KeyboardState currentState;

        MouseState lastMouseState;
        Vector2 lastMousePosition;

        public static Touch FromXNATouch(TouchLocation touchLocation)
        {
            ETouchState touchState = ETouchState.Invalid;

            switch (touchLocation.State)
            {
                case TouchLocationState.Invalid:
                    touchState = ETouchState.Invalid;
                    break;
                case TouchLocationState.Moved:
                    touchState = ETouchState.Moved;
                    break;
                case TouchLocationState.Pressed:
                    touchState = ETouchState.Pressed;
                    break;
                case TouchLocationState.Released:
                    touchState = ETouchState.Released;
                    break;
            }

            return new Touch { TouchID = touchLocation.Id, Position = new Vector2(touchLocation.Position.X, touchLocation.Position.Y) / (Layout.Current as MonoGameLayout).Scale, TouchState = touchState };
        }

        internal bool IsKeyDown(InputKey key)
        {
            return currentState.IsKeyDown((Keys)key);
        }

        internal bool WasPressed(InputKey key)
        {
            return currentState.IsKeyDown((Keys)key) && !lastState.IsKeyDown((Keys)key);
        }

        internal bool WasReleased(InputKey key)
        {
            return !currentState.IsKeyDown((Keys)key) && lastState.IsKeyDown((Keys)key);
        }

        protected void PlatformUpdate(float secondsElapsed)
        {
            lastState = currentState;
            currentState = Keyboard.GetState();
        }

        public IEnumerable<Touch> GetTouches()
        {
            TouchCollection touches = TouchPanel.GetState();

            //TouchCount = touches.Count;

            //if (TouchCount > 1)
            //{
            //    HandleMultiTouch(userInterface, PixTouch.FromWP7Touch(touches[0]), PixTouch.FromWP7Touch(touches[1]));
            //}

            foreach (TouchLocation touch in touches)
            {
                yield return FromXNATouch(touch);
            }

            MouseState mouseState = Mouse.GetState();

            Vector2 position = new Vector2(mouseState.X, mouseState.Y) / (Layout.Current as MonoGameLayout).Scale;

            MousePosition = position;

            if (mouseState.LeftButton == ButtonState.Pressed)
            {
                if (lastMouseState.LeftButton == ButtonState.Released)
                {
                    yield return new Touch()
                    {
                        Position = position,
                        TouchState = ETouchState.Pressed
                    };
                }
                else
                {
                    if (Vector2.Distance(position, lastMousePosition) == 0)
                    {
                        yield return new Touch()
                        {
                            Position = position,
                            TouchState = ETouchState.Held
                        };
                    }
                    else
                    {
                        yield return new Touch()
                        {
                            Position = position,
                            TouchState = ETouchState.Moved
                        };
                    }
                }
            }
            else
            {
                if (lastMouseState.LeftButton == ButtonState.Pressed)
                {
                    yield return new Touch()
                    {
                        Position = position,
                        TouchState = ETouchState.Released
                    };
                }
            }

            MouseWheelDelta = mouseState.ScrollWheelValue - lastMouseState.ScrollWheelValue;

            lastMouseState = mouseState;
            lastMousePosition = position;
        }
    }
}
