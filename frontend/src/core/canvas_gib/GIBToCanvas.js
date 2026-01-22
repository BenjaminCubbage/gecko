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

        const compressedBitonal = Module.CompressedBitonal.TryReadFromBuffer(
            compressedBytes,
            Module.CompressedBitonal_StorageFormat.GIB
        );

        const rgba = new Uint8ClampedArray(compressedBitonal.GetWidth() *
                                           compressedBitonal.GetHeight() * 4);

        const cb = Module.addFunction((y, xStart, xEnd, white) => {
            let start = compressedBitonal.GetWidth() * y + xStart;
            let end   = start + xEnd - xStart;

            for (let i = start; i <= end; ++i) {
                rgba[i * 4 + 0] = white ? 255 : 0;
                rgba[i * 4 + 1] = rgba[i * 4 + 0];
                rgba[i * 4 + 2] = rgba[i * 4 + 0];
                rgba[i * 4 + 3] = 255;
            }
        }, "viiii");

        Module.Decoder.TryDecompress(compressedBitonal, cb);
        Module.removeFunction(cb);


        const width     = compressedBitonal.GetWidth();
        const height    = compressedBitonal.GetHeight();

        const imageData = new ImageData(rgba, width, height);

        ctx.putImageData(imageData, 0, 0);

        compressedBitonal.delete();
        compressedBytes.delete();
    }
};

export { GIBToCanvas };