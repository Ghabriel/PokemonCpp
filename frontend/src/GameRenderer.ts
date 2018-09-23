import { Camera } from './Camera';
import { GameLoop } from './engine/GameLoop';
import { ResourceStorage } from './engine/resource-system/ResourceStorage';
import { render } from './render';

export class GameRenderer {
    constructor(
        private resourceStorage: ResourceStorage,
        private canvas: HTMLCanvasElement
    ) {
        const camera = resourceStorage.get<Camera>('camera');
        canvas.width = camera.width;
        canvas.height = camera.height;
        this.adjustView();

        this.camera = camera;
        this.context = this.getContext();
    }

    render(gameLoop: GameLoop): void {
        this.adjustView();
        this.clearCanvas();
        render(this.context, this.resourceStorage);
    }

    private getContext(): CanvasRenderingContext2D {
        const context = this.canvas.getContext('2d');

        if (!context) {
            throw Error('Failed to get 2D context');
        }

        return context;
    }

    private adjustView(): void {
        // TODO
    }

    private clearCanvas(): void {
        this.context.clearRect(0, 0, this.camera.width, this.camera.height);
    }

    private camera: Camera;
    private context: CanvasRenderingContext2D;
}
