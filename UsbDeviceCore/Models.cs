namespace UsbDeviceCore;

public class DeviceEvent
{
    public ApiFamily ApiFamily { get; set; }
    public int EventCode { get; set; }
    public int Line { get; set; }
    public uint Param { get; set; }
    public uint DeviceId { get; set; }
    public DateTime Timestamp { get; set; }
    public string? CallerId { get; set; }
    public string? Dtmf { get; set; }
    public string? Status { get; set; }
    public string? Meaning { get; set; }
    public long? Duration { get; set; }
    public string? CallType { get; set; }
}

public class LineInfo
{
    public int LineIndex { get; set; }
    public LineStatus Status { get; set; }
    public string? LastCallerId { get; set; }
    public string? LastDtmf { get; set; }
    public int LastEventCode { get; set; }
    public bool DeviceConnected { get; set; }
    public int RingCount { get; set; }
    public int Voltage { get; set; }
}

