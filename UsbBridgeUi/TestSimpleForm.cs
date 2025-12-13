using System;
using System.Windows.Forms;

namespace UsbBridgeUi;

// Simple test form to verify UI works
public class TestSimpleForm : Form
{
    public TestSimpleForm()
    {
        Text = "Test - USB Bridge UI";
        Size = new System.Drawing.Size(400, 300);
        StartPosition = FormStartPosition.CenterScreen;
        
        var label = new Label
        {
            Text = "If you see this, the UI is working!",
            Location = new System.Drawing.Point(50, 50),
            Size = new System.Drawing.Size(300, 50),
            Font = new System.Drawing.Font("Arial", 12)
        };
        
        var button = new Button
        {
            Text = "Close",
            Location = new System.Drawing.Point(150, 150),
            Size = new System.Drawing.Size(100, 30)
        };
        button.Click += (s, e) => Close();
        
        Controls.Add(label);
        Controls.Add(button);
    }
}

