using System.Numerics;

namespace UILayout
{
    public enum ETouchState
    {
        Pressed,
        Moved,
        Held,
        Released,
        Invalid
    }

    public struct Touch
    {
        public Vector2 Position;
        public ETouchState TouchState;
        public int TouchID;
    }
}
