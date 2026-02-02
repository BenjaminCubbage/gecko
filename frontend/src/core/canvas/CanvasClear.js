/*
    Canvas clearing animation

    Call all 3 functions to clear the whole screen
*/
class CanvasClear {
    static clear1(canvas) { this._clear(canvas, 3, 1); }
    static clear2(canvas) { this._clear(canvas, 3, 2); }
    static clear3(canvas) { this._clear(canvas, 3, 3); }

    static _clear(canvas, iterationCount, iteration) {
        const ctx = canvas?.getContext('2d');

        if (!canvas) {
            console.warn('canvas was null or undefined.');
            return;
        }

        if (!ctx) {
            console.warn('`canvas?.getContext("2d")` was null or undefined.');
            return;
        }

        const width  = canvas.width;
        const height = canvas.height;
        const imageData = ctx.getImageData(0, 0, width, height).data;

        for (let y = 0; y < height; ++y) {
            for (let x = (iteration - 1 - y % iterationCount); x < width; x += iterationCount) {
                if (x < 0)
                    continue;

                imageData[(y * width + x) * 4 + 0] = 255;
                imageData[(y * width + x) * 4 + 1] = 255;
                imageData[(y * width + x) * 4 + 2] = 255;
                imageData[(y * width + x) * 4 + 3] = 255;
            }
        }

        ctx.putImageData(new ImageData(imageData, width, height), 0, 0);
    }
};

export { CanvasClear };