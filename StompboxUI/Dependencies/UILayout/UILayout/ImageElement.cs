using System.Drawing;
using System.Numerics;

namespace UILayout
{
    public partial class ImageElement : UIElement
    {
        public UIImage Image { get; set; }
        public UIColor Color { get; set; } = UIColor.White;
        public Rectangle? SourceRectangle { get; set; } = null;

        public ImageElement(string imageName)
            : this(Layout.Current.GetImage(imageName))
        {
        }

        public ImageElement(UIImage image)
        {
            Image = image;
        }

        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            if (Image != null)
            {
                if (SourceRectangle.HasValue)
                {
                    width = SourceRectangle.Value.Width;
                    height = SourceRectangle.Value.Height;
                }
                else
                {
                    width = Image.Width;
                    height = Image.Height;
                }
            }
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            if (SourceRectangle.HasValue)
            {
                Layout.Current.GraphicsContext.DrawImage(Image, SourceRectangle.Value, new RectF(ContentBounds.X, ContentBounds.Y, ContentBounds.Width, ContentBounds.Height), Color);
            }
            else
            {
                Layout.Current.GraphicsContext.DrawImage(Image, new Rectangle(0, 0, Image.Width, Image.Height), new RectF(ContentBounds.X, ContentBounds.Y, ContentBounds.Width, ContentBounds.Height), Color);
            }
        }
    }

    public partial class RotatingImageElement : UIElement
    {
        public UIImage Image { get; set; }
        public UIColor Color { get; set; } = UIColor.White;
        public float Rotation { get; set; } = 0;

        public RotatingImageElement(string imageName)
            : this(Layout.Current.GetImage(imageName))
        {
        }

        public RotatingImageElement(UIImage image)
        {
            Image = image;
        }

        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            if (Image != null)
            {
                width = Image.Width;
                height = Image.Height;
            }
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            Layout.Current.GraphicsContext.DrawImage(Image, ContentBounds.CenterX, ContentBounds.CenterY, Color, Rotation, new Vector2(Image.Width / 2.0f, Image.Height / 2.0f), 1.0f);
        }
    }

    public partial class NinePatchWrapper : UIElementWrapper
    {
        UIImage image;

        public UIImage Image
        {
            get => image;

            set
            {
                image = value;

                Padding = new LayoutPadding((image.Width / 2) - 1, (image.Height / 2) - 1);
                UpdateNintePatch();
            }
        }

        public bool DrawCenter { get; set; } = true;

        public UIColor Color { get; set; } = UIColor.White;

        int[] imageWidths = new int[3];
        int[] imageHeights = new int[3];
        int[] destWidths = new int[3];
        int[] destHeights = new int[3];

        public NinePatchWrapper()
        {

        }

        public NinePatchWrapper(UIImage ninePatchImage)
            : this()
        {
            this.Image = ninePatchImage;
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            UpdateNintePatch();
        }

        void UpdateNintePatch()
        {
            if (Image != null)
            {
                imageWidths[0] = imageWidths[2] = (image.Width / 2) - 1;
                imageWidths[1] = 2;

                imageHeights[0] = imageHeights[2] = (image.Height / 2) - 1;
                imageHeights[1] = 2;

                destWidths[0] = destWidths[2] = imageWidths[0];
                destWidths[1] = (int)(layoutBounds.Width - (destWidths[0] + destWidths[2]));

                destHeights[0] = destHeights[2] = imageHeights[0];
                destHeights[1] = (int)(layoutBounds.Height - (destHeights[0] + destHeights[2]));
            }
        }

        protected override void DrawContents()
        {
            if (Image != null)
            {
                int srcOffsetY = 0;
                int destOffsetY = (int)layoutBounds.Y;

                for (int y = 0; y < 3; y++)
                {
                    int srcOffsetX = 0;
                    float destOffsetX = layoutBounds.X;

                    for (int x = 0; x < 3; x++)
                    {
                        if (DrawCenter || (x != 1) || (y != 1))
                            Layout.Current.GraphicsContext.DrawImage(Image, new System.Drawing.Rectangle(srcOffsetX, srcOffsetY, imageWidths[x], imageHeights[y]), new RectF(destOffsetX, destOffsetY, destWidths[x], destHeights[y]), Color);

                        srcOffsetX += imageWidths[x];
                        destOffsetX += destWidths[x];
                    }

                    srcOffsetY += imageHeights[y];
                    destOffsetY += destHeights[y];
                }
            }

            base.DrawContents();
        }
    }
}