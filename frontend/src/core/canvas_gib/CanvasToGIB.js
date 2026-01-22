class CanvasToGIB {
    static readBlob(canvas) {
        console.time("total");
        const ctx = canvas?.getContext('2d');
        const bgr = new Module.VectorUint8();

        if (!ctx) {
            console.warn('`canvas?.getContext("2d")` was null or undefined.');
            return;
        }

        const width  = canvas.width;
        const height = canvas.height;
        const canvasBGR = ctx.getImageData(0, 0, width, height);

        const cb = Module.addFunction((pixelX, pixelY) => {
            const pos = pixelY * width + pixelX;
            return canvasBGR.data[pos * 4 + 0] > 240 ||
                   canvasBGR.data[pos * 4 + 1] > 240 ||
                   canvasBGR.data[pos * 4 + 2] > 240;
        }, "iii");

        const compressedBitonal = Module.Encoder.TryCompress(cb, width, height);
        const compressedBytes = Module.CompressedBitonal.TryWriteToBuffer(
            compressedBitonal,
            Module.CompressedBitonal_StorageFormat.GIB
        );

        const size = compressedBytes.size();
        const copy = new Uint8Array(size);
        for (let i = 0; i < size; ++i)
            copy[i] = compressedBytes.get(i);

        compressedBytes.delete();
        compressedBitonal.delete();
        bgr.delete();
        return new Blob([copy]);
    }
};

export { CanvasToGIB };