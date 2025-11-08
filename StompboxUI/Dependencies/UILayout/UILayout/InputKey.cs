using System;
using System.Collections.Generic;
using System.Text;

namespace UILayout
{
    public enum InputKey
    {
        None = 0,
        Back = 8,
        Tab = 9,
        Enter = 13,
        Pause = 19,
        CapsLock = 20,
        Escape = 27,
        Space = 32,
        PageUp = 33,
        PageDown = 34,
        End = 35,
        Home = 36,
        Left = 37,
        Up = 38,
        Right = 39,
        Down = 40,
        Insert = 45,
        Delete = 46,
        D0 = 48,
        D1 = 49,
        D2 = 50,
        D3 = 51,
        D4 = 52,
        D5 = 53,
        D6 = 54,
        D7 = 55,
        D8 = 56,
        D9 = 57,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        NumPad0 = 96,
        NumPad1 = 97,
        NumPad2 = 98,
        NumPad3 = 99,
        NumPad4 = 100,
        NumPad5 = 101,
        NumPad6 = 102,
        NumPad7 = 103,
        NumPad8 = 104,
        NumPad9 = 105,
        Multiply = 106,
        Add = 107,
        Separator = 108,
        Subtract = 109,
        Decimal = 110,
        Divide = 111,
        F1 = 112,
        F2 = 113,
        F3 = 114,
        F4 = 115,
        F5 = 116,
        F6 = 117,
        F7 = 118,
        F8 = 119,
        F9 = 120,
        F10 = 121,
        F11 = 122,
        F12 = 123,
        NumLock = 144,
        Scroll = 145,
        LeftShift = 160,
        RightShift = 161,
        LeftControl = 162,
        RightControl = 163,
        LeftAlt = 164,
        RightAlt = 165,
        OemSemicolon = 186,
        OemPlus = 187,
        OemComma = 188,
        OemMinus = 189,
        OemPeriod = 190,
        OemQuestion = 191,
        OemTilde = 192,
        OemOpenBrackets = 219,
        OemPipe = 220,
        OemCloseBrackets = 221,
        OemQuotes = 222,
        OemBackslash = 226
    }

    public class KeyMapping : InputMappingBase
    {
        public InputKey Modifier { get; set; }

        InputKey[] keys;

        public override bool IsDown(InputManager inputManager)
        {
            if (Modifier != InputKey.None)
            {
                if (!inputManager.IsKeyDown(Modifier))
                    return false;
            }

            for (int i = 0; i < keys.Length; i++)
            {
                if (inputManager.IsKeyDown(keys[i]))
                    return true;
            }

            return false;
        }

        public override bool WasPressed(InputManager inputManager)
        {
            if (Modifier != InputKey.None)
            {
                if (!inputManager.IsKeyDown(Modifier))
                    return false;
            }

            for (int i = 0; i < keys.Length; i++)
            {
                if (inputManager.WasPressed(keys[i]))
                    return true;
            }

            return false;
        }

        public override bool WasReleased(InputManager inputManager)
        {
            if (Modifier != InputKey.None)
            {
                if (!inputManager.IsKeyDown(Modifier))
                    return false;
            }

            for (int i = 0; i < keys.Length; i++)
            {
                if (inputManager.WasReleased(keys[i]))
                    return true;
            }

            return false;
        }

        public KeyMapping(params InputKey[] keys)
        {
            this.keys = keys;

            Modifier = InputKey.None;
        }

        public override string ToString()
        {
            string toStr = "Key Mapping: ";

            foreach (InputKey k in keys)
            {
                toStr += k.ToString() + ";";
            }

            return toStr;
        }
    }
}
