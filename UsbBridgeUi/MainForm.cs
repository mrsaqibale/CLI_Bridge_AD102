using UsbDeviceCore;

namespace UsbBridgeUi;

public partial class MainForm : Form
{
    private readonly DeviceService _deviceService;
    private ListBox _logListBox;
    private Panel _statusPanel;
    private ComboBox _deviceTypeCombo;
    private TextBox _lineTextBox;
    private TextBox _dialNumberTextBox;
    private Label _connectionStatusLabel;

    public MainForm()
    {
        try
        {
            _deviceService = new DeviceService();
            _deviceService.OnDeviceEvent += DeviceService_OnDeviceEvent;

            InitializeComponent();
            SetupControls();
            UpdateConnectionStatus(false);
            
            // Ensure form is visible
            this.Visible = true;
            this.WindowState = FormWindowState.Normal;
            this.ShowInTaskbar = true;
        }
        catch (Exception ex)
        {
            MessageBox.Show($"Error initializing form: {ex.Message}\n\n{ex.StackTrace}", 
                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            throw;
        }
    }

    private void InitializeComponent()
    {
        Text = "USB Device Bridge - Test UI";
        Size = new Size(900, 700);
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

        var statusLabel = new Label
        {
            Text = "Line Status:",
            Location = new Point(10, 90),
            Size = new Size(100, 20),
            Font = new Font(Font.FontFamily, 9, FontStyle.Bold)
        };

        _statusPanel = new Panel
        {
            Location = new Point(10, 115),
            Size = new Size(870, 200),
            BorderStyle = BorderStyle.FixedSingle,
            AutoScroll = true
        };

        var logLabel = new Label
        {
            Text = "Event Log:",
            Location = new Point(10, 325),
            Size = new Size(100, 20),
            Font = new Font(Font.FontFamily, 9, FontStyle.Bold)
        };

        _logListBox = new ListBox
        {
            Location = new Point(10, 350),
            Size = new Size(870, 300),
            Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right
        };

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
        Controls.Add(statusLabel);
        Controls.Add(_statusPanel);
        Controls.Add(logLabel);
        Controls.Add(_logListBox);

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

    private void DeviceService_OnDeviceEvent(object? sender, DeviceEvent e)
    {
        if (InvokeRequired)
        {
            Invoke(new Action(() => DeviceService_OnDeviceEvent(sender, e)));
            return;
        }

        var logEntry = $"[{e.Timestamp:HH:mm:ss.fff}] {e.ApiFamily} Event {e.EventCode} Line {e.Line}: {e.Meaning}";
        if (!string.IsNullOrEmpty(e.CallerId))
            logEntry += $" CallerID: {e.CallerId}";
        if (!string.IsNullOrEmpty(e.Dtmf))
            logEntry += $" DTMF: {e.Dtmf}";

        _logListBox.Items.Insert(0, logEntry);
        if (_logListBox.Items.Count > 1000)
        {
            _logListBox.Items.RemoveAt(_logListBox.Items.Count - 1);
        }

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

    private void UpdateConnectionStatus(bool connected)
    {
        _connectionStatusLabel.Text = connected ? "Status: Connected" : "Status: Disconnected";
        _connectionStatusLabel.ForeColor = connected ? Color.Green : Color.Red;
    }

    private void EnableButton_Click(object? sender, EventArgs e)
    {
        var deviceType = (UsbBoxDeviceType)_deviceTypeCombo.SelectedIndex;
        bool success = _deviceService.StartUsbBox(deviceType);
        MessageBox.Show(success ? "Device enabled successfully" : "Failed to enable device",
            "Enable Device", MessageBoxButtons.OK, success ? MessageBoxIcon.Information : MessageBoxIcon.Error);
    }

    private void DisableButton_Click(object? sender, EventArgs e)
    {
        bool success = _deviceService.StopUsbBox();
        MessageBox.Show(success ? "Device disabled successfully" : "Failed to disable device",
            "Disable Device", MessageBoxButtons.OK, success ? MessageBoxIcon.Information : MessageBoxIcon.Error);
    }

    private void PickupButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line))
        {
            bool success = _deviceService.Pickup(line);
            MessageBox.Show(success ? $"Picked up line {line}" : $"Failed to pickup line {line}",
                "Pickup", MessageBoxButtons.OK, success ? MessageBoxIcon.Information : MessageBoxIcon.Error);
        }
    }

    private void HookOnButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line))
        {
            bool success = _deviceService.HookOn(line);
            MessageBox.Show(success ? $"Hooked on line {line}" : $"Failed to hook on line {line}",
                "Hook On", MessageBoxButtons.OK, success ? MessageBoxIcon.Information : MessageBoxIcon.Error);
        }
    }

    private void DialButton_Click(object? sender, EventArgs e)
    {
        if (int.TryParse(_lineTextBox.Text, out int line) && !string.IsNullOrWhiteSpace(_dialNumberTextBox.Text))
        {
            bool success = _deviceService.Dial(line, _dialNumberTextBox.Text);
            MessageBox.Show(success ? $"Dialing {_dialNumberTextBox.Text} on line {line}" : $"Failed to dial on line {line}",
                "Dial", MessageBoxButtons.OK, success ? MessageBoxIcon.Information : MessageBoxIcon.Error);
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

