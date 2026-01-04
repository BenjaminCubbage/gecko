class ResourceMutex {
    constructor() {
        this._locked = false;
    }

    tryLock() {
        if (this._locked)
            return false;
        this._locked = true;
        return true;
    }

    unlock() {
        if (!this._locked)
            return false;
        this._locked = false;
        return true;
    }
}

class MultiResourceMutex {
    constructor() {
        this._inFlight   = {};
        this._globalLock = false;
    }

    tryLock(id) {
        if (this._globalLock || this._inFlight[id])
            return false;
        this._inFlight[id] = true;
        return true;
    }

    unlock(id) {
        if (!this._inFlight[id])
            return false;
        this._inFlight[id] = false;
        return true;
    }

    tryLockAll() {
        if (Object.values(this._inFlight).some(v => v))
            return false;
        this._globalLock = true;
        return true;
    }

    unlockAll() {
        if (!this._globalLock && !this._inFlight.some(v => v))
            return false;
        this._globalLock = false;
        this._inFlight   = [];
        return true;
    }
}

export { ResourceMutex, MultiResourceMutex };