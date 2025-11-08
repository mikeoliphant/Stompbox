using System.IO;
using System.Reflection;

namespace UILayout
{
    public abstract class ContentLoader
    {
        public abstract Stream OpenContentStream(string contentPath);
        public abstract UIImage LoadImage(string imageName);
    }

    public class AssemblyResourceContentLoader : ContentLoader
    {
        Assembly resourceAssembly;
        string resourceNamespace;

        public AssemblyResourceContentLoader(Assembly resourceAssembly)
            : this(resourceAssembly, resourceAssembly.GetName().Name)
        {

        }

        public AssemblyResourceContentLoader(Assembly resourceAssembly, string resourceNamespace)
        {
            this.resourceAssembly = resourceAssembly;
            this.resourceNamespace = resourceNamespace;
        }

        public override Stream OpenContentStream(string contentPath)
        {
            return resourceAssembly.GetManifestResourceStream(resourceNamespace + "." + contentPath.Replace('\\', '.'));
        }

        public override UIImage LoadImage(string imageName)
        {
            using (Stream stream = resourceAssembly.GetManifestResourceStream(resourceNamespace + ".Textures." + imageName + ".png"))
            {
                return new UIImage(stream);
            }
        }
    }
}
