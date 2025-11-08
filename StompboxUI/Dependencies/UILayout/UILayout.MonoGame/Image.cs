using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace UILayout
{
    public partial class UIImage
    {
        public Texture2D Texture { get; private set; }
        public virtual int ActualWidth { get { return Texture.Width; } }
        public virtual int ActualHeight { get { return Texture.Height; } }

        public UIImage(int width, int height)
            : this(new Texture2D(MonoGameLayout.Current.Host.GraphicsDevice, width, height, false, SurfaceFormat.Color))
        {
        }

        public UIImage(Texture2D texture)
        {
            this.Texture = texture;

            Width = Texture.Width;
            Height = Texture.Height;
        }

        public UIImage(Stream stream)
        {
            this.Texture = Texture2D.FromStream(MonoGameLayout.Current.Host.GraphicsDevice, stream);

            Width = Texture.Width;
            Height = Texture.Height;
        }

        public UIImage(UIImage baseImage)
        {
            Texture = baseImage.Texture;
        }

        public UIColor[] GetData()
        {
            UIColor[] tmpData = new UIColor[Width * Height];

            Texture.GetData<UIColor>(0, new Rectangle(XOffset, YOffset, Width, Height), tmpData, 0, Width * Height);

            return tmpData;
        }

        public void SetData(UIColor[] setData)
        {
            Texture.SetData<UIColor>(0, new Rectangle(XOffset, YOffset, Width, Height), setData, 0, setData.Length);
        }
    }
}
