import { Camera } from './Camera';
import { GameLoop } from './engine/GameLoop';
import { ResourceStorage } from './engine/resource-system/ResourceStorage';
import { GameLogic } from './GameLogic';
import { GameRenderer } from './GameRenderer';
import { Settings } from './Settings';

window.addEventListener('load', main);

function main(): void {
    const canvas = getCanvas();
    const resourceStorage = buildResourceStorage();
    const logic = new GameLogic(resourceStorage);
    const renderer = new GameRenderer(resourceStorage, canvas);
    const gameLoop = new GameLoop(logic, renderer);
    gameLoop.setUpdateFrequency(60);
    gameLoop.start();
}

function getCanvas(): HTMLCanvasElement {
    const canvas = document.getElementById('game-container');

    if (!canvas) {
        throw Error('Canvas element not found');
    }

    if (!(canvas instanceof HTMLCanvasElement)) {
        throw Error('Game container is not a canvas element');
    }

    return canvas;
}

function buildResourceStorage(): ResourceStorage {
    const resourceStorage = new ResourceStorage();
    const settings = new Settings();

    resourceStorage.store('settings', settings);
    resourceStorage.store<Camera>('camera', {
        x: 0,
        y: 0,
        width: settings.getInitialWindowWidth(),
        height: settings.getInitialWindowHeight()
    });

    return resourceStorage;
}
