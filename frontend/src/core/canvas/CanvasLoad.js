import { FileSystem } from '../fs/FileSystem.js';

class CanvasLoad {
    static async promptLoadCompressed(canvas) {
        const ctx = canvas?.getContext('2d');
        if (!ctx) throw new Error('canvas?.getContext("2d") was null.');

        const buffer = await FileSystem.promptLoadFileBuffer('.bdc');

        const compressedBytes = new Module.VectorUint8();
        compressedBytes.resize(buffer.length);

        for (let i = 0; i < buffer.length; ++i)
            compressedBytes.set(i, buffer[i]);

        const compressedBitonal = Module.CompressedBitonal.TryReadFromBuffer(
            compressedBytes,
            Module.CompressedBitonal_StorageFormat.BDC
        );

        const uncompressedBitonal = Module.Decoder.TryDecompressBitonal(compressedBitonal);
        const uncompressedBytes   = uncompressedBitonal.GetBGR();

        /*
         *      BGR to RGBA 
         */
        const pixelCount = uncompressedBytes.size() / 3;
        const rgba = new Uint8ClampedArray(pixelCount * 4);
        for (let i = 0; i < pixelCount; ++i) {
            rgba[i * 4 + 0] = uncompressedBytes.get(i * 3 + 0);
            rgba[i * 4 + 1] = uncompressedBytes.get(i * 3 + 1);
            rgba[i * 4 + 2] = uncompressedBytes.get(i * 3 + 2);
            rgba[i * 4 + 3] = 255;
        }

        const width  = uncompressedBitonal.GetWidth();
        const height = uncompressedBitonal.GetHeight();

        const imageData = new ImageData(rgba, width, height);

        ctx.putImageData(imageData, 0, 0);

        uncompressedBytes.delete();
        uncompressedBitonal.delete();
        compressedBitonal.delete();
        compressedBytes.delete();
    }
}

export { CanvasLoad };