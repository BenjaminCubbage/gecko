export class CanvasUndoRedo {
    #history;
    #historyCurr;
    
    constructor(canvas) {
        this.#history     = [];
        this.#historyCurr = -1;
    }

    get canUndo() {
        return this.#history.length !== 0 && this.#historyCurr >= 1;
    }

    get canRedo() {
        return this.#historyCurr < this.#history.length - 1;
    }

    forgetRedoableHistory() {
        this.#history.length = this.#historyCurr + 1;
    }

    pushState(canvas) {
        this.#history.length = this.#historyCurr + 1;
        this.#history.push(
            this.#ctx(canvas).getImageData(0, 0, canvas.width, canvas.height));
        
        ++this.#historyCurr;
    }

    applyUndo(canvas) {
        if (!this.canUndo)
            return;

        let ctx = canvas?.getContext('2d');

        if (import.meta.env.DEV && ctx == null)
            console.error('CanvasUndoRedo: Could not get canvas 2D context.');

        --this.#historyCurr;
        this.#ctx(canvas).putImageData(this.#history[this.#historyCurr], 0, 0);
    }

    applyRedo(canvas) {
        if (!this.canRedo)
            return;

        ++this.#historyCurr;
        this.#ctx(canvas).putImageData(this.#history[this.#historyCurr], 0, 0);
    }

    #ctx(canvas) {
        let ctx = canvas?.getContext('2d');
        if (import.meta.env.DEV && ctx == null)
            console.error('CanvasUndoRedo: Could not get canvas 2D context.');
        return ctx;
    }
};