using System;
using System.Collections.Generic;

namespace UILayout
{
    public class DialogInput
    {
        public bool WaitForRelease { get; set; }
        public string Text { get; set; }
        public Action Action { get; set; }
        public bool CloseOnInput { get; set; }

        public DialogInput()
        {
            CloseOnInput = false;
            WaitForRelease = true;
        }
    }

    public class InputDialog : NinePatchWrapper, IPopup
    {
        public static float DefaultButtonSpacing = 2;

        public Action CloseAction { get { return inputStack.CloseAction; } set { inputStack.CloseAction = value; } }

        public float ButtonSpacing
        {
            get { return inputStack.InputContainer.ChildSpacing; }
            set { inputStack.InputContainer.ChildSpacing = value; }
        }

        public EHorizontalAlignment ButtonAlignment
        {
            get { return inputStack.HorizontalAlignment; }
            set { inputStack.HorizontalAlignment = value; }
        }

        VerticalStack stack;
        UIElement contents;
        UIElementWrapper wrapper;
        protected DialogInputStack inputStack;

        public InputDialog(UIImage ninePatchImage)
            : this(ninePatchImage, null, null)
        {
        }

        public InputDialog(UIImage ninePatchImage, UIElement contents)
            : this(ninePatchImage, contents, null)
        {
        }

        public InputDialog(UIImage ninePatchImage, UIElement contents, params DialogInput[] inputs)
            : base(ninePatchImage)
        {
            this.contents = contents;

            HorizontalAlignment = EHorizontalAlignment.Center;
            VerticalAlignment = EVerticalAlignment.Center;

            stack = new VerticalStack();
            stack.HorizontalAlignment = EHorizontalAlignment.Stretch;
            stack.VerticalAlignment = EVerticalAlignment.Stretch;

            stack.ChildSpacing = 5;
            Child = stack;

            wrapper = new UIElementWrapper();
            wrapper.HorizontalAlignment = EHorizontalAlignment.Stretch;
            wrapper.VerticalAlignment = EVerticalAlignment.Stretch;
            stack.Children.Add(wrapper);

            wrapper.Child = contents;

            inputStack = new DialogInputStack(this, new HorizontalStack { HorizontalAlignment = EHorizontalAlignment.Center, ChildSpacing = DefaultButtonSpacing }, inputs) { HorizontalAlignment = EHorizontalAlignment.Center, VerticalAlignment = EVerticalAlignment.Bottom };
            stack.Children.Add(inputStack);
        }

        public virtual void Opened()
        {
        }

        public void SetContents(UIElement contents)
        {
            wrapper.Child = contents;
        }

        public void AddInput(DialogInput input)
        {
            inputStack.AddInput(input);
        }

        public virtual void Exit()
        {
            if (CloseAction != null)
            {
                CloseAction();
            }
        }
    }

    public class DialogInputStack : UIElementWrapper
    {
        public Action CloseAction { get; set; }

        public ListUIElement InputContainer { get; private set; }

        IPopup hostElement;
        List<DialogInput> inputs = new List<DialogInput>();
        int selectedInput = 0;

        public DialogInputStack(IPopup hostElement, ListUIElement inputContainer, params DialogInput[] inputs)
        {
            this.hostElement = hostElement;
            this.InputContainer = inputContainer;

            Child = inputContainer;

            if (inputs != null)
            {
                foreach (DialogInput input in inputs)
                {
                    AddInput(input);
                }
            }
        }

        public void AddInput(DialogInput input)
        {
            inputs.Add(input);

            TextButton button = new TextButton(input.Text);

            Action action = delegate { DoAction(input); };

            if (input.WaitForRelease)
                button.ClickAction = action;
            else
                button.PressAction = action;

            InputContainer.Children.Add(button);

            SelectInput(0);
        }

        public void SelectInput(int pos)
        {
            selectedInput = pos;
        }

        bool DoAction(DialogInput input)
        {
            bool handled = false;

            if (input.CloseOnInput)
            {
                if ((hostElement != null) && (hostElement.CloseAction != null))
                    hostElement.CloseAction();

                handled = true;
            }

            if (input.Action != null)
            {
                input.Action();

                handled = true;
            }

            return handled;
        }
    }
}
