export class ResourceStorage {
    store<T>(identifier: string, data: T): void {
        this.storage.set(identifier, data);
    }

    get<T>(identifier: string): T {
        if (!this.storage.has(identifier)) {
            throw Error('Resource not found: ' + identifier);
        }

        return this.storage.get(identifier) as T;
    }

    remove(identifier: string): void {
        this.storage.delete(identifier);
    }

    private storage = new Map<string, unknown>();
}
