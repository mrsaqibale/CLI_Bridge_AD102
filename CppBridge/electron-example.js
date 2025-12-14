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
        // Path to the C++ bridge EXE
        const bridgePath = path.join(__dirname, 'bin', 'x64', 'Release', 'CppBridge.exe');
        
        console.log('[Bridge] Starting C++ bridge...');
        console.log('[Bridge] Path:', bridgePath);
        
        // Spawn the EXE in background
        this.bridge = spawn(bridgePath, [], {
            stdio: ['ignore', 'pipe', 'pipe'], // stdin ignored, stdout/stderr piped
            cwd: path.dirname(bridgePath),
            detached: false // Keep attached to parent process
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
            // Wait a bit for bridge to initialize
            setTimeout(() => {
                if (this.isRunning) {
                    console.log('[Bridge] Ready! Listening for calls...');
                    resolve();
                } else {
                    reject(new Error('Bridge failed to start'));
                }
            }, 1000);
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
                // Call comes - display caller ID
                const callerId = event.callerId || 'Unknown';
                const line = event.line || 0;
                console.log(`\n📞 INCOMING CALL - LINE ${line}`);
                console.log(`📱 CALL FROM: ${callerId}`);
                console.log(`🔔 STATUS: RINGING\n`);
            } else if (event.status === 'Free') {
                // Call ends
                const line = event.line || 0;
                console.log(`\n✓ Call ended on line ${line}\n`);
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
        // For example, update UI, save to database, etc.
    });
    
    try {
        // Start bridge (auto-enables device)
        await bridge.start();
        
        console.log('USB Bridge is running in background.');
        console.log('Waiting for incoming calls...\n');
        
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

