import BMP from './bmp.js';

export default class BMPBuilder {
    static bmpFromCanvasCTX(ctx) {
        const imageData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);
        const width = imageData.width;
        const height = imageData.height;
        const rowSize = Math.ceil((width * 3) / 4) * 4;

        const bytes = new Uint8Array(54 + rowSize * height);
        // BMP Header
        bytes[0] = 0x42; // 'B'
        bytes[1] = 0x4D; // 'M'

        // File size

        // File size
        const fileSize = bytes.length;
        bytes[2] = fileSize         & 0xFF;
        bytes[3] = (fileSize >> 8)  & 0xFF;
        bytes[4] = (fileSize >> 16) & 0xFF;
        bytes[5] = (fileSize >> 24) & 0xFF;

        // Reserved
        bytes[6] = 0x00;
        bytes[7] = 0x00;
        bytes[8] = 0x00;
        bytes[9] = 0x00;

        // Pixel data offset
        bytes[10] = 54;
        bytes[11] = 0x00;
        bytes[12] = 0x00;
        bytes[13] = 0x00;

        // DIB Header
        bytes[14] = 40;
        bytes[15] = 0x00;
        bytes[16] = 0x00;
        bytes[17] = 0x00;

        // Image width
        bytes[18] = width         & 0xFF;
        bytes[19] = (width >> 8)  & 0xFF;
        bytes[20] = (width >> 16) & 0xFF;
        bytes[21] = (width >> 24) & 0xFF;

        // Image height
        bytes[22] = height         & 0xFF;
        bytes[23] = (height >> 8)  & 0xFF;
        bytes[24] = (height >> 16) & 0xFF;
        bytes[25] = (height >> 24) & 0xFF;

        // Planes - always 1
        bytes[26] = 1;
        bytes[27] = 0x00;

        // Bits per pixel - 24 for RGB
        bytes[28] = 24;
        bytes[29] = 0x00;

        // Compression - none
        bytes[30] = 0x00;
        bytes[31] = 0x00;
        bytes[32] = 0x00;
        bytes[33] = 0x00;

        // Image size - can be 0 for no compression
        bytes[34] = 0x00;
        bytes[35] = 0x00;
        bytes[36] = 0x00;
        bytes[37] = 0x00;

        // Resolution (pixels per meter) - set to 0 (no printing)
        bytes[38] = 0x00;
        bytes[39] = 0x00;
        bytes[40] = 0x00;
        bytes[41] = 0x00;
        bytes[42] = 0x00;
        bytes[43] = 0x00;
        bytes[44] = 0x00;
        bytes[45] = 0x00;

        // Colors in palette - 0 for no palette
        bytes[46] = 0x00;
        bytes[47] = 0x00;
        bytes[48] = 0x00;
        bytes[49] = 0x00;

        // "Important colors" - not useful
        bytes[50] = 0x00;
        bytes[51] = 0x00;
        bytes[52] = 0x00;
        bytes[53] = 0x00;

        // Pixel data
        for (let y = 0; y < height; y++) {
            for (let x = 0; x < width; x++) {
                const pixelIndex = (y * width + x) * 4;
                const rowIndex = (height - 1 - y) * rowSize + x * 3 + 54;

                bytes[rowIndex + 0] = imageData.data[pixelIndex + 2] == 255 ? 255 : 0;
                bytes[rowIndex + 1] = imageData.data[pixelIndex + 1] == 255 ? 255 : 0;
                bytes[rowIndex + 2] = imageData.data[pixelIndex + 0] == 255 ? 255 : 0;
            }
        }

        return new BMP(bytes.buffer);
    }
}