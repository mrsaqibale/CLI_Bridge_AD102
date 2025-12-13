namespace UsbBridgeUi;

static class Program
{
    [STAThread]
    static void Main(string[] args)
    {
        try
        {
            // Check for test mode
            if (args.Length > 0 && args[0] == "--test")
            {
                ApplicationConfiguration.Initialize();
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new TestSimpleForm());
                return;
            }
            
            ApplicationConfiguration.Initialize();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            var form = new MainForm();
            form.Visible = true;
            form.Show();
            Application.Run(form);
        }
        catch (Exception ex)
        {
            MessageBox.Show($"Error starting application: {ex.Message}\n\nStack Trace:\n{ex.StackTrace}", 
                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            Environment.Exit(1);
        }
    }
}

