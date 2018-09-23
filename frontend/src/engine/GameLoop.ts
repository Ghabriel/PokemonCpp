export interface UpdateFunction {
    tick(gameLoop: GameLoop, timeSinceLastFrame: number): void;
}

export interface RenderFunction {
    render(gameLoop: GameLoop): void;
}

export class GameLoop {
    constructor(private update: UpdateFunction, private renderer: RenderFunction) {
        this.updatePeriod = 1000 / this.defaultUpdateFrequency;
    }

    setUpdateFrequency(ticksPerSecond: number): void {
        this.updatePeriod = 1000 / ticksPerSecond;
    }

    start(): void {
        this.running = true;
        this.currentTime = Date.now();
        this.runLoop();
    }

    stop(): void {
        this.running = false;
    }

    private runLoop(): void {
        if (!this.running) {
            return;
        }

        requestAnimationFrame(() => this.runLoop());
        const now = Date.now();
        const elapsedTime = now - this.currentTime;
        this.currentTime = now;
        this.timeAccumulator += elapsedTime;

        while (this.timeAccumulator >= this.updatePeriod) {
            this.update.tick(this, this.updatePeriod);
            this.timeAccumulator -= this.updatePeriod;
        }

        this.renderer.render(this);
    }

    private readonly defaultUpdateFrequency = 25;
    private updatePeriod: number;
    private running: boolean = false;
    private currentTime: number = 0;
    private timeAccumulator: number = 0;
}
