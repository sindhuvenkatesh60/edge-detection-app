interface FrameData {
    imageData: string; // base64 encoded
    width: number;
    height: number;
    fps: number;
    processingTime: number;
    mode: string;
}

class EdgeDetectionViewer {
    private canvas: HTMLCanvasElement;
    private ctx: CanvasRenderingContext2D;
    private statsPanel: HTMLDivElement;
    private frameCount: number = 0;
    private lastTime: number = 0;
    private currentFPS: number = 0;
    private isPlaying: boolean = true;

    constructor() {
        this.canvas = document.getElementById('outputCanvas') as HTMLCanvasElement;
        this.ctx = this.canvas.getContext('2d')!;
        this.statsPanel = document.getElementById('statsPanel') as HTMLDivElement;
        
        this.initializeUI();
        this.setupEventListeners();
        this.startFrameSimulation();
    }

    private initializeUI(): void {
        this.canvas.width = 640;
        this.canvas.height = 480;
        this.updateStats();
    }

    private setupEventListeners(): void {
        const toggleBtn = document.getElementById('toggleBtn') as HTMLButtonElement;
        const modeSelect = document.getElementById('modeSelect') as HTMLSelectElement;

        toggleBtn.addEventListener('click', () => {
            this.isPlaying = !this.isPlaying;
            toggleBtn.textContent = this.isPlaying ? 'Pause' : 'Play';
        });

        modeSelect.addEventListener('change', (e) => {
            this.updateStats();
        });
    }

    private startFrameSimulation(): void {
        const simulateFrame = () => {
            if (this.isPlaying) {
                this.generateAndDisplayFrame();
            }
            requestAnimationFrame(simulateFrame);
        };
        simulateFrame();

        // Update FPS counter every second
        setInterval(() => {
            this.updateFPS();
        }, 1000);
    }

    private generateAndDisplayFrame(): void {
        const modeSelect = document.getElementById('modeSelect') as HTMLSelectElement;
        const mode = modeSelect.value;
        
        const imageData = this.generateFrame(mode);
        this.displayFrame(imageData);
        this.frameCount++;
    }

    private generateFrame(mode: string): ImageData {
        const imageData = this.ctx.createImageData(this.canvas.width, this.canvas.height);
        const data = imageData.data;
        const time = Date.now() / 1000;

        for (let y = 0; y < this.canvas.height; y++) {
            for (let x = 0; x < this.canvas.width; x++) {
                const index = (y * this.canvas.width + x) * 4;
                
                let r = 0, g = 0, b = 0;

                switch (mode) {
                    case 'edges':
                        // Simulate edge detection output
                        const edgeValue = Math.sin(x * 0.05 + time) * Math.sin(y * 0.05 + time) > 0.7 ? 255 : 0;
                        r = g = b = edgeValue;
                        break;
                    
                    case 'grayscale':
                        // Simulate grayscale output
                        const intensity = (Math.sin(x * 0.02 + time) + 1) * 127;
                        r = g = b = intensity;
                        break;
                    
                    case 'original':
                        // Simulate original camera feed with some movement
                        r = (x + time * 50) % 255;
                        g = (y + time * 30) % 255;
                        b = (x + y + time * 70) % 255;
                        break;
                }
                
                data[index] = r;
                data[index + 1] = g;
                data[index + 2] = b;
                data[index + 3] = 255;
            }
        }

        return imageData;
    }

    private displayFrame(imageData: ImageData): void {
        this.ctx.putImageData(imageData, 0, 0);
    }

    private updateFPS(): void {
        const currentTime = performance.now();
        if (this.lastTime > 0) {
            this.currentFPS = Math.round((this.frameCount * 1000) / (currentTime - this.lastTime));
        }
        this.frameCount = 0;
        this.lastTime = currentTime;
        this.updateStats();
    }

    private updateStats(): void {
        const modeSelect = document.getElementById('modeSelect') as HTMLSelectElement;
        const mode = modeSelect.value;
        
        this.statsPanel.innerHTML = `
            <div class="stat-item">
                <span class="stat-label">Resolution:</span>
                <span class="stat-value">${this.canvas.width}×${this.canvas.height}</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">FPS:</span>
                <span class="stat-value">${this.currentFPS}</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">Mode:</span>
                <span class="stat-value">${mode.toUpperCase()}</span>
            </div>
            <div class="stat-item">
                <span class="stat-label">Status:</span>
                <span class="stat-value">${this.isPlaying ? 'LIVE' : 'PAUSED'}</span>
            </div>
        `;
    }

    // Method to simulate receiving data from Android app
    public receiveFrameData(frameData: FrameData): void {
        const img = new Image();
        img.onload = () => {
            this.ctx.drawImage(img, 0, 0, this.canvas.width, this.canvas.height);
        };
        img.src = frameData.imageData;
        this.updateStats();
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    new EdgeDetectionViewer();
});

// Export for potential module usage
export default EdgeDetectionViewer;