using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace UILayout
{
    public interface IInputMapping
    {
        bool DoRepeat { get; set; }
        float RepeatDelay { get; set; }

        bool CheckRepeat(InputManager inputManager, float secondsElapsed);

        bool IsDown(InputManager inputManager);
        bool WasPressed(InputManager inputManager);
        bool WasReleased(InputManager inputManager);
    }

    public class InputMappingBase : IInputMapping
    {
        public bool DoRepeat { get; set; }
        public float InitialDelay { get; set; }
        public float RepeatDelay { get; set; }

        bool inInitialRepeat;
        float secondsSinceRepeat { get; set; }

        public InputMappingBase()
        {
            DoRepeat = false;
            InitialDelay = 0.5f;
            RepeatDelay = 0.1f;
        }

        public virtual bool IsDown(InputManager inputManager)
        {
            return false;
        }

        public virtual bool WasPressed(InputManager inputManager)
        {
            return false;
        }

        public virtual bool WasReleased(InputManager inputManager)
        {
            return false;
        }

        public bool CheckRepeat(InputManager inputManager, float secondsElapsed)
        {
            if (!DoRepeat)
                return false;

            if (!IsDown(inputManager))
            {
                secondsSinceRepeat = 0;
                inInitialRepeat = true;

                return false;
            }

            secondsSinceRepeat += secondsElapsed;

            if (inInitialRepeat)
            {
                if (secondsSinceRepeat > InitialDelay)
                {
                    secondsSinceRepeat -= InitialDelay;

                    inInitialRepeat = false;

                    return true;
                }
            }
            else
            {
                if (secondsSinceRepeat > RepeatDelay)
                {
                    secondsSinceRepeat -= RepeatDelay;

                    return true;
                }
            }

            return false;
        }
    }

    public partial class InputManager
    {
        Dictionary<string, List<IInputMapping>> inputMappings = new Dictionary<string, List<IInputMapping>>();
        float secondsElapsed;

        public int MouseWheelDelta { get; private set; }
        public Vector2 MousePosition { get; private set; }

        public void AddMapping(string name, IInputMapping mapping)
        {
            if (!inputMappings.ContainsKey(name))
            {
                inputMappings[name] = new List<IInputMapping>();
            }

            inputMappings[name].Add(mapping);
        }

        public void AddMappingAlias(string alias, string existingName)
        {
            if (inputMappings.ContainsKey(existingName))
            {
                inputMappings[alias] = inputMappings[existingName];
            }
        }

        public bool MappingExists(string name)
        {
            return inputMappings.ContainsKey(name);
        }

        public void SwapMappings(string name1, string name2)
        {
            List<IInputMapping> tmp = inputMappings[name1];
            inputMappings[name1] = inputMappings[name2];
            inputMappings[name2] = tmp;
        }

        public bool IsDown(string name)
        {
            if (inputMappings.ContainsKey(name))
            {
                foreach (IInputMapping mapping in inputMappings[name])
                {
                    if (mapping.IsDown(this))
                        return true;
                }
            }

            return false;
        }

        public bool WasPressed(string name)
        {
            if (inputMappings.ContainsKey(name))
            {
                foreach (IInputMapping mapping in inputMappings[name])
                {
                    if (mapping.WasPressed(this) || mapping.CheckRepeat(this, secondsElapsed))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        public bool WasReleased(string name)
        {
            if (inputMappings.ContainsKey(name))
            {
                foreach (IInputMapping mapping in inputMappings[name])
                {
                    if (mapping.WasReleased(this))
                        return true;
                }
            }

            return false;
        }

        Dictionary<object, string> clickRequests = new Dictionary<object, string>();

        public bool WasClicked(string name, object sender)
        {
            if (sender != null)
            {
                if (WasPressed(name))
                {
                    clickRequests[sender] = name;
                }
                else if (!clickRequests.ContainsKey(sender))
                {
                    return false;
                }
            }

            if (inputMappings.ContainsKey(name))
            {
                foreach (IInputMapping mapping in inputMappings[name])
                {
                    if (mapping.WasReleased(this) || mapping.CheckRepeat(this, secondsElapsed))
                    {
                        if (sender != null)
                        {
                            clickRequests.Remove(sender);
                        }

                        return true;
                    }
                }
            }

            return false;
        }

        public void UpdateUserInterface(Layout userInterface)
        {
            foreach (Touch touch in GetTouches())
            {
                userInterface.HandleTouch(touch);
            }
        }

        public void Update(float secondsElapsed)
        {
            this.secondsElapsed = secondsElapsed;

            PlatformUpdate(secondsElapsed);
        }
    }
}
