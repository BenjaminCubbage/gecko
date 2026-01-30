class GIBToCanvas {
    static writeBlob(canvas, buffer) {
        const ctx = canvas?.getContext('2d');
        if (!ctx) {
            console.warn('`canvas?.getContext("2d")` was null or undefined.');
            return;
        }

        const compressedBytes = new Module.VectorUint8();
        compressedBytes.resize(buffer.length);

        for (let i = 0; i < buffer.length; ++i)
            compressedBytes.set(i, buffer[i]);

        let width  = 0;
        let height = 0;
        let rgba = null;

        /* Called first */
        const cbHeader = Module.addFunction((w, h) => {
            width  = w;
            height = h;
            rgba = new Uint8ClampedArray(w * h * 4);
            return true;
        }, "iii");

        /* Called for every decoded pixel */
        const cbWriter = Module.addFunction((y, xStart, xEnd, white) => {
            const start = width * y + xStart;
            const end   = start + xEnd - xStart;

            for (let i = start; i <= end; ++i) {
                rgba[i * 4 + 0] = white ? 255 : 0;
                rgba[i * 4 + 1] = rgba[i * 4 + 0];
                rgba[i * 4 + 2] = rgba[i * 4 + 0];
                rgba[i * 4 + 3] = 255;
            }
        }, "viiii");

        const success = Module.Decode.TryDecompress(compressedBytes, cbHeader, cbWriter);

        Module.removeFunction(cbWriter);
        Module.removeFunction(cbHeader);
        compressedBytes.delete();

        if (!success) {
            console.warn(`Failed to decompress an image.`);
            return false;
        }

        ctx.putImageData(new ImageData(rgba, width, height), 0, 0);
        return true;
    }
};

export { GIBToCanvas };