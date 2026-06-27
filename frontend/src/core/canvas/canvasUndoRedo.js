/*
    Save the image data history of a canvas.
*/

const maxHistoryLen = 500;

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

    /*
        Push canvas state to history. Deletes redoable history beyond
        current point.
    */
    pushState(canvas) {
        this.#history.length = this.#historyCurr + 1;
        this.#history.push(
            this.#ctx(canvas).getImageData(0, 0, canvas.width, canvas.height));
        ++this.#historyCurr;

        /*
            Limit length to maxHistoryLen.

            This assumes only one state is pushed at a time in above
            code.
        */
        if (this.#history.length > maxHistoryLen) {
            this.#history.shift();
            --this.#historyCurr;
        }
    }

    /*
        Undo if possible. If not possible, do nothing.
    */
    applyUndo(canvas) {
        if (!this.canUndo)
            return;

        let ctx = canvas?.getContext('2d');

        if (import.meta.env.DEV && ctx == null)
            console.error('CanvasUndoRedo: Could not get canvas 2D context.');

        --this.#historyCurr;
        this.#ctx(canvas).putImageData(this.#history[this.#historyCurr], 0, 0);
    }

    /*
        Redo if possible. If not possible, do nothing.
    */
    applyRedo(canvas) {
        if (!this.canRedo)
            return;

        ++this.#historyCurr;
        this.#ctx(canvas).putImageData(this.#history[this.#historyCurr], 0, 0);
    }

    /*
        Forget all history beyond current point.
    */
    forgetRedoableHistory() {
        this.#history.length = this.#historyCurr + 1;
    }

    #ctx(canvas) {
        let ctx = canvas?.getContext('2d');
        if (import.meta.env.DEV && ctx == null)
            console.error('CanvasUndoRedo: Could not get canvas 2D context.');
        return ctx;
    }
};