class EdgeDetectionViewer {
    private canvas: HTMLCanvasElement;
    private ctx: CanvasRenderingContext2D;
    private fpsCounter: HTMLDivElement;
    private frameCount: number = 0;
    private lastTime: number = 0;
    private currentFPS: number = 0;

    constructor() {
        this.canvas = document.getElementById('outputCanvas') as HTMLCanvasElement;
        this.ctx = this.canvas.getContext('2d')!;
        this.fpsCounter = document.getElementById('fpsCounter') as HTMLDivElement;
        
        this.initializeUI();
        this.loadSampleFrame();
    }

    private initializeUI(): void {
        this.canvas.width = 640;
        this.canvas.height = 480;
        this.updateStats(640, 480, 0);
    }

    public loadSampleFrame(): void {
        const sampleFrame = this.generateSampleFrame();
        this.displayFrame(sampleFrame);
        
        setInterval(() => {
            this.updateFPS();
        }, 1000);
    }

    private generateSampleFrame(): ImageData {
        const imageData = this.ctx.createImageData(this.canvas.width, this.canvas.height);
        const data = imageData.data;

        for (let y = 0; y < this.canvas.height; y++) {
            for (let x = 0; x < this.canvas.width; x++) {
                const index = (y * this.canvas.width + x) * 4;
                const edgeValue = Math.sin(x * 0.1) * Math.sin(y * 0.1) > 0.8 ? 255 : 0;
                
                data[index] = edgeValue;
                data[index + 1] = edgeValue;
                data[index + 2] = edgeValue;
                data[index + 3] = 255;
            }
        }
        return imageData;
    }

    public displayFrame(imageData: ImageData): void {
        this.ctx.putImageData(imageData, 0, 0);
        this.frameCount++;
    }

    private updateFPS(): void {
        const currentTime = performance.now();
        if (this.lastTime > 0) {
            this.currentFPS = Math.round((this.frameCount * 1000) / (currentTime - this.lastTime));
        }
        this.frameCount = 0;
        this.lastTime = currentTime;
        this.updateStats(this.canvas.width, this.canvas.height, this.currentFPS);
    }

    private updateStats(width: number, height: number, fps: number): void {
        this.fpsCounter.innerHTML = `
            Resolution: ${width}×${height}<br>
            FPS: ${fps}<br>
            Mode: Edge Detection
        `;
    }
}

document.addEventListener('DOMContentLoaded', () => {
    new EdgeDetectionViewer();
});