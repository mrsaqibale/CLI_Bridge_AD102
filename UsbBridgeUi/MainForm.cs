using UsbDeviceCore;
using System.IO;

namespace UsbBridgeUi;

public partial class MainForm : Form
{
    private readonly DeviceService _deviceService;
    private ListBox _logListBox = null!;
    private TextBox _logTextBox = null!;
    private Panel _statusPanel = null!;
    private ComboBox _deviceTypeCombo = null!;
    private TextBox _lineTextBox = null!;
    private TextBox _dialNumberTextBox = null!;
    private Label _connectionStatusLabel = null!;
    private Button _copyLogsButton = null!;

    public MainForm()
    {
        try
        {
            _deviceService = new DeviceService();
            _deviceService.OnDeviceEvent += DeviceService_OnDeviceEvent;

            InitializeComponent();
            SetupControls();
            UpdateConnectionStatus(false);
            
            // Now we can use AddLog since InitializeComponent has been called
            AddLog("=== USB Bridge UI Starting ===");
            AddLog("Initializing DeviceService...");
            AddLog("DeviceService initialized successfully");
            
            // Test DLL availability
            AddLog("Checking DLL availability...");
            TestDllAvailability();
            
            // Ensure form is visible
            this.Visible = true;
            this.WindowState = FormWindowState.Normal;
            this.ShowInTaskbar = true;
            
            AddLog("=== USB Bridge UI Ready ===");
        }
        catch (Exception ex)
        {
            var errorMsg = $"Error initializing form: {ex.Message}\n\n{ex.StackTrace}";
            // Try to log, but if controls aren't initialized, just show message box
            try
            {
                AddLog($"FATAL ERROR: {errorMsg}", true);
            }
            catch
            {
                // Ignore if AddLog fails
            }
            MessageBox.Show(errorMsg, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            throw;
        }
    }

    private void TestDllAvailability()
    {
        var dlls = new[] { "USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll" };
        var exePath = Path.GetDirectoryName(Application.ExecutablePath) ?? Environment.CurrentDirectory;
        
        foreach (var dll in dlls)
        {
            var dllPath = Path.Combine(exePath, dll);
            if (File.Exists(dllPath))
            {
                var fileInfo = new FileInfo(dllPath);
                AddLog($"✓ DLL Found: {dll} | Size: {fileInfo.Length} bytes | Path: {dllPath}");
            }
            else
            {
                AddLog($"✗ DLL Missing: {dll} | Expected at: {dllPath}", true);
            }
        }
        
        // Test if we can get device count (tests DLL loading)
        try
        {
            var count = _deviceService.GetDeviceCount();
            AddLog($"DLL Load Test: GetDeviceCount() = {count}");
        }
        catch (Exception ex)
        {
            AddLog($"DLL Load Test Failed: {ex.Message}", true);
        }
    }

    private void InitializeComponent()
    {
        Text = "USB Caller ID Test UI - Developed by Saqib Ali (mrsaqibale) AD102 Bridge App";
        Size = new Size(900, 700);
        MinimumSize = new Size(900, 700);
        StartPosition = FormStartPosition.CenterScreen;

        _deviceTypeCombo = new ComboBox
        {
            Location = new Point(10, 10),
            Size = new Size(150, 25),
            DropDownStyle = ComboBoxStyle.DropDownList
        };
        _deviceTypeCombo.Items.AddRange(new[] { "F1 (1 ch)", "F2 (2 ch)", "F4 (4 ch)", "F8 (8 ch)", "EAR" });
        _deviceTypeCombo.SelectedIndex = 1; // Default F2

        var enableButton = new Button
        {
            Text = "Enable Device",
            Location = new Point(170, 10),
            Size = new Size(120, 30)
        };
        enableButton.Click += EnableButton_Click;

        var disableButton = new Button
        {
            Text = "Disable Device",
            Location = new Point(300, 10),
            Size = new Size(120, 30)
        };
        disableButton.Click += DisableButton_Click;

        _connectionStatusLabel = new Label
        {
            Text = "Status: Disconnected",
            Location = new Point(430, 15),
            Size = new Size(200, 20),
            ForeColor = Color.Red
        };

        var lineLabel = new Label { Text = "Line:", Location = new Point(10, 50), Size = new Size(50, 20) };
        _lineTextBox = new TextBox { Location = new Point(60, 48), Size = new Size(50, 25), Text = "0" };

        var dialLabel = new Label { Text = "Dial:", Location = new Point(120, 50), Size = new Size(50, 20) };
        _dialNumberTextBox = new TextBox { Location = new Point(170, 48), Size = new Size(150, 25) };

        var pickupButton = new Button
        {
            Text = "Pickup",
            Location = new Point(330, 47),
            Size = new Size(80, 30)
        };
        pickupButton.Click += PickupButton_Click;

        var hookOnButton = new Button
        {
            Text = "Hook On",
            Location = new Point(420, 47),
            Size = new Size(80, 30)
        };
        hookOnButton.Click += HookOnButton_Click;

        var dialButton = new Button
        {
            Text = "Dial",
            Location = new Point(510, 47),
            Size = new Size(80, 30)
        };
        dialButton.Click += DialButton_Click;

        var titleLabel = new Label
        {
            Text = "USB Caller ID Test UI",
            Location = new Point(10, 90),
            Size = new Size(300, 25),
            Font = new Font(Font.FontFamily, 12, FontStyle.Bold),
            ForeColor = Color.DarkBlue
        };

        var developerLabel = new Label
        {
            Text = "Developed by Saqib Ali (mrsaqibale) - AD102 Bridge App",
            Location = new Point(320, 92),
            Size = new Size(550, 20),
            Font = new Font(Font.FontFamily, 8, FontStyle.Italic),
            ForeColor = Color.Gray
        };

        var statusLabel = new Label
        {
            Text = "Line Status:",
            Location = new Point(10, 120),
            Size = new Size(100, 20),
            Font = new Font(Font.FontFamily, 9, FontStyle.Bold)
        };

        _statusPanel = new Panel
        {
            Location = new Point(10, 145),
            Size = new Size(870, 200),
            BorderStyle = BorderStyle.FixedSingle,
            AutoScroll = true
        };

        var logLabel = new Label
        {
            Text = "Event Log:",
            Location = new Point(10, 355),
            Size = new Size(100, 20),
            Font = new Font(Font.FontFamily, 9, FontStyle.Bold)
        };

        _logListBox = new ListBox
        {
            Location = new Point(10, 380),
            Size = new Size(870, 170),
            Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right
        };

        _logTextBox = new TextBox
        {
            Location = new Point(10, 560),
            Size = new Size(790, 100),
            Multiline = true,
            ScrollBars = ScrollBars.Vertical,
            ReadOnly = true,
            Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top,
            Font = new Font("Consolas", 9)
        };

        _copyLogsButton = new Button
        {
            Text = "Copy Logs",
            Location = new Point(810, 560),
            Size = new Size(70, 30),
            Anchor = AnchorStyles.Bottom | AnchorStyles.Right
        };
        _copyLogsButton.Click += CopyLogsButton_Click;

        var clearLogsButton = new Button
        {
            Text = "Clear",
            Location = new Point(810, 600),
            Size = new Size(70, 30),
            Anchor = AnchorStyles.Bottom | AnchorStyles.Right
        };
        clearLogsButton.Click += (s, e) => { _logTextBox.Clear(); _logListBox.Items.Clear(); };

        Controls.Add(_deviceTypeCombo);
        Controls.Add(enableButton);
        Controls.Add(disableButton);
        Controls.Add(_connectionStatusLabel);
        Controls.Add(lineLabel);
        Controls.Add(_lineTextBox);
        Controls.Add(dialLabel);
        Controls.Add(_dialNumberTextBox);
        Controls.Add(pickupButton);
        Controls.Add(hookOnButton);
        Controls.Add(dialButton);
        Controls.Add(titleLabel);
        Controls.Add(developerLabel);
        Controls.Add(statusLabel);
        Controls.Add(_statusPanel);
        Controls.Add(logLabel);
        Controls.Add(_logListBox);
        Controls.Add(_logTextBox);
        Controls.Add(_copyLogsButton);
        Controls.Add(clearLogsButton);

        _deviceService.SetMessageWindow(Handle);
    }

    private void SetupControls()
    {
        UpdateStatusPanel();
    }

    private void UpdateStatusPanel()
    {
        _statusPanel.Controls.Clear();

        var lines = _deviceService.GetAllLines().ToList();
        if (lines.Count == 0)
        {
            lines.Add(new LineInfo { LineIndex = 0 });
        }

        int y = 5;
        foreach (var line in lines)
        {
            var linePanel = new Panel
            {
                Location = new Point(5, y),
                Size = new Size(850, 40),
                BorderStyle = BorderStyle.FixedSingle
            };

            var lineLabel = new Label
            {
                Text = $"Line {line.LineIndex}:",
                Location = new Point(5, 10),
                Size = new Size(80, 20),
                Font = new Font(Font.FontFamily, 9, FontStyle.Bold)
            };

            var statusLabel = new Label
            {
                Text = $"Status: {line.Status}",
                Location = new Point(90, 10),
                Size = new Size(150, 20)
            };

            var callerLabel = new Label
            {
                Text = $"Caller ID: {line.LastCallerId ?? "N/A"}",
                Location = new Point(250, 10),
                Size = new Size(200, 20)
            };

            var dtmfLabel = new Label
            {
                Text = $"DTMF: {line.LastDtmf ?? "N/A"}",
                Location = new Point(460, 10),
                Size = new Size(150, 20)
            };

            var connectedLabel = new Label
            {
                Text = line.DeviceConnected ? "Connected" : "Disconnected",
                Location = new Point(620, 10),
                Size = new Size(100, 20),
                ForeColor = line.DeviceConnected ? Color.Green : Color.Red
            };

            var ringLabel = new Label
            {
                Text = $"Rings: {line.RingCount}",
                Location = new Point(730, 10),
                Size = new Size(100, 20)
            };

            linePanel.Controls.Add(lineLabel);
            linePanel.Controls.Add(statusLabel);
            linePanel.Controls.Add(callerLabel);
            linePanel.Controls.Add(dtmfLabel);
            linePanel.Controls.Add(connectedLabel);
            linePanel.Controls.Add(ringLabel);

            _statusPanel.Controls.Add(linePanel);
            y += 45;
        }
    }

    private void AddLog(string message, bool isError = false)
    {
        if (InvokeRequired)
        {
            Invoke(new Action(() => AddLog(message, isError)));
            return;
        }

        // Check if controls are initialized
        if (_logListBox == null || _logTextBox == null)
            return;

        var timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
        var logEntry = $"[{timestamp}] {(isError ? "ERROR: " : "")}{message}";
        
        try
        {
            // Add to ListBox
            _logListBox.Items.Insert(0, logEntry);
            if (_logListBox.Items.Count > 1000)
            {
                _logListBox.Items.RemoveAt(_logListBox.Items.Count - 1);
            }

            // Add to TextBox
            _logTextBox.AppendText(logEntry + Environment.NewLine);
            _logTextBox.SelectionStart = _logTextBox.Text.Length;
            _logTextBox.ScrollToCaret();
        }
        catch
        {
            // Ignore errors if controls aren't ready
        }
    }

    private void DeviceService_OnDeviceEvent(object? sender, DeviceEvent e)
    {
        if (InvokeRequired)
        {
            Invoke(new Action(() => DeviceService_OnDeviceEvent(sender, e)));
            return;
        }

        var logEntry = $"EVENT: {e.ApiFamily} EventCode={e.EventCode} Line={e.Line} Param={e.Param} DeviceId={e.DeviceId}";
        if (!string.IsNullOrEmpty(e.Meaning))
            logEntry += $" | {e.Meaning}";
        if (!string.IsNullOrEmpty(e.CallerId))
            logEntry += $" | CallerID: {e.CallerId}";
        if (!string.IsNullOrEmpty(e.Dtmf))
            logEntry += $" | DTMF: {e.Dtmf}";
        if (!string.IsNullOrEmpty(e.Status))
            logEntry += $" | Status: {e.Status}";

        AddLog(logEntry);

        UpdateStatusPanel();

        if (e.EventCode == 22) // USB Connect
        {
            UpdateConnectionStatus(true);
        }
        else if (e.EventCode == 23) // USB Disconnect
        {
            UpdateConnectionStatus(false);
        }
    }

    private void CopyLogsButton_Click(object? sender, EventArgs e)
    {
        try
        {
            if (!string.IsNullOrEmpty(_logTextBox.Text))
            {
                Clipboard.SetText(_logTextBox.Text);
                AddLog("Logs copied to clipboard");
            }
            else
            {
                MessageBox.Show("No logs to copy", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
        catch (Exception ex)
        {
            MessageBox.Show($"Failed to copy logs: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private void UpdateConnectionStatus(bool connected)
    {
        _connectionStatusLabel.Text = connected ? "Status: Connected" : "Status: Disconnected";
        _connectionStatusLabel.ForeColor = connected ? Color.Green : Color.Red;
    }

    private void EnableButton_Click(object? sender, EventArgs e)
    {
        var deviceType = (UsbBoxDeviceType)_deviceTypeCombo.SelectedIndex;
        AddLog($"BUTTON CLICK: Enable Device | DeviceType: {deviceType} (Index: {(int)deviceType})");
        
        try
        {
            // Check device count before enabling
            var deviceCount = _deviceService.GetDeviceCount();
            AddLog($"INFO: Current device count: {deviceCount}");
            
            if (deviceCount == 0)
            {
                AddLog("WARNING: No devices detected. Make sure USB device is connected.", true);
            }

            bool success = _deviceService.StartUsbBox(deviceType);
            var errorMsg = _deviceService.LastError;
            
            if (success)
            {
                var newDeviceCount = _deviceService.GetDeviceCount();
                var channelCount = _deviceService.GetChannelCount();
                AddLog($"RESPONSE: Device enabled successfully | DeviceType: {deviceType} | DeviceCount: {newDeviceCount} | ChannelCount: {channelCount}");
                MessageBox.Show($"Device enabled successfully!\nDevice Count: {newDeviceCount}\nChannel Count: {channelCount}", 
                    "Enable Device", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                AddLog($"ERROR: Failed to enable device | DeviceType: {deviceType} | Error: {errorMsg ?? "Unknown error"}", true);
                
                var detailedMsg = $"Failed to enable device.\n\n{errorMsg ?? "Unknown error"}\n\n" +
                    $"Troubleshooting:\n" +
                    $"1. Check if USB device is physically connected\n" +
                    $"2. Verify device drivers are installed\n" +
                    $"3. Ensure device is not used by another application\n" +
                    $"4. Try a different device type (F1/F2/F4/F8)\n" +
                    $"5. Check Device Manager for device status";
                
                MessageBox.Show(detailedMsg, "Enable Device Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        catch (Exception ex)
        {
            AddLog($"EXCEPTION: Enable Device | Type: {ex.GetType().Name} | Message: {ex.Message} | StackTrace: {ex.StackTrace}", true);
            MessageBox.Show($"Exception occurred:\n\nType: {ex.GetType().Name}\nMessage: {ex.Message}", 
                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private void DisableButton_Click(object? sender, EventArgs e)
    {
        AddLog("BUTTON CLICK: Disable Device");
        
        try
        {
            bool success = _deviceService.StopUsbBox();
            if (success)
            {
                AddLog("RESPONSE: Device disabled successfully");
                MessageBox.Show("Device disabled successfully", "Disable Device", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                AddLog("ERROR: Failed to disable device", true);
                MessageBox.Show("Failed to disable device", "Disable Device", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        catch (Exception ex)
        {
            AddLog($"EXCEPTION: Disable Device | {ex.Message}", true);
            MessageBox.Show($"Exception: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private void PickupButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line))
        {
            AddLog($"BUTTON CLICK: Pickup | Line: {line}");
            
            try
            {
                bool success = _deviceService.Pickup(line);
                if (success)
                {
                    AddLog($"RESPONSE: Picked up line {line} successfully");
                    MessageBox.Show($"Picked up line {line}", "Pickup", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    AddLog($"ERROR: Failed to pickup line {line}", true);
                    MessageBox.Show($"Failed to pickup line {line}", "Pickup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                AddLog($"EXCEPTION: Pickup Line {line} | {ex.Message}", true);
                MessageBox.Show($"Exception: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        else
        {
            AddLog("ERROR: Invalid line number", true);
            MessageBox.Show("Please enter a valid line number", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
    }

    private void HookOnButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line))
        {
            AddLog($"BUTTON CLICK: Hook On | Line: {line}");
            
            try
            {
                bool success = _deviceService.HookOn(line);
                if (success)
                {
                    AddLog($"RESPONSE: Hooked on line {line} successfully");
                    MessageBox.Show($"Hooked on line {line}", "Hook On", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    AddLog($"ERROR: Failed to hook on line {line}", true);
                    MessageBox.Show($"Failed to hook on line {line}", "Hook On", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                AddLog($"EXCEPTION: Hook On Line {line} | {ex.Message}", true);
                MessageBox.Show($"Exception: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        else
        {
            AddLog("ERROR: Invalid line number", true);
            MessageBox.Show("Please enter a valid line number", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
    }

    private void DialButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line) && !string.IsNullOrWhiteSpace(_dialNumberTextBox.Text))
        {
            var number = _dialNumberTextBox.Text;
            AddLog($"BUTTON CLICK: Dial | Line: {line} | Number: {number}");
            
            try
            {
                bool success = _deviceService.Dial(line, number);
                if (success)
                {
                    AddLog($"RESPONSE: Dialing {number} on line {line} successfully");
                    MessageBox.Show($"Dialing {number} on line {line}", "Dial", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    AddLog($"ERROR: Failed to dial {number} on line {line}", true);
                    MessageBox.Show($"Failed to dial on line {line}", "Dial", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                AddLog($"EXCEPTION: Dial Line {line} Number {number} | {ex.Message}", true);
                MessageBox.Show($"Exception: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        else
        {
            AddLog("ERROR: Invalid line number or dial number", true);
            MessageBox.Show("Please enter a valid line number and dial number", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
    }

    protected override void OnFormClosing(FormClosingEventArgs e)
    {
        _deviceService.StopUsbBox();
        _deviceService.StopAD101();
        _deviceService.StopAD800();
        base.OnFormClosing(e);
    }
}

