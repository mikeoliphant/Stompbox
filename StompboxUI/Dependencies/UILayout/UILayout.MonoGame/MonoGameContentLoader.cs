using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using System.IO;

namespace UILayout
{
    public class MonoGameContentLoader : ContentLoader
    {
        ContentManager contentManager;

        public MonoGameContentLoader(ContentManager contentManager)
        {
            this.contentManager = contentManager;
        }

        public override Stream OpenContentStream(string contentPath)
        {
#if WINDOWS
            return AssemblyRelativeContentManager.OpenAseemblyRelativeStream(Path.Combine(contentManager.RootDirectory, contentPath));
#else
            return TitleContainer.OpenStream(Path.Combine(contentManager.RootDirectory, contentPath));
#endif
        }

        public override UIImage LoadImage(string imageName)
        {
            return new UIImage(contentManager.Load<Texture2D>(Path.Combine("Textures", imageName)));
        }
    }
}
