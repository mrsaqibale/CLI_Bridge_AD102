// Example Electron integration with C++ Bridge

const { spawn } = require('child_process');
const path = require('path');

class UsbBridge {
    constructor() {
        this.bridge = null;
        this.eventHandlers = [];
    }

    start() {
        const bridgePath = path.join(__dirname, 'bin', 'x64', 'Release', 'CppBridge.exe');
        
        this.bridge = spawn(bridgePath, [], {
            stdio: ['pipe', 'pipe', 'pipe'],
            cwd: path.dirname(bridgePath)
        });

        // Handle stdout (events)
        let buffer = '';
        this.bridge.stdout.on('data', (data) => {
            buffer += data.toString();
            const lines = buffer.split('\n');
            buffer = lines.pop() || ''; // Keep incomplete line in buffer
            
            lines.forEach(line => {
                if (line.trim()) {
                    try {
                        const event = JSON.parse(line);
                        this.handleEvent(event);
                    } catch (e) {
                        console.error('Failed to parse event:', line, e);
                    }
                }
            });
        });

        // Handle stderr (logs)
        this.bridge.stderr.on('data', (data) => {
            console.log('[Bridge]', data.toString());
        });

        // Handle process exit
        this.bridge.on('exit', (code) => {
            console.log(`Bridge exited with code ${code}`);
        });

        return new Promise((resolve, reject) => {
            // Wait for bridge to be ready
            setTimeout(() => resolve(), 500);
        });
    }

    stop() {
        if (this.bridge) {
            this.bridge.kill();
            this.bridge = null;
        }
    }

    sendCommand(cmd) {
        if (!this.bridge || !this.bridge.stdin) {
            throw new Error('Bridge not started');
        }
        
        const json = JSON.stringify(cmd) + '\n';
        this.bridge.stdin.write(json);
    }

    onEvent(handler) {
        this.eventHandlers.push(handler);
    }

    handleEvent(event) {
        this.eventHandlers.forEach(handler => {
            try {
                handler(event);
            } catch (e) {
                console.error('Event handler error:', e);
            }
        });
    }

    // Convenience methods
    enable(deviceType = 'F2') {
        this.sendCommand({ cmd: 'enable', deviceType });
    }

    disable() {
        this.sendCommand({ cmd: 'disable' });
    }

    pickup(line = 0) {
        this.sendCommand({ cmd: 'pickup', line });
    }

    hookOn(line = 0) {
        this.sendCommand({ cmd: 'hookon', line });
    }

    dial(line, number) {
        this.sendCommand({ cmd: 'dial', line, number });
    }
}

// Usage example
async function main() {
    const bridge = new UsbBridge();
    
    // Register event handler
    bridge.onEvent((event) => {
        console.log('Event received:', event);
        
        if (event.type === 'event') {
            if (event.status === 'Ringing' && event.callerId) {
                console.log(`Incoming call from ${event.callerId} on line ${event.line}`);
            }
        }
    });
    
    // Start bridge
    await bridge.start();
    
    // Enable device
    bridge.enable('F2');
    
    // Example: Pickup after 2 seconds
    setTimeout(() => {
        bridge.pickup(0);
    }, 2000);
    
    // Cleanup on exit
    process.on('SIGINT', () => {
        bridge.disable();
        bridge.stop();
        process.exit(0);
    });
}

if (require.main === module) {
    main().catch(console.error);
}

module.exports = UsbBridge;

