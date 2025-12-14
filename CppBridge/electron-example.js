// Electron integration with C++ Bridge
// Auto-runs EXE in background and receives call events

const { spawn } = require('child_process');
const path = require('path');

class UsbBridge {
    constructor() {
        this.bridge = null;
        this.eventHandlers = [];
        this.isRunning = false;
    }

    start() {
        // Path to the C++ bridge EXE (auto-runs in background)
        const bridgePath = path.join(__dirname, 'bin', 'x64', 'Release', 'CppBridge.exe');
        
        console.log('[Bridge] Starting C++ bridge in background...');
        console.log('[Bridge] Path:', bridgePath);
        
        // Spawn the EXE in background (auto-enables device)
        this.bridge = spawn(bridgePath, [], {
            stdio: ['ignore', 'pipe', 'pipe'], // stdin ignored, stdout/stderr piped
            cwd: path.dirname(bridgePath),
            detached: false, // Keep attached to parent process
            windowsHide: true // Hide console window on Windows
        });

        // Handle stdout (JSON events)
        let buffer = '';
        this.bridge.stdout.on('data', (data) => {
            buffer += data.toString();
            const lines = buffer.split('\n');
            buffer = lines.pop() || ''; // Keep incomplete line in buffer
            
            lines.forEach(line => {
                const trimmed = line.trim();
                if (trimmed) {
                    try {
                        const event = JSON.parse(trimmed);
                        this.handleEvent(event);
                    } catch (e) {
                        // Ignore parse errors (might be partial JSON)
                        // console.error('Failed to parse event:', trimmed, e);
                    }
                }
            });
        });

        // Handle stderr (logs from C++ bridge)
        this.bridge.stderr.on('data', (data) => {
            const log = data.toString().trim();
            if (log) {
                console.log('[Bridge]', log);
            }
        });

        // Handle process exit
        this.bridge.on('exit', (code) => {
            console.log(`[Bridge] Process exited with code ${code}`);
            this.isRunning = false;
            this.bridge = null;
        });

        // Handle errors
        this.bridge.on('error', (err) => {
            console.error('[Bridge] Error:', err);
            this.isRunning = false;
        });

        this.isRunning = true;
        
        return new Promise((resolve, reject) => {
            // Wait a bit for bridge to initialize and auto-enable device
            setTimeout(() => {
                if (this.isRunning) {
                    console.log('[Bridge] ✓ Ready!');
                    console.log('[Bridge] ✓ Device auto-enabled (F2)');
                    console.log('[Bridge] ✓ Listening for incoming calls...\n');
                    resolve();
                } else {
                    reject(new Error('Bridge failed to start'));
                }
            }, 1500); // Give more time for device initialization
        });
    }

    stop() {
        if (this.bridge) {
            console.log('[Bridge] Stopping...');
            this.bridge.kill();
            this.bridge = null;
            this.isRunning = false;
        }
    }

    onEvent(handler) {
        this.eventHandlers.push(handler);
    }

    handleEvent(event) {
        // Process the event
        if (event.type === 'event') {
            if (event.status === 'Ring') {
                // Call comes - display caller ID prominently
                const callerId = event.callerId || 'Unknown';
                const line = event.line || 0;
                
                console.log('\n' + '='.repeat(50));
                console.log(`📞 INCOMING CALL - LINE ${line}`);
                console.log(`📱 CALL FROM THIS NUMBER: ${callerId}`);
                console.log(`🔔 STATUS: RINGING`);
                console.log('='.repeat(50) + '\n');
            } else if (event.status === 'Free') {
                // Call ends
                const line = event.line || 0;
                console.log(`\n✓ Call ended on line ${line} (Line is now FREE)\n`);
            }
        }
        
        // Notify all handlers
        this.eventHandlers.forEach(handler => {
            try {
                handler(event);
            } catch (e) {
                console.error('Event handler error:', e);
            }
        });
    }
}

// Usage example
async function main() {
    const bridge = new UsbBridge();
    
    // Register event handler for custom processing
    bridge.onEvent((event) => {
        // You can add custom logic here
        // For example, update UI, save to database, send notifications, etc.
        if (event.type === 'event' && event.status === 'Ring') {
            // Example: You could trigger UI updates, notifications, etc.
            // updateUI(event);
            // saveToDatabase(event);
            // sendNotification(`Call from ${event.callerId}`);
        }
    });
    
    try {
        // Start bridge (auto-runs EXE in background, auto-enables device)
        await bridge.start();
        
        console.log('USB Bridge is running in background.');
        console.log('The EXE file is auto-running and listening for calls.');
        console.log('When a call comes, you will see: "CALL FROM THIS NUMBER: [number]"\n');
        
        // Keep process alive
        process.on('SIGINT', () => {
            console.log('\n[Bridge] Shutting down...');
            bridge.stop();
            process.exit(0);
        });
        
        process.on('SIGTERM', () => {
            console.log('\n[Bridge] Shutting down...');
            bridge.stop();
            process.exit(0);
        });
        
    } catch (error) {
        console.error('Failed to start bridge:', error);
        process.exit(1);
    }
}

// Run if executed directly
if (require.main === module) {
    main().catch(console.error);
}

module.exports = UsbBridge;

