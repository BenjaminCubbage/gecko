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
        this._inflight   = {};
        this._globalLock = false;
    }

    tryLock(id) {
        if (this._globalLock || this._inflight[id])
            return false;
        this._inflight[id] = true;
        return true;
    }

    unlock(id) {
        if (!this._inflight[id])
            return false;
        this._inflight[id] = false;
        return true;
    }

    tryLockMany(ids) {
        for (const id of ids)
            if (this._inflight[id])
                return false;
        for (const id of ids)
            this._inflight[id] = true;
        return true;
    }

    unlockMany(ids) {
        for (const id of ids)
            this._inflight[id] = false;
    }

    tryLockAll() {
        if (Object.values(this._inflight).some(v => v))
            return false;
        this._globalLock = true;
        return true;
    }

    unlockAll() {
        if (!this._globalLock && !this._inflight.some(v => v))
            return false;
        this._globalLock = false;
        this._inflight   = [];
        return true;
    }
}

export { ResourceMutex, MultiResourceMutex };