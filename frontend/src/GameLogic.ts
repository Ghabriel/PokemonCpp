import controlsMap from '../../resources/json/controls.json';
import { CoreStructures } from './CoreStructures';
import { GameLoop, UpdateFunction } from './engine/GameLoop';
import { InputDispatcher } from './engine/input-system/InputDispatcher';
import { ResourceStorage } from './engine/resource-system/ResourceStorage';
import { StateMachine } from './engine/state-system/StateMachine';
import { loadInputTracker } from './init/load-input-tracker';

export class GameLogic implements UpdateFunction {
    constructor(resourceStorage: ResourceStorage) {
        const inputTracker = loadInputTracker(controlsMap);
        const inputDispatcher = new InputDispatcher(inputTracker);
        const stateMachine = new StateMachine();

        this.gameData = {
            resourceStorage,
            inputDispatcher,
            stateMachine,
            timeSinceLastFrame: 0
        };
    }

    tick(_: GameLoop, timeSinceLastFrame: number): void {
        this.gameData.timeSinceLastFrame = timeSinceLastFrame;
        this.gameData.inputDispatcher.tick();
        this.gameData.stateMachine.execute();
    }

    private gameData: CoreStructures;
}
