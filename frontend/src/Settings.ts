import settings from '../../resources/json/settings.json';

export class Settings {
    getInitialState(): string {
        return settings['initial-state'];
    }

    getInitialWindowWidth(): number {
        return settings['initial-window-width'];
    }

    getInitialWindowHeight(): number {
        return settings['initial-window-height'];
    }

    getPlayerWalkingSpeed(): number {
        return parseFloat(settings['player-walking-speed']);
    }

    getTileSize(): number {
        return settings['tile-size'];
    }
}
