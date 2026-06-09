/*
    Canvas clearing animation

    Call all 3 functions to clear the whole screen
*/
export class CanvasClear {
    static clearWithoutAnimation(canvas) {
        const ctx = canvas?.getContext('2d');

        if (import.meta.env.DEV && (!canvas || !ctx)) {
            console.error('[CanvasClear]: Canvas wasn\'t defined');
            return;
        }

        ctx.save();
        ctx.fillStyle = 'white';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.restore();
    }

    static clear1(canvas) { this._clear(canvas, 3, 1); }
    static clear2(canvas) { this._clear(canvas, 3, 2); }
    static clear3(canvas) { this._clear(canvas, 3, 3); }

    static _clear(canvas, iterationCount, iteration) {
        const ctx = canvas?.getContext('2d');

        if (import.meta.env.DEV && (!canvas || !ctx)) {
            console.error('[CanvasClear]: Canvas wasn\'t defined');
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