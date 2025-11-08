using System.IO;
using System.Xml.Serialization;
using UILayout;

namespace UILayout.DefaultTextures
{
    public class TextureLoader
    {
        public static void LoadDefaultTextures(ContentLoader loader)
        {
            Layout.Current.AddImage(loader, "ScrollBar");
            Layout.Current.AddImage(loader, "ScrollBarGutter");
            Layout.Current.AddImage(loader, "ScrollUpArrow");
            Layout.Current.AddImage(loader, "ScrollDownArrow");

            Layout.Current.AddImage(loader, "TabPanelBackground");
            Layout.Current.AddImage(loader, "TabBackground");
            Layout.Current.AddImage(loader, "TabForeground");

            Layout.Current.DefaultOutlineNinePatch = Layout.Current.AddImage(loader, "OutlineNinePatch");

            Layout.Current.DefaultPressedNinePatch = Layout.Current.AddImage(loader, "ButtonPressed");
            Layout.Current.DefaultUnpressedNinePatch = Layout.Current.AddImage(loader, "ButtonUnpressed");

            Layout.Current.DefaultDragImage = Layout.Current.AddImage(loader, "ButtonUnpressed");

            Layout.Current.DefaultForegroundColor = UIColor.White;

            using (Stream fontStream = loader.OpenContentStream("Textures.Font.xml"))
            {
                XmlSerializer serializer = new XmlSerializer(typeof(SpriteFontDefinition));

                SpriteFontDefinition fontDef = serializer.Deserialize(fontStream) as SpriteFontDefinition;

                Layout.Current.AddImage(loader, fontDef.Name);

                UIFont font = UIFont.FromSpriteFont(fontDef);

                if (font != null)
                {
                    Layout.Current.DefaultFont = Layout.Current.AddFont(fontDef.Name,  font);
                }
            }
        }
    }
}
