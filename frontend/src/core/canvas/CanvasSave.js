import { FileSystem } from '../fs/FileSystem.js';

class CanvasSave {
    static async promptSaveCompressed(canvas) {
        const ctx = canvas?.getContext('2d');
        const bgr = new Module.VectorUint8();

        if (!ctx) {
            console.warn('`canvas?.getContext("2d")` was null or undefined.');
            return;
        }

        const width  = canvas.width;
        const height = canvas.height;
        const canvasBGR = ctx.getImageData(0, 0, width, height);

        // 4-channel RGBA to 3-channel BGR
        const pixelCount = canvasBGR.data.length / 4;
        bgr.resize(pixelCount * 3);

        for (let i = 0; i < pixelCount; ++i) {
            const pixelBecomesWhite = 
                canvasBGR.data[i * 4 + 0] > 240 ||
                canvasBGR.data[i * 4 + 1] > 240 ||
                canvasBGR.data[i * 4 + 2] > 240;
            bgr.set(i * 3 + 0, pixelBecomesWhite * 255);
            bgr.set(i * 3 + 1, pixelBecomesWhite * 255);
            bgr.set(i * 3 + 2, pixelBecomesWhite * 255);
        }

        let uncompressedBitonal = new Module.UncompressedBitonal(bgr, width, height);
        let compressedBitonal   = Module.Encoder.TryCompressBitonal(uncompressedBitonal);
        let compressedBytes     = Module.CompressedBitonal.TryWriteToBuffer(
            compressedBitonal,
            Module.CompressedBitonal_StorageFormat.BDC
        );

        const size = compressedBytes.size();
        const copy = new Uint8Array(size);
        for (let i = 0; i < size; ++i)
            copy[i] = compressedBytes.get(i);

        FileSystem.promptSaveFileBuffer(copy, 'Image.bdc', 'application/vnd.gecko.bdc');

        compressedBytes.delete();
        compressedBitonal.delete();
        uncompressedBitonal.delete();
        bgr.delete();
    }
}

export { CanvasSave };