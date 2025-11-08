using System;
using System.Numerics;

namespace UILayout
{
    public struct RectF : IEquatable<RectF>
    {
        private float x;
        private float y;
        private float width;
        private float height;

        public float X { get => x; set => x = value; }
        public float Y { get => y; set => y = value; }
        public float Width { get => width; set => width = value; }
        public float Height { get => height; set => height = value; }
        public float Top { get { return y; } }
        public float Bottom { get { return y + height; } }
        public float Left { get { return x; } }
        public float Right { get { return x + width; } }
        public bool IsEmpty { get { return (width == 0) || (height == 0); } }
        public float CenterX { get { return X + (Width / 2); } }
        public float CenterY { get { return Y + (Height / 2); } }
        public Vector2 Center
        {
            get
            {
                return new Vector2(X + (Width / 2), Y + (Height / 2));
            }
        }

        public static RectF Empty { get { return new RectF(0, 0, 0, 0); } }

        public RectF(float x, float y, float width, float height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }

        public bool Equals(RectF other)
        {
            return (this.x == ((RectF)other).x) && (this.y == ((RectF)other).y) && (this.width == ((RectF)other).width) && (this.height == ((RectF)other).height);
        }

        public void MakeEmpty()
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }

        public void Copy(in RectF other)
        {
            x = other.x;
            y = other.y;
            width = other.width;
            height = other.height;
        }

        public bool Contains(float x, float y)
        {
            return (x >= this.x) && (y >= this.y) && (x < Right) && (y < Bottom);
        }

        public bool Contains(in Vector2 point)
        {
            return (point.X >= x) && (point.Y >= y) && (point.X < Right) && (point.Y < Bottom);
        }

        public bool Intersects(in RectF other)
        {
            return (other.x < Right) &&
               (x < other.Right) &&
               (other.y < Bottom) &&
               (y < other.Bottom);
        }

        public void UnionWith(in RectF other)
        {
            float maxX = Math.Max(Right, other.Right);
            float maxY = Math.Max(Bottom, other.Bottom);

            x = Math.Min(x, other.x);
            y = Math.Min(y, other.y);
            width = maxX - x;
            height = maxY - y;
        }

        public void IntersectWith(in RectF other)
        {
            if (Intersects(in other))
            {
                float right = Math.Min(x + width, other.x + other.width);
                float left = Math.Max(x, other.x);
                float top = Math.Max(y, other.y);
                float bottom = Math.Min(y + height, other.y + other.height);

                x = left;
                y = top;
                width = right - left;
                height = bottom - top;
            }
            else
            {
                MakeEmpty();
            }
        }

        public override string ToString()
        {
            return x + "," + y + " (" + width + "x" + height + ")";
        }
    }
}
