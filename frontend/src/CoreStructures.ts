import { InputDispatcher } from './engine/input-system/InputDispatcher';
import { ResourceStorage } from './engine/resource-system/ResourceStorage';
import { StateMachine } from './engine/state-system/StateMachine';

export interface CoreStructures {
    inputDispatcher: InputDispatcher;
    resourceStorage: ResourceStorage;
    stateMachine: StateMachine;
    timeSinceLastFrame: number;
}
