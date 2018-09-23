type UpdateFunction = (gameLoop: GameLoop, elapsedTime: number) => void;
type RenderFunction = (gameLoop: GameLoop) => void;

export class GameLoop {
    constructor(private update: UpdateFunction, private render: RenderFunction) {
        this.updatePeriod = 1000 / this.defaultUpdateFrequency;
    }

    setUpdateFrequency(ticksPerSecond: number): void {
        this.updatePeriod = 1000 / ticksPerSecond;
    }

    start(): void {
        this.running = true;
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
            this.update(this, this.updatePeriod);
            this.timeAccumulator -= this.updatePeriod;
        }

        this.render(this);
    }

    private readonly defaultUpdateFrequency = 25;
    private updatePeriod: number;
    private running: boolean = false;
    private currentTime: number = 0;
    private timeAccumulator: number = 0;
}
