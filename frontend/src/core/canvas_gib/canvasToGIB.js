export class CanvasToGIB {
    static readBlob(canvas) {
        const ctx = canvas?.getContext('2d');

        if (import.meta.env.DEV && ctx == null)
            console.warn('CanvasToGIB: Could not get canvas 2d context.');

        const width  = canvas.width;
        const height = canvas.height;
        const canvasBGR = ctx.getImageData(0, 0, width, height);

        const cb = Module.addFunction((pixelX, pixelY) => {
            const pos = pixelY * width + pixelX;

            return canvasBGR.data[pos * 4 + 0] > 240 ||
                   canvasBGR.data[pos * 4 + 1] > 240 ||
                   canvasBGR.data[pos * 4 + 2] > 240;
        }, "iii");

        const compressedBytes = Module.Encode.TryCompress(cb, width, height);

        const size = compressedBytes.size();
        const copy = new Uint8Array(size);
        for (let i = 0; i < size; ++i)
            copy[i] = compressedBytes.get(i);

        compressedBytes.delete();
        return new Blob([copy]);
    }
};