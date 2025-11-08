using Microsoft.Xna.Framework;

namespace UILayout
{
    public partial struct UIColor
    {
        public Color NativeColor
        {
            get
            {
                float value = (float)A / 255.0f;

                return new Microsoft.Xna.Framework.Color((byte)(((float)R * value)), (byte)(((float)G * value)), (byte)(((float)B * value)), A);
            }
        }
    }
}
