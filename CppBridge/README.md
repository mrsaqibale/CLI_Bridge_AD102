# C++ Console Bridge for Electron

This is a minimal C++ console application that wraps the USBBox DLLs for Electron integration.

## Why C++ Instead of .NET?

- ✅ **No P/Invoke issues** - Direct DLL access
- ✅ **No VC++ Redistributable needed** (if statically linked)
- ✅ **Same code pattern as working demos**
- ✅ **Smaller executable**
- ✅ **Better performance**

## How It Works

1. **Reads JSON commands from stdin:**
   ```json
   {"cmd":"enable","deviceType":"F2"}
   {"cmd":"pickup","line":0}
   {"cmd":"dial","line":0,"number":"1234567890"}
   ```

2. **Outputs JSON events to stdout:**
   ```json
   {"type":"event","eventCode":22,"line":0,"status":"Connected"}
   {"type":"event","eventCode":3,"line":0,"callerId":"1234567890"}
   {"type":"result","ok":true,"cmd":"enable"}
   ```

3. **Uses the same callback pattern as working demos**

## Building

1. Open `CppBridge.sln` in Visual Studio
2. Build for x64 Release
3. Copy DLLs from `EN/64BIT/` to output folder
4. Run: `CppBridge.exe`

## Electron Integration

```javascript
const { spawn } = require('child_process');

const bridge = spawn('./CppBridge.exe', [], {
  stdio: ['pipe', 'pipe', 'pipe']
});

// Send commands
bridge.stdin.write(JSON.stringify({cmd: 'enable', deviceType: 'F2'}) + '\n');

// Receive events
bridge.stdout.on('data', (data) => {
  const lines = data.toString().split('\n');
  lines.forEach(line => {
    if (line.trim()) {
      const event = JSON.parse(line);
      console.log('Event:', event);
    }
  });
});
```

